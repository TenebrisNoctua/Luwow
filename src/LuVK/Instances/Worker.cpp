#include "lua.h"
#include "lualib.h"

#include "Worker.h"
#include "MessageQueue.h"

#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <queue>
#include <string>
#include <condition_variable>
#include <unordered_map>
#include <algorithm>

namespace Luwow::LuVK {
    struct Broker {
        std::unordered_map<std::string, std::vector<std::pair<Worker*, int>>> subs;
        std::mutex mut;

        void bind(const std::string& topic, Worker* worker, int lua_ref) {
            std::lock_guard<std::mutex> lk(mut);
            subs[topic].emplace_back(worker, lua_ref);
        }

        void publish(const std::string& topic, const std::string& body) {
            std::lock_guard<std::mutex> lk(mut);
            auto it = subs.find(topic);
            if (it == subs.end()) return;

            for (auto &p : it->second) {
                Worker* worker = p.first;
                if (!worker) continue;
                worker->queue->push(Message {topic, body});
            }
        }
    } broker;
    
    static int lua_worker_send(lua_State* L) {
        Worker* worker = static_cast<Worker*>(lua_touserdata(L, lua_upvalueindex(1)));
        if (!worker) {luaL_error(L, "Invalid Worker"); return 0;};

        const char* topic = luaL_checkstring(L, 2);
        const char* msg = luaL_checkstring(L, 3);

        broker.publish(topic, msg);
        return 0;
    }
    
    static int lua_worker_bind(lua_State* L) {
        Worker* worker = static_cast<Worker*>(lua_touserdata(L, lua_upvalueindex(1)));
        if (!worker) luaL_error(L, "Invalid Worker"); return 0;

        const char* topic = luaL_checkstring(L, 2);
        if (!lua_isfunction(L, 3)) { luaL_error(L, "callback must be function"); return 0;};

        // Create a reference to the callback in this worker's Lua registry.
        lua_pushvalue(L, 3);
        int ref = lua_ref(L, LUA_REGISTRYINDEX);

        broker.bind(topic, worker, ref);
        return 0;
    }

    void registerWorkerUserdata(lua_State* L, Worker* worker) {
        lua_createtable(L, 0, 2);
        lua_pushlightuserdata(L, worker);

        lua_pushcclosure(L, &lua_worker_send, "SendMessage", 1);
        lua_setfield(L, -2, "SendMessage");

        lua_pushlightuserdata(L, worker);
        lua_pushcclosure(L, &lua_worker_bind, "BindToMessage", 1);
        lua_setfield(L, -2, "BindToMessage");

        lua_setreadonly(L, -1, 1);
        lua_setglobal(L, "Worker");
    }

    int beginLuauThread(lua_State* L, const std::string& name, const std::string& bytecode) {
        int result = luau_load(L, name.c_str(), bytecode.data(), bytecode.size(), 0);
        if (result != 0) {
            luaL_error(L, "Failed to load bytecode for module: %s", lua_tostring(L, -1));
            lua_pop(L, 1);
            return 0;
        }

        if (lua_pcall(L, 0, 1, 0) != 0) {
            luaL_error(L, "Failed to execute module: %s", lua_tostring(L, -1));
            lua_pop(L, 1);
            return 0;
        }

        return 1;
    }

    void workerThread(Worker* worker, const std::string name, const std::string bytecode, MessageQueue* queue) {
        lua_State* newState = luaL_newstate();
        worker->L = newState;
        worker->queue = queue;

        luaL_openlibs(newState);
        registerWorkerUserdata(newState, worker);
        beginLuauThread(newState, name, bytecode);

        while (worker->running.load()) {
            try {
                Message msg = queue->pop();

                // TODO: Implement main sending and binding system.
            } catch (const std::system_error &e) {
                std::cerr << e.what() << " code=" << e.code().value() << '\n';
            }
        }

        // Cleanup

        std::lock_guard<std::mutex> lk(broker.mut);
        for (auto &kv : broker.subs) {
            auto &vec = kv.second;
            vec.erase(std::remove_if(vec.begin(), vec.end(),
                [&](const std::pair<Worker*, int> &p){
                    if (p.first == worker) {
                        lua_unref(worker->L, p.second);
                        return true;
                    }
                    return false;
                }), vec.end());
        }

        lua_close(worker->L);
    }
}

