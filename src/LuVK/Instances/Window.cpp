#include "Window.h"
#include "lua.h"
#include "lualib.h"

#include <iostream>
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Luwow::LuVK {
    Window::Window(const WindowDescriptor& descriptor) : descriptor(descriptor) {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(descriptor.Size.x, descriptor.Size.y, descriptor.Title.c_str(), nullptr, nullptr);
    }

    Window::~Window() {
        if (window) {
            glfwDestroyWindow(window);
            window = nullptr;
        }
        glfwTerminate();  
    }

    void Window::initialize() {
        // TODO: Add event callbacks
        while(!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    static int initialize(lua_State* L) {
        Window* window = static_cast<Window*>(lua_touserdata(L, lua_upvalueindex(1)));

        if (!window) {
            luaL_error(L, "The window object cannot be found.");
        }

        window->initialize();
        return 0;
    }

    void getWindowTable(lua_State* L, Window* window) {
        lua_createtable(L, 0, 2);
        lua_pushlightuserdata(L, window);
        lua_pushcclosure(L, &initialize, "initialize", 1);
        lua_setfield(L, -2, "initialize");
        lua_setreadonly(L, -1, 1);
    }
} // namespace Luwow::LuVK