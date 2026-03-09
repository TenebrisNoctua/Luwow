#pragma once

#include <string>
#include <functional>
#include "lua.h"

using namespace std;

typedef struct vector2 {
    float x;
    float y;
} vector2;

namespace Luwow::Luvk {
    struct WindowDescriptor {
        string Title = "";
        vector2 Size;
        vector2 Position;
    };

    // Methods for getting the descriptors from their respective Luau tables.
    WindowDescriptor getWindowDescriptor(lua_State* L);
}