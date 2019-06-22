#pragma once

#include "common/application/application.h"
#include "db/db.h"
#include "sdk/sdk.h"
#include "blockdownloader.h"

namespace ICONation::Aegis
{
    class Application : public Common::Application::Application
    {
        // Allocators
        public:
            Application (int argc, char **argv);
            ~Application (void);

        // States
        private:
            void check_bootstrap (void);
            void bootstrap (void);

        // Main loop
        private:
            int main (void) override;

        // Block Downloader
        private:
            std::unique_ptr<BlockDownloader::Client> m_downloader;

        // Usage
        public:
            void print_usage (void) override;

        // SDK Client
        private:
            std::unique_ptr<SDK::Client> m_client;
            std::string m_endpoint;

        // Database
        public:
            void insert_cache (std::list<SDK::Blockchain::Block> &cache);
        private:
            int m_cacheSize;
            Db::Db m_db;
    };
}