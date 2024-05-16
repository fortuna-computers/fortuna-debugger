#include "machine.hh"

#include <lua.hpp>

#include <string>
#include <stdexcept>

using namespace std::string_literals;

Machine::Machine()
    : L(luaL_newstate())
{
    luaL_openlibs(L);
}

Machine::~Machine()
{
    lua_close(L);
}

void Machine::get_field(const char* field, bool mandatory) const
{
    lua_getfield(L, -1, field);
    if (mandatory && lua_isnil(L, -1))
        throw std::runtime_error("Field '"s + field + "' missing from lua response");
}

void Machine::assert_stack(int sz) const
{
    if (lua_gettop(L) != sz)
        throw std::runtime_error("Lua stack size is different than "s + std::to_string(sz));
}

bool Machine::field_bool(const char *field_name, bool mandatory) const
{
    get_field(field_name, mandatory);
    bool value = lua_isnil(L, -1) ? false : lua_toboolean(L, -1);
    lua_pop(L, 1);
    return value;
}

int Machine::field_int(const char* field_name, bool mandatory) const
{
    get_field(field_name, mandatory);
    int value = lua_isnil(L, -1) ? 0 : luaL_checkinteger(L, -1);
    lua_pop(L, 1);
    return value;
}

std::string Machine::field_str(const char* field_name, bool mandatory) const
{
    get_field(field_name, mandatory);
    std::string value = lua_isnil(L, -1) ? "" : luaL_checkstring(L, -1);
    lua_pop(L, 1);
    return value;
}

std::vector<uint8_t> Machine::field_byte_array(const char* field_name, bool mandatory) const
{
    int stack_sz = lua_gettop(L);

    get_field(field_name, mandatory);
    size_t field_len = lua_isnil(L, -1) ? 0 : luaL_len(L, -1);

    std::vector<uint8_t> value;
    value.reserve(field_len);
    for (size_t i = 0; i < field_len; ++i) {
        lua_rawgeti(L, -1, (lua_Integer) i + 1);
        value.push_back((uint8_t) luaL_checkinteger(L, -1));
        lua_pop(L, 1);
    }

    lua_pop(L, 1);

    assert_stack(stack_sz);
    return value;
}

std::vector<std::string> Machine::field_string_array(const char* field_name, bool mandatory) const
{
    int stack_sz = lua_gettop(L);

    get_field(field_name, mandatory);
    size_t field_len = lua_isnil(L, -1) ? 0 : luaL_len(L, -1);

    std::vector<std::string> value;
    value.reserve(field_len);
    for (size_t i = 0; i < field_len; ++i) {
        lua_rawgeti(L, -1, (lua_Integer) i + 1);
        value.at(i) = luaL_checkstring(L, -1);
        lua_pop(L, 1);
    }

    lua_pop(L, 1);

    assert_stack(stack_sz);
    return value;
}

void Machine::load_user_definition(std::string const &filename)
{
    lua_settop(L, 0);  // clear the stack

    int r = luaL_loadfile(L, filename.c_str());
    if (r != LUA_OK)
        throw std::runtime_error("Lua error loading the machine definition");

    r = lua_pcall(L, 0, 1, 0);
    if (r != LUA_OK)
        throw std::runtime_error(lua_tostring(L, -1));

    assert_stack(1);

    get_field("machine");
    id = field_int("id");
    name = field_str("name");
    total_memory = field_int("total_memory");
    lua_pop(L, 1);

    get_field("microcontroller", false);
    if (!lua_isnil(L, -1)) {
        uc_baudrate = field_int("uart_baud_rate", false);
        vendor_id = field_str("vendor_id", false);
        product_id = field_str("product_id", false);
    }
    lua_pop(L, 1);

    assert_stack(1);

    user_definition_loaded_ = true;
}

DebugInfo Machine::compile(std::string const& filename) const
{
    if (!user_definition_loaded_)
        throw std::runtime_error("User definition not loaded yet");

    // execute compilation function

    get_field("compile", true);
    lua_pushstring(L, filename.c_str());
    int r = lua_pcall(L, 1, 1, 0);
    if (r != LUA_OK)
        throw std::runtime_error(lua_tostring(L, -1));
    if (!lua_istable(L, -1))
        throw std::runtime_error("Compilation via Lua should return a table");

    // debug info

    DebugInfo di;
    di.success = field_bool("success");
    di.result_info = field_str("result_info", false);
    di.error_info = field_str("error_info", !di.success);
    di.files = field_string_array("files", false);

    // binaries

    get_field("binaries", true);

    size_t binaries_n = luaL_len(L, -1);
    di.binaries.resize(binaries_n);
    for (size_t i = 0; i < binaries_n; ++i) {
        lua_rawgeti(L, -1, (lua_Integer) i + 1);

        // rom
        di.binaries.at(i).rom = field_byte_array("rom", true);
        di.binaries.at(i).load_pos = field_int("load_pos", true);

        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    assert_stack(2);

    // source lines

    get_field("source_lines", false);

    if (!lua_isnil(L, -1)) {
        size_t source_lines_n = luaL_len(L, -1);
        for (size_t i = 0; i < source_lines_n; ++i) {
            lua_rawgeti(L, -1, (lua_Integer) i + 1);

            std::string line = field_str("line");
            size_t file_idx = field_int("file_idx");
            size_t line_nr = field_int("line_number");

            get_field("address", false);
            uint64_t address = lua_isnil(L, -1) ? DebugInfo::NO_ADDRESS : luaL_checkinteger(L, -1);
            lua_pop(L, 2);  // clear address + current iteration

            di.source_lines[std::pair(file_idx, line_nr)] = { line, address };
        }
    }
    lua_pop(L, 1);
    assert_stack(2);

    // symbols

    get_field("symbols", false);

    if (!lua_isnil(L, -1)) {
        size_t symbols_n = luaL_len(L, -1);
        for (size_t i = 0; i < symbols_n; ++i) {
            lua_rawgeti(L, -1, (lua_Integer) i + 1);

            di.symbols[field_str("name")] = field_int("address");

            lua_pop(L, 1);
        }
    }
    lua_pop(L, 2);   // also remove debugging info object
    assert_stack(1);

    return di;
}
