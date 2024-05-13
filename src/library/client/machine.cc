#include "machine.hh"

#include <lua.hpp>

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
    if (mandatory && lua_isnil(L, -1)) {
        fprintf(stderr, "Field %s missing from lua response.\n", field);
        exit(1);
    }
}

void Machine::assert_stack(int sz) const
{
    if (lua_gettop(L) != sz) {
        fprintf(stderr, "Lua stack size is different than %d.\n", sz);
        exit(1);
    }
}

void Machine::load_user_definition(std::string const &filename)
{
    int r = luaL_loadfile(L, filename.c_str());
    if (r != LUA_OK)
        throw std::runtime_error("Lua error loading the machine definition");

    r = lua_pcall(L, 0, 1, 0);
    if (r != LUA_OK)
        throw std::runtime_error(lua_tostring(L, -1));

    get_field("machine", true);
    get_field("id", true); id = luaL_checkinteger(L, -1); lua_pop(L, 1);
    get_field("name", true); name = luaL_checkstring(L, -1); lua_pop(L, 1);
    get_field("total_memory", true); total_memory = luaL_checkinteger(L, -1); lua_pop(L, 1);
    lua_pop(L, 1);

    get_field("microcontroller", true);
    get_field("uart_baud_rate", true); uc_baudrate = (int) luaL_checkinteger(L, -1); lua_pop(L, 1);
    get_field("vendor_id", true); vendor_id = luaL_checkstring(L, -1); lua_pop(L, 1);
    get_field("product_id", true); product_id = luaL_checkstring(L, -1); lua_pop(L, 1);
    lua_pop(L, 1);

    assert_stack(1);
}
