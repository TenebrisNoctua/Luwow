#pragma once

#include "IGuiModule.h"
#include <GLFW/glfw3.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>

namespace Luwow::LuVK {
    class Window : public IWindow {
        public:
            Window(const WindowDescriptor& descriptor);
            ~Window();
            virtual void initialize() override;

            // Accessors
            WindowDescriptor getDescriptor() const { return descriptor; };
        private:
            WindowDescriptor descriptor;

            // GLFW Window Object
            GLFWwindow *window;
    };

    void getWindowTable(lua_State* L, Window* window);
} // namespace Luwow::LuVK