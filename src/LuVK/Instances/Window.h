#pragma once

#include "IGuiModule.h"
#include <GLFW/glfw3.h>

namespace Luwow::LuVK {
    class Window : public IWindow {
        public:
            Window(const WindowDescriptor& descriptor);
            ~Window();

            Window(const Window &) = delete;
            Window &operator=(const Window &) = delete;

            virtual void initialize() override;

            // Accessor
            WindowDescriptor getDescriptor() const { return descriptor; };
        private:
            WindowDescriptor descriptor;

            // GLFW Window Object
            GLFWwindow *window;
    };

    void getWindowTable(lua_State* L, Window* window);
} // namespace Luwow::LuVK