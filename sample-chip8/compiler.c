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

static CompilationResult report_error(CompilationResult* cr, const char* error)
{
    cr->success = false;

    cr->error_info = calloc(1, strlen(error));
    strncpy(cr->error_info, error, strlen(error));

    return *cr;
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

    // parse the result

    cr.success = false;

    const char* text = "There was a compilation error.";
    cr.error_info = calloc(1, strlen(text));
    strncpy(cr.error_info, text, strlen(text));

    return cr;
}