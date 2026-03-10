#pragma once

#include "ILuauModule.h"
#include "Descriptors.h"

namespace Luwow::LuVK {
    class IWindow {
        public:
            virtual ~IWindow() = default;
            virtual void initialize() = 0;
    };

    // Interface for the GUI API consistent across platforms
    class IGuiModule : public Luwow::Engine::ILuauModule {
        public:
            virtual ~IGuiModule() = default;
            virtual IWindow *createWindow(const WindowDescriptor &descriptor) = 0;
    };
};