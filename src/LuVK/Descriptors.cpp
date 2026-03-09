#include "Descriptors.h"
#include "lua.h"
#include "lualib.h"
#include "IGuiModule.h"
#include <iostream>

#define DEFAULTX 100
#define DEFAULTY 100

namespace Luwow::Luvk {
    // Helper function to get the WindowDescriptor from the table.
    WindowDescriptor getWindowDescriptor(lua_State* L) {
        WindowDescriptor windowDescriptor;
        luaL_checktype(L, -1, LUA_TTABLE);

        lua_getfield(L, -1, "Title");
        windowDescriptor.Title = luaL_checkstring(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "Size");
        const float* siz = luaL_optvector(L, -1, 0);
        if (siz == 0) {
            windowDescriptor.Size = {.x = DEFAULTX, .y = DEFAULTY}; // No vector provided from Luau, use a default value.
        } else {
            windowDescriptor.Size = {.x = siz[0], .y = siz[1]};
        }
        lua_pop(L, 1);

        lua_getfield(L, -1, "Position");
        const float* pos = luaL_optvector(L, -1, 0);
        if (pos == 0) {
            windowDescriptor.Position = {.x = DEFAULTX, .y = DEFAULTY}; // No vector provided from Luau, use a default value.
        } else {
            windowDescriptor.Position = {.x = pos[0], .y = pos[1]};
        }
        lua_pop(L, 1);

        return windowDescriptor;
    }
}