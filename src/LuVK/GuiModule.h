#pragma once

#include "IGuiModule.h"
#include "Engine.h"

#include "Instances/Worker.h"

namespace Luwow::LuVK {
    using Package = Luwow::Engine::Package;
    using Engine = Luwow::Engine::Engine;

    class GuiModule : public IGuiModule {
        public:
            GuiModule();
            ~GuiModule() = default;

            const char* getModuleName() const override;
            const LuauExport* getExports() const override;

            ILuauModule* initialize(Engine* engine, Package* package) override;
            IWindow* createWindow(const WindowDescriptor& descriptor) override;

            void endThreads();

            Package* getPackage() { return package; };
            Engine* getEngine() { return engine; };

            std::vector<std::unique_ptr<Worker>> workers;

            static void messagePump();
        private:
            void setEngine(Engine* engine, Package* package);

            Engine* engine;
            Package* package;
    };
}