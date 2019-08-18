#include "application.h"
#include "common/dbg/dbg.h"

using namespace ICONation::SDK::Blockchain;
using namespace ICONation::Common::Dbg;
using ICONation::Aegis::Db::Db;

namespace ICONation::Aegis
{
    Application::Application (int argc, char **argv)
    :   Common::Application::Application (argc, argv)
    {
        // Arg 1 (optional) : ICON endpoint URL
        m_endpoint = (argc > 1) ? argv[1] : "https://ctz.solidwallet.io/api/v3";
        // m_endpoint = (argc > 1) ? argv[1] : "https://bicon.net.solidwallet.io/api/v3";
        // m_endpoint = (argc > 1) ? argv[1] : "http://iconation.team:9100/api/v3";
        // Arg 2 (optional) : Number of threads
        int threads = (argc > 2) ? atoi (argv[2]) : 5;
        m_cacheSize = (argc > 3) ? atoi (argv[3]) : 50;

        // Create client
        m_client = std::make_unique<SDK::Client> (m_endpoint);

        // Create block downloader
        m_downloader = std::make_unique<BlockDownloader::Client> (threads, m_endpoint);

        Common::Dbg::info ("Launching Aegis with the following parameters :");
        Common::Dbg::info ("  Endpoint : {}", m_endpoint);
        Common::Dbg::info ("  Threads : {}", threads);
        Common::Dbg::info ("  Cache Size : {}", m_cacheSize);
    }

    Application::~Application (void)
    {
        // Stop threading before delete
        Common::Dbg::warn ("Stopping Application...");
        if (m_downloader->running()) {
            m_downloader->stop();
            m_downloader->join();
        }
    }

    void Application::print_usage (void)
    {
        info ("==========================================");
        info ("  +++ Aegis v1.1 +++++++++++++");
        info ("  Usage : {} [endpoint] [number of threads] [cache size]", m_binary_name);
        info (" Exemple : {} https://ctz.solidwallet.io/api/v3 5 50", m_binary_name);
        info ("==========================================");
    }

    void Application::bootstrap (void)
    {
        warn ("Bootstrapping...");

        // Bootstrap db
        m_db.bootstrap (m_client->icx());

        // Get & insert genesis block
        GenesisBlock genesis = m_client->get_genesis_block ();
        ::Db::Id id = m_db.genesis_insert (genesis);
    }

    void Application::check_bootstrap (void)
    {
        if (m_db.need_bootstrap()) {
            // No block has been created yet
            bootstrap();
        }
    }

    void Application::insert_cache (std::list<Block> &cache)
    {
        Common::Dbg::info ("Inserting block height {} to {} into database...", 
            cache.front().height(), cache.back().height());

        // Insert everything
        m_db.start_transaction();
        for (auto &block : cache) {
            m_db.block_insert (block);
        }
        m_db.commit();

        cache.clear();
    }

    int Application::main (void)
    {
        // Bootstrap
        check_bootstrap();

        while (true)
        {
            // Get latest block
            Block lastRemoteBlock = m_client->get_last_block();
            Block lastLocalBlock = m_db.block_get (m_db.last_block_id());

            if (lastRemoteBlock.height() == lastLocalBlock.height()) {
                break;
            }

            Common::Dbg::info ("Current height : {} | Remote height : {}", lastLocalBlock.height(), lastRemoteBlock.height());

            // Start downloading blocks
            m_downloader->start_download (lastLocalBlock.height() + 1, lastRemoteBlock.height());

            // Block cache for bulk insertion
            std::list<Block> cache;

            while (lastLocalBlock.height() < lastRemoteBlock.height())
            {
                // Get the next block height
                Block::Height newBlockHeight = lastLocalBlock.height() + 1;

                // Get the next block from cache
                Block newBlock = m_downloader->get_block (newBlockHeight);

                // Insert it in cache
                cache.emplace_back(newBlock);

                if (cache.size() > m_cacheSize) {
                    insert_cache (cache);
                }

                // Update to the next block
                lastLocalBlock = newBlock;
            }

            // Insert remaining items
            insert_cache (cache);
        }

        m_downloader->stop();
        m_downloader->join();

        return 0;
    }
}