#pragma once

#include "IGuiModule.h"
#include "Engine.h"

namespace Luwow::LuVK {
    using Engine = Luwow::Engine::Engine;

    class GuiModule : public IGuiModule {
        public:
            GuiModule();
            ~GuiModule() = default;

            const char* getModuleName() const override;
            const LuauExport* getExports() const override;
            ILuauModule* initialize(Engine* engine) override;
            IWindow* createWindow(const WindowDescriptor& descriptor) override;
            static void messagePump();
        private:
            void setEngine(Engine* engine);
            Engine* engine;
    };
}