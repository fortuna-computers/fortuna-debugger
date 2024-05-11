#if 0   // Example of a compiler.lua file

return function(source_file)
    return {
        success = true,
        result_info = "Compilation successful",
        d_info = {
            files = { "MAZE.SRC" },
            source_lines = {
                { line = "    nop", file_idx=1, line_number=1, address=0x0 },
                { line = "hello:",  file_idx=1, line_number=2 },
                { line = "    nop", file_idx=1, line_number=3, address=0x1 },
            },
            symbols = {
                { name = "hello", address = 0x1 },
            },
        },
        binaries = {
            { rom = { 61, 62, 63 }, load_pos = 30 },
        }
    }
end

#endif

#include "compiler.h"

#include <string.h>
#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "compiler_lua.h"

static lua_State* L = NULL;

static void done()
{
    if (L)
        lua_close(L);
}

static char* copy_str(const char* str)
{
    if (str == NULL)
        return NULL;

    char* ptr = calloc(1, strlen(str) + 1);
    strcpy(ptr, str);
    return ptr;
}

static CompilationResult report_error(CompilationResult* cr, const char* error)
{
    cr->success = false;
    cr->error_info = copy_str(error);
    return *cr;
}

static void assert_stack(int sz)
{
    if (lua_gettop(L) != sz) {
        fprintf(stderr, "Lua stack size is different than %d.\n", sz);
        exit(1);
    }
}

static void get_field(const char* field, bool mandatory)
{
    lua_getfield(L, -1, field);
    if (mandatory && lua_isnil(L, -1)) {
        fprintf(stderr, "Field %s missing from lua response.\n", field);
        exit(1);
    }
}

CompilationResult compile(const char* source_file)
{
    CompilationResult cr;
    memset(&cr, 0, sizeof(cr));

    // load Lua script

    L = luaL_newstate();
    atexit(done);

    luaL_openlibs(L);
    int r = luaL_loadbuffer(L, (const char *) compiler_lua_out, compiler_lua_out_len, "compiler");
    if (r != LUA_OK)
        return report_error(&cr, "Lua error loading the compiler");
    if (lua_gettop(L) != 1 || !lua_isfunction(L, -1))
        return report_error(&cr, "The Lua script should return a function");

    // execute lua script

    r = lua_pcall(L, 0, 1, 0);
    if (r != LUA_OK)
        return report_error(&cr, lua_tostring(L, -1));

    // execute compilation function

    lua_pushstring(L, source_file);
    r = lua_pcall(L, 1, 1, 0);
    if (r != LUA_OK)
        return report_error(&cr, lua_tostring(L, -1));
    if (!lua_istable(L, -1))
        return report_error(&cr, "Compilation via Lua should return a table");

    //
    // parse the result
    //

    // status
    get_field("success", true);
    cr.success = lua_toboolean(L, -1);
    lua_pop(L, 1);
    assert_stack(1);

    // info

    if (!cr.success) {
        get_field("error_info", true);
        cr.error_info = copy_str(lua_tostring(L, -1));
        lua_pop(L, -1);
        return cr;
    } else {
        get_field("result_info", false);
        if (!lua_isnil(L, -1))
            cr.result_info = copy_str(lua_tostring(L, -1));
        lua_pop(L, 1);
    }
    assert_stack(1);

    // binaries

    get_field("binaries", true);

    cr.binaries_n = luaL_len(L, -1);
    cr.binaries = calloc(cr.binaries_n, sizeof(Binary));

    size_t i = 0;
    lua_pushnil(L);
    while (lua_next(L, -2)) {
        // -1 = binary, -2 = binaries

        // rom
        get_field("rom", true);
        cr.binaries[i].rom_sz = luaL_len(L, -1);
        cr.binaries[i].rom = malloc(cr.binaries[i].rom_sz);
        for (size_t j = 0; j < cr.binaries[i].rom_sz; ++j) {
            lua_rawgeti(L, -1, (lua_Integer) j + 1);
            cr.binaries[i].rom[j] = (uint8_t) luaL_checkinteger(L, -1);
            lua_pop(L, 1);
        }
        lua_pop(L, 1);

        get_field("load_pos", false);
        cr.binaries[i].load_pos = lua_isnil(L, -1) ? 0 : lua_tointeger(L, -1);

        lua_pop(L, 2);
    }
    lua_pop(L, 1);
    assert_stack(1);

    // debugging info

    get_field("d_info", true);
    assert_stack(2);

    // files

    get_field("files", true);
    cr.d_info.files_n = luaL_len(L, -1);
    cr.d_info.files = calloc(cr.d_info.files_n, sizeof(char*));

    for (size_t j = 0; j < cr.d_info.files_n; ++j) {
        lua_rawgeti(L, -1, (lua_Integer) j + 1);
        cr.d_info.files[j] = copy_str(luaL_checkstring(L, -1));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    assert_stack(2);

    // source lines

    get_field("source_lines", true);
    cr.d_info.source_lines_n = luaL_len(L, -1);
    cr.d_info.source_lines = calloc(cr.d_info.source_lines_n, sizeof(SourceLine));

    for (size_t j = 0; j < cr.d_info.source_lines_n; ++j) {

        lua_rawgeti(L, -1, (lua_Integer) j + 1);

        luaL_checktype(L, -1, LUA_TTABLE);

        get_field("line", true);
        cr.d_info.source_lines[j].line = copy_str(lua_tostring(L, -1));
        lua_pop(L, 1);

        get_field("file_idx", true);
        cr.d_info.source_lines[j].file_idx = lua_tointeger(L, -1);
        lua_pop(L, 1);

        get_field("line_number", true);
        cr.d_info.source_lines[j].line_number = lua_tointeger(L, -1);
        lua_pop(L, 1);

        get_field("address", false);
        cr.d_info.source_lines[j].line_number = lua_isnil(L, -1) ? NO_ADDRESS : lua_tointeger(L, -1);
        lua_pop(L, 2);
    }
    lua_pop(L, 1);
    assert_stack(2);

    // symbols

    get_field("symbols", false);
    if (!lua_isnil(L, -1)) {

        cr.d_info.symbols_n = luaL_len(L, -1);
        cr.d_info.symbols = calloc(cr.d_info.symbols_n, sizeof(Symbol));

        for (size_t j = 0; j < cr.d_info.symbols_n; ++j) {

            lua_rawgeti(L, -1, (lua_Integer) j + 1);

            luaL_checktype(L, -1, LUA_TTABLE);

            get_field("name", true);
            cr.d_info.source_lines[j].line = copy_str(lua_tostring(L, -1));
            lua_pop(L, 1);

            get_field("address", true);
            cr.d_info.source_lines[j].file_idx = lua_tointeger(L, -1);
            lua_pop(L, 2);
        }

    }
    lua_pop(L, 1);
    assert_stack(2);

    // end

    lua_pop(L, 1);
    assert_stack(1);

    return cr;
}