#include <lua.hpp>

#include "../libfdbg-client.hh"

#define CLIENT ((FdbgClient *) luaL_checkudata(L, 1, "FdbgClient"))
#define CHECK(f) try { \
                    f;  \
                 } catch (std::exception& e) { \
                    luaL_error(L, "%s", e.what()); \
                    return 0; \
                 }

static const luaL_Reg client_methods[] = {

    // METAMETHODS

    { "__gc", [](lua_State* L) {
        CLIENT->~FdbgClient();
        return 0;
    }},

    { "__index", [](lua_State* L) {
        luaL_getmetatable(L, "FdbgClient");
        lua_pushvalue(L, 2);
        lua_rawget(L, -2);
        if (lua_isnil(L, -1))
            luaL_error(L, "Method '%s' not found", lua_tostring(L, 2));
        return 1;
    }},

    // METHODS

    { "load_user_definition", [](lua_State* L) {
        CHECK(CLIENT->load_user_definition(luaL_checkstring(L, 2)))
        return 0;
    }},

    { "connect", [](lua_State* L) {
        CHECK(CLIENT->connect(luaL_checkstring(L, 2), luaL_checkinteger(L, 3)));
        return 0;
    }},

    {nullptr, nullptr}
};

// STATIC METHODS

static const luaL_Reg client_lib[] = {

        { "new", [](lua_State* L) {
            // create object
            auto client = (FdbgClient *) lua_newuserdata(L, sizeof(FdbgClient));
            client = new (client) FdbgClient();

            // add destructor
            luaL_newmetatable(L, "FdbgClient");
            luaL_setfuncs(L, client_methods, 0);

            lua_setmetatable(L, -2);

            return 1;
        }},

        { "start_emulator", [](lua_State* L) {
            CHECK(lua_pushstring(L, CLIENT->start_emulator(luaL_checkstring(L, 2)).c_str()))
            return 1;
        }},

        {nullptr, nullptr}
};

// library declaration

extern "C" {

int luaopen_fdbg_client(lua_State *L)
{
    luaL_newlib(L, client_lib);

    return 1;
}

}