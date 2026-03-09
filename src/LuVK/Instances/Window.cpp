#include "Window.h"
#include <iostream>
#include <string>
#include "lua.h"
#include "lualib.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Luwow::Luvk {
    Window::Window(const WindowDescriptor& descriptor) : descriptor(descriptor) {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(descriptor.Size.x, descriptor.Size.y, descriptor.Title.c_str(), nullptr, nullptr);
    }

    Window::~Window() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Window::toggleVisibility(bool enabled) {
        if (enabled == true) {
            // Make window visible
        } else {
            // Make window invisible
        }
    }

    static int toggleVisibility(lua_State* L) {
        Window* window = static_cast<Window*>(lua_touserdata(L, lua_upvalueindex(1)));

        if (!window) {
            throw std::runtime_error("Window not found");
        }

        if (lua_gettop(L) < 1) {
            luaL_error(L, "toggleVisibility expects 1 boolean argument");
            return 0;
        }

        int t = lua_type(L, 1);
        if (t != LUA_TBOOLEAN) {
            luaL_error(L, "expected boolean, got %s", lua_typename(L, t));
            return 0;
        }

        bool enabled = (lua_toboolean(L, 1) != 0);

        window->toggleVisibility(enabled);
        return 0;
    }

    void getWindowTable(lua_State* L, Window* window) {
        lua_createtable(L, 0, 2);
        lua_pushlightuserdata(L, window);
        lua_pushcclosure(L, &toggleVisibility, "toggleVisibility", 1);
        lua_setfield(L, -2, "toggleVisibility");
        lua_setreadonly(L, -1, 1);
    }
} // namespace Luvk