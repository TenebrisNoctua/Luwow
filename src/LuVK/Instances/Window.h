#pragma once

#include "IGuiModule.h"
#include <GLFW/glfw3.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>

namespace Luwow::Luvk {
    class Window : public IWindow {
        public:
            Window(const WindowDescriptor& descriptor);
            ~Window();
            virtual void toggleVisibility(bool enabled) override;

            // Accessors
            WindowDescriptor getDescriptor() const { return descriptor; };
        private:
            WindowDescriptor descriptor;

            // GLFW Window Object
            GLFWwindow *window;

            std::thread eventThread;
            std::atomic<bool> eventThreadRunning{false};
            std::atomic<bool> requestedVisible{false};
            std::mutex startStopMutex;
    };

    void getWindowTable(lua_State* L, Window* window);
} // namespace Luwow::Luvk