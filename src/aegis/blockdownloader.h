#pragma once

#include "blockchain/block.h"
#include <map>
#include <mutex>
#include <thread>

namespace ICONation::BlockDownloader
{
    class Client
    {
        // Allocators
        public:
            Client (int threads, const std::string &endpoint);
            ~Client (void) = default;

        // States
        public:
            void start_download (SDK::Blockchain::Block::Height current, SDK::Blockchain::Block::Height target);
            bool running (void) { return m_running; }
            void stop (void) { m_running = false; }
        private:
            void download (SDK::Blockchain::Block::Height current, SDK::Blockchain::Block::Height target);
            bool m_running = false;

        // Endpoint
        private:
            std::string m_endpoint;

        // Threads
        private:
            int m_threads;

        // Cache
        public:
            SDK::Blockchain::Block get_block (const SDK::Blockchain::Block::Height &height);

        // Threading
        private:
            std::thread m_thread;
            std::mutex m_mutex;
        public:
            void join (void);

        // Blocks cache
        private:
            std::map<SDK::Blockchain::Block::Height, SDK::Blockchain::Block> m_cache;
    };
}