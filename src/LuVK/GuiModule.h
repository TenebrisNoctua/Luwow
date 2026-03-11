#pragma once

#include "IGuiModule.h"
#include "Engine.h"

#include "Instances/Worker.h"
#include "Instances/MessageQueue.h"

namespace Luwow::LuVK {
    using Package = Luwow::Engine::Package;
    using Engine = Luwow::Engine::Engine;

    class GuiModule : public IGuiModule {
        public:
            GuiModule();
            ~GuiModule() = default;

            GuiModule(const GuiModule& guiModule) = delete;

            const char* getModuleName() const override;
            const LuauExport* getExports() const override;

            ILuauModule* initialize(Engine* engine) override;
            IWindow* createWindow(const WindowDescriptor& descriptor) override;
            Engine* getEngine() { return engine; };

            void endThreads();

            Package* package;
            MessageQueue* queue;

            std::vector<std::unique_ptr<Worker>> workers;

            static void messagePump();
        private:
            void setEngine(Engine* engine);
            Engine* engine;
    };
}