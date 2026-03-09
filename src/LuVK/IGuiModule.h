#pragma once

#include "ILuauModule.h"
#include "Descriptors.h"

namespace Luwow::Luvk {
    class IWindow {
        public:
            virtual ~IWindow() = default;
            virtual void toggleVisibility(bool enabled) = 0;
    };

    // Interface for the GUI API consistent across platforms
    class IGuiModule : public Luwow::Engine::ILuauModule {
        public:
            virtual ~IGuiModule() = default;
            virtual IWindow *createWindow(const WindowDescriptor &descriptor) = 0;
    };
};