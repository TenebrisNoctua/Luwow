#pragma once

#include "IGuiModule.h"
#include <GLFW/glfw3.h>

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
    };

    void getWindowTable(lua_State* L, Window* window);
}