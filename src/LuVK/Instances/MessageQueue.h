#pragma once

#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <queue>
#include <string>

namespace Luwow::LuVK {
    struct Message {
        std::string topic;
        std::string body;
    };

    class MessageQueue {
        public:
            void push(Message item) {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_queue.push(item);
                m_cond.notify_one();
            }

            Message pop() {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_cond.wait(lock, [this]() { return !m_queue.empty(); });
                
                Message item = m_queue.front();
                m_queue.pop();
                
                return item;
            }

        private:
            std::queue<Message> m_queue;
            std::mutex m_mutex;
            std::condition_variable m_cond;
    };

    /*
    class MessageQueue {
        public:
            void push(Message msg) {
                std::lock_guard<std::mutex> lk(mut); 
                queue.push(std::move(msg));
                cv.notify_one();
            }

            Message pop() {
                std::unique_lock<std::mutex> lk(mut);
                cv.wait(lk, [&]{ return !queue.empty() || stopped; });
                if (queue.empty()) return Message {};

                Message msg = std::move(queue.front()); 
                queue.pop();

                return msg;
            }

            void stop() {
                std::lock_guard<std::mutex> lk(mut); 
                stopped = true; 
                cv.notify_all();
            }
        private:
            std::mutex mut;
            std::condition_variable cv;
            std::queue<Message> queue;
            bool stopped = false;
    };
    */

}