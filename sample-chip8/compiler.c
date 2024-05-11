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

static void assert_stack()
{
    if (lua_gettop(L) != 1) {
        fprintf(stderr, "Lua stack size is different than 1.\n");
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
    lua_getfield(L, -1, "success");
    cr.success = lua_toboolean(L, -1);
    lua_pop(L, 1);
    assert_stack();

    // info

    if (!cr.success) {
        lua_getfield(L, -1, "error_info");
        cr.error_info = copy_str(lua_tostring(L, -1));
        lua_pop(L, -1);
        return cr;
    } else {
        lua_getfield(L, -1, "result_info");
        if (!lua_isnil(L, -1))
            cr.result_info = copy_str(lua_tostring(L, -1));
        lua_pop(L, 1);
    }
    assert_stack();

    // binaries

    lua_getfield(L, -1, "binaries");

    cr.binaries_n = luaL_len(L, -1);
    cr.binaries = calloc(cr.binaries_n, sizeof(Binary));

    size_t i = 0;
    lua_pushnil(L);
    while (lua_next(L, -2)) {
        // -1 = binary, -2 = binaries

        // rom
        lua_getfield(L, -1, "rom");
        cr.binaries[i].rom_sz = luaL_len(L, -1);
        cr.binaries[i].rom = malloc(cr.binaries[i].rom_sz);
        for (size_t j = 0; j < cr.binaries[i].rom_sz; ++j) {
            lua_rawgeti(L, -1, (lua_Integer) j + 1);
            cr.binaries[i].rom[j] = (uint8_t) lua_tointeger(L, -1);
            lua_pop(L, 1);
        }
        lua_pop(L, 1);

        lua_getfield(L, -1, "load_pos");
        cr.binaries[i].load_pos = lua_isnil(L, -1) ? 0 : lua_tointeger(L, -1);

        lua_pop(L, 2);
    }
    lua_pop(L, 1);
    assert_stack();



    return cr;
}