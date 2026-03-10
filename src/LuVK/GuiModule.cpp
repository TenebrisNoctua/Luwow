#include "GuiModule.h"
#include "Engine.h"

#include "Instances/Window.h"
#include "Instances/MessageQueue.h"
#include "Instances/Worker.h"

#include "lua.h"
#include "lualib.h"

#include <iostream>
#include <vector>
#include <cstring>
#include <thread>
#include <string>

namespace Luwow::LuVK {
    using ILuauModule = Luwow::Engine::ILuauModule;
    using Engine = Luwow::Engine::Engine;
    using Package = Luwow::Engine::Package;
    
    // Gets the module userdata from the lua state.
    static GuiModule* getModuleInstance(lua_State* L) {
        GuiModule* gui = static_cast<GuiModule*>(lua_touserdata(L, lua_upvalueindex(1)));
        if (!gui) {
            throw std::runtime_error("Gui module not found");
        }
        return gui;
    }

    // Luau Userdata Functions

    static int createWindow(lua_State* L) {
        GuiModule* gui = getModuleInstance(L);
        WindowDescriptor windowDescriptor = getWindowDescriptor(L);
        Window* window = static_cast<Window*>(gui->createWindow(windowDescriptor));
        getWindowTable(L, window);
        return 1;
    }
    
    static int createFunction(lua_State* L) {
        if (lua_gettop(L) < 2) {
            luaL_error(L, "new expects 2 arguments");
            return 0;
        }
    
        const char *name = luaL_checkstring(L, -2); 
        luaL_checktype(L, -1, LUA_TTABLE);
    
        if (strcmp(name, "Window") == 0) {
            return createWindow(L);
        } else {
            luaL_error(L, "Cannot find control type: %s", name);
        };
    
        return 0;
    }

    static int startThreads(lua_State* L) {
        GuiModule* gui = getModuleInstance(L);
        Package* package = gui->getPackage();

        if (lua_gettop(L) > 5) {
            luaL_error(L, "Only a maximum of 4 parameters allowed.");
            return 0;
        }

        auto workers = gui->workers;
        workers.reserve(4);

        for (int i = -1; i >= -4; i--) {
            const char* name = luaL_checkstring(L, i);
            int index = package->indexOfFile(name);
            if (index == -1) continue;

            std::string bytecode = package->getFileContent(index);

            auto worker = std::make_unique<Worker>();
            worker->id = i;
            worker->running = true;
            worker->thread = std::thread(workerThread, worker.get(), name, bytecode);
            workers.push_back(std::move(worker));
        }

        return 0;
    }

    static int endThreads(lua_State* L) {
        GuiModule* gui = getModuleInstance(L);
        gui->endThreads();
        return 0;
    }

    // Class Functions
    
    GuiModule::GuiModule() : engine(nullptr) {}
    
    ILuauModule* GuiModule::initialize(Engine* engine, Package* package) {
        GuiModule* gui = new GuiModule();
        gui->setEngine(engine, package);
        return gui;
    }
    
    void GuiModule::messagePump() {
        // Add message pump
    }
    
    void GuiModule::setEngine(Engine* engine, Package* package) {
        this->engine = engine;
        this->package = package;
        engine->setMessagePumpCallback(messagePump);
    }

    void GuiModule::endThreads() {
        for (auto &worker : workers) {
            MessageQueue* inbox = worker->getInbox();
            inbox->stop();
            worker->running = false;

            if (!worker->thread.joinable()) worker->thread.join();
        }
    }

    IWindow* GuiModule::createWindow(const WindowDescriptor& descriptor) {
        return new Window(descriptor);
    };
    
    const char* GuiModule::getModuleName() const {
        return "LuVK";
    }
    
    static LuauExport exports[] = {
        { "new", createFunction },
        { "startThreads", startThreads },
        { "endThreads", endThreads },
        { nullptr, nullptr }
    };
    
    const LuauExport* GuiModule::getExports() const {
        return exports;
    }
}