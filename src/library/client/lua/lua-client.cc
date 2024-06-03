#include <lua.hpp>

#include "../libfdbg-client.hh"

#define CLIENT ((FdbgClient *) luaL_checkudata(L, 1, "FdbgClient"))
#define CHECK(f) try { \
                    f;  \
                 } catch (std::exception& e) { \
                    luaL_error(L, "%s", e.what()); \
                    return 0; \
                 }

static void push_computer_status(lua_State* L, fdbg::ComputerStatus const& cs)
{
    lua_newtable(L);

    lua_pushinteger(L, cs.pc()); lua_setfield(L, -2, "pc");

    lua_newtable(L);
    for (int i = 0; i < cs.registers_size(); ++i) {
        lua_pushinteger(L, cs.registers(i));
        lua_rawseti(L, -2, i + 1);
    }
    lua_setfield(L, -2, "registers");

    lua_newtable(L);
    for (int i = 0; i < cs.flags_size(); ++i) {
        lua_pushboolean(L, cs.flags(i));
        lua_rawseti(L, -2, i + 1);
    }
    lua_setfield(L, -2, "flags");

    lua_newtable(L);
    for (size_t i = 0; i < cs.stack().size(); ++i) {
        lua_pushlstring(L, &cs.stack().c_str()[i], 1);
        lua_rawseti(L, -2, i + 1);
    }
    lua_setfield(L, -2, "stack");

    // TODO - add events
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

    { "set_debugging_level", [](lua_State* L) {
        std::string level = luaL_checkstring(L, 2);
        DebuggingLevel dl = DebuggingLevel::NORMAL;
        if (level == "trace")
            dl = DebuggingLevel::TRACE;
        else if (level == "debug")
            dl = DebuggingLevel::DEBUG_;
        else if (level == "normal")
            dl = DebuggingLevel::NORMAL;
        else
            luaL_error(L, "debugging level '%s' not supported", level.c_str());
        CLIENT->set_debugging_level(dl);
        return 0;
    }},

    { "load_user_definition", [](lua_State* L) {
        CHECK(CLIENT->load_user_definition(luaL_checkstring(L, 2)))
        return 0;
    }},

    { "connect", [](lua_State* L) {
        uint32_t baudrate = EMULATOR_BAUD_RATE;
        if (lua_gettop(L) > 2 && !lua_isnil(L, 3))
            baudrate = luaL_checkinteger(L, 3);
        CHECK(CLIENT->connect(luaL_checkstring(L, 2), baudrate));
        return 0;
    }},

    { "ack", [](lua_State* L) {
        CHECK({
            auto r = CLIENT->ack(luaL_checkinteger(L, 2));
            lua_newtable(L);
            lua_pushinteger(L, r.id()); lua_setfield(L, -2, "id");
            lua_pushinteger(L, r.server_sz()); lua_setfield(L, -2, "server_sz");
            lua_pushinteger(L, r.to_computer_sz()); lua_setfield(L, -2, "to_computer_sz");
            lua_pushinteger(L, r.to_debugger_sz()); lua_setfield(L, -2, "to_debugger_sz");
        });
        return 1;
    }},

    { "reset", [](lua_State* L) {
        CHECK(push_computer_status(L, CLIENT->reset()));
        return 1;
    }},

    { "write_memory", [](lua_State* L) {
        luaL_checktype(L, 4, LUA_TTABLE);
        std::vector<uint8_t> data;
        for (int i = 0; i < luaL_len(L, 4); ++i) {
            lua_rawgeti(L, 4, i + 1);
            data.push_back(lua_tointeger(L, -1));
            lua_pop(L, 1);
        }
        CHECK(CLIENT->write_memory_full(luaL_checkinteger(L, 2), luaL_checkinteger(L, 3), data, lua_toboolean(L, 4)));
        return 0;
    }},

    { "read_memory", [](lua_State* L) {
        CHECK({
            uint8_t seq = 1;
            if (lua_gettop(L) > 4)
                seq = luaL_checkinteger(L, 5);
            auto data = CLIENT->read_memory(luaL_checkinteger(L, 2), luaL_checkinteger(L, 3), luaL_checkinteger(L, 4), seq);
            lua_newtable(L);
            int i = 1;
            for (uint8_t byte: data) {
                lua_pushinteger(L, byte);
                lua_rawseti(L, -2, i++);
            }
        });
        return 1;
    }},

    { "step", [](lua_State *L) {
        // TODO - events
        CHECK(push_computer_status(L, CLIENT->step(lua_toboolean(L, 2))));
        return 1;
    }},

    { "run", [](lua_State *L) {
        CHECK(CLIENT->run(lua_toboolean(L, 2)));
        return 0;
    }},

    { "next", [](lua_State *L) {
        CHECK(CLIENT->next());
        return 0;
    }},

    { "run_status", [](lua_State* L) {
        CHECK(
            auto rs = CLIENT->run_status();
            lua_newtable(L);
            lua_pushboolean(L, rs.running()); lua_setfield(L, -2, "running");
            lua_pushinteger(L, rs.pc()); lua_setfield(L, -2, "pc");
            // TODO - events
        )
        return 1;
    }},

    { "pause", [](lua_State *L) {
        CHECK(push_computer_status(L, CLIENT->pause()));
        return 0;
    }},

    { "add_breakpoint", [](lua_State *L) {
        CHECK(
            auto bkps = CLIENT->add_breakpoint(luaL_checkinteger(L, 2));
            lua_newtable(L);
            int i = 1;
            for (auto const& bkp: bkps) {
                lua_pushinteger(L, bkp);
                lua_rawseti(L, -2, i++);
            }
        )
        return 1;
    }},

    { "remove_breakpoint", [](lua_State *L) {
        CHECK(
            auto bkps = CLIENT->remove_breakpoint(luaL_checkinteger(L, 2));
            lua_newtable(L);
            int i = 1;
            for (auto const& bkp: bkps) {
                lua_pushinteger(L, bkp);
                lua_rawseti(L, -2, i++);
            }
        )
        return 1;
    }},

    { "compile_and_write_memory", [](lua_State* L) {
        CHECK(
            CLIENT->compile_and_write_memory(luaL_checkstring(L, 2), 0, lua_toboolean(L, 3));
        )
        return 0;
    }},

    { "autodetect_usb_serial_port", [](lua_State* L) {
        auto client = CLIENT;
        std::string port = FdbgClient::autodetect_usb_serial_port(
                client->machine().vendor_id,
                client->machine().product_id
        );
        return 1;
    }},

    // STATIC METHODS

    { "start_emulator", [](lua_State* L) {
        CHECK(lua_pushstring(L, FdbgClient::start_emulator(luaL_checkstring(L, 1)).c_str()))
        return 1;
    }},

    {nullptr, nullptr}
};

// STATIC METHODS

static const luaL_Reg client_lib[] = {

        { "new", [](lua_State* L) {
            // create object
            auto client = (FdbgClient *) lua_newuserdata(L, sizeof(FdbgClient));
            new(client) FdbgClient();

            // add destructor
            luaL_newmetatable(L, "FdbgClient");
            luaL_setfuncs(L, client_methods, 0);

            lua_setmetatable(L, -2);

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