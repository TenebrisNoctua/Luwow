#include "GuiModule.h"
#include "Engine.h"
#include "Window.h"

#include "lua.h"
#include "lualib.h"
#include <iostream>
#include <vector>
#include <cstring>

namespace Luwow::Luvk {
    using ILuauModule = Luwow::Engine::ILuauModule;
    using Engine = Luwow::Engine::Engine;
    
    // For all methods that require the Gui instance, we need to get it from the userdata.
    static GuiModule* getModuleInstance(lua_State* L) {
        GuiModule* gui = static_cast<GuiModule*>(lua_touserdata(L, lua_upvalueindex(1)));
        if (!gui) {
            throw std::runtime_error("Gui module not found");
        }
        return gui;
    }
    
    GuiModule::GuiModule() : engine(nullptr) {}
    
    ILuauModule* GuiModule::initialize(Engine* engine) {
        GuiModule* gui = new GuiModule();
        gui->setEngine(engine);
        return gui;
    }
    
    void GuiModule::messagePump() {
        // Add message pump
    }
    
    void GuiModule::setEngine(Engine* engine) {
        this->engine = engine;
        engine->setMessagePumpCallback(messagePump);
    }

    IWindow* GuiModule::createWindow(const WindowDescriptor& descriptor) {
        return new Window(descriptor);
    };
    
    static int createWindow(lua_State* L) {
        GuiModule* gui = getModuleInstance(L);
        WindowDescriptor windowDescriptor = getWindowDescriptor(L);
        Window* window = static_cast<Window*>(gui->createWindow(windowDescriptor));
        getWindowTable(L, window);
    
        // Store the window pointer in the table for later retrieval
        lua_setreadonly(L, -1, 0);  // Make writable
        lua_pushlightuserdata(L, window);
        lua_setfield(L, -2, "__window_ptr");
        lua_setreadonly(L, -1, 1);  // Make readonly again
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
    
    const char* GuiModule::getModuleName() const {
        return "Gui";
    }
    
    static LuauExport exports[] = {
        { "new", createFunction },
        { nullptr, nullptr }
    };
    
    const LuauExport* GuiModule::getExports() const {
        return exports;
    }
}