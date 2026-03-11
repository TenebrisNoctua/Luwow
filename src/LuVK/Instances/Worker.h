#pragma once

#include "lua.h"
#include "lualib.h"

#include "MessageQueue.h"

#include <thread>
#include <atomic>
#include <queue>

namespace Luwow::LuVK {
    class Worker {
        public:
            int id;
            std::thread thread;
            std::atomic<bool> running {true};

            lua_State* L = nullptr;
            MessageQueue* queue;
    };

    // The thread the worker will run on.
    void workerThread(Worker* worker, const std::string name, const std::string bytecode, MessageQueue* queue);
}

