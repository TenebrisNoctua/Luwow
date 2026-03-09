#include "Window.h"
#include "lua.h"
#include "lualib.h"

#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <iostream>
#include <string>

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
        toggleVisibility(false);

        std::lock_guard<std::mutex> lock(startStopMutex);
        if (window) {
            glfwDestroyWindow(window);
            window = nullptr;
        }
        glfwTerminate();
    }

    void Window::toggleVisibility(bool enabled) {
        std::lock_guard<std::mutex> lock(startStopMutex);

        if (enabled) {
            if (requestedVisible.load()) return;
            requestedVisible.store(true);

            glfwShowWindow(window);

            if (eventThreadRunning.load()) return;

            eventThreadRunning.store(true);
            eventThread = std::thread([this]() {
                // This thread must NOT make the context current if main thread owns it.
                while (eventThreadRunning.load()) {
                    if (!window) break;
                    glfwPollEvents();

                    // Check for close request from the user
                    if (glfwWindowShouldClose(window)) {
                        requestedVisible.store(false);
                        eventThreadRunning.store(false);
                        break;
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                }
            });
        } else {
            if (!requestedVisible.load()) return;
            requestedVisible.store(false);

            // Hide the window on main thread
            glfwHideWindow(window);

            // Stop the event thread
            if (!eventThreadRunning.load()) return;
            
            eventThreadRunning.store(false);
            if (eventThread.joinable()) eventThread.join();
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
} // namespace Luwow::Luvk