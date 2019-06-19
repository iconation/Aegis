#include "blockdownloader.h"
#include "sdk/sdk.h"
#include "threadpool/threadpool.h"
#include "common/dbg/dbg.h"
#include <chrono>
#include <iostream>

using namespace ICONation::SDK::Blockchain;

namespace ICONation::BlockDownloader
{
    Client::Client (int threads, const std::string &endpoint)
    :   m_endpoint (endpoint),
        m_threads (threads)
    {
    }

    Block Client::get_block (const Block::Height &height)
    {
        Block result;
        bool wait = true;

        while (wait)
        {
            // Protect m_cache
            m_mutex.lock();
            if (m_cache.find (height) != m_cache.end()) {
                result = m_cache[height];
                wait = false;
            }
            m_mutex.unlock();

            // Wait a little bit for the cache
            std::this_thread::sleep_for (std::chrono::milliseconds (1));
        }

        return result;
    }

    void Client::start_download (Block::Height current, Block::Height target)
    {
        m_thread = std::thread (&Client::download, this, current, target);
    }

    void Client::join (void) 
    {
        return m_thread.join();
    }

    void Client::download (Block::Height current, Block::Height target)
    {
        const std::string endpoint = m_endpoint;
        m_running = true;
        Block::Height lastBlockDownloaded = current;

        // Download the whole gap
        while (m_running && lastBlockDownloaded <= target)
        {
            current = lastBlockDownloaded;
            progschj::ThreadPool blockPool (m_threads);
            std::vector<std::future<Block>> blocks;

            // Launch m_threads asynchronous requests
            for (int i = 0; (i < m_threads) && (current <= target); i++)
            {
                blocks.emplace_back (
                    blockPool.enqueue([endpoint, current, target] {
                        std::unique_ptr<SDK::Client> client = std::make_unique<SDK::Client> (endpoint);
                        while (true) {
                            try {
                                return client->get_block_by_height (current);
                            } catch (std::exception &e) {
                                Common::Dbg::error ("Application exception :");
                                Common::Dbg::error ("    - Type   : {}", typeid(e).name());
                                Common::Dbg::error ("    - Reason : {}", e.what());
                                Common::Dbg::warn ("Retrying to download block {} ...", current);
                            }
                        }
                    })
                );

                current += 1;
                // Don't spam too much, limit 200 requests / second should be enough
                std::this_thread::sleep_for (std::chrono::milliseconds (5));
            }

            // Put the results in cache
            for (auto &&result : blocks)
            {
                try {
                    Block block = result.get();

                    // Protect m_cache
                    m_mutex.lock();
                    m_cache [block.height()] = block;
                    m_mutex.unlock();
                    lastBlockDownloaded = block.height();

                } catch (std::exception &e) {
                    Common::Dbg::error ("Application exception :");
                    Common::Dbg::error ("    - Type   : {}", typeid(e).name());
                    Common::Dbg::error ("    - Reason : {}", e.what());
                    Common::Dbg::warn ("Retrying to insert block in cache {} ...", lastBlockDownloaded);
                    break;
                }
            }
        }

        stop();
    }
}