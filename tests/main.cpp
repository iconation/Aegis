#include <gtest/gtest.h>
#include "db/db.h"
#include "common/dbg/dbg.h"
#include <chrono>
#include <thread>

using namespace ICONation::Common;
using namespace ICONation::SDK::Blockchain;
using namespace ICONation::Aegis;

#define DEBUG_TRY_CATCH(_x) \
    try { _x; } catch (const std::exception &e) { \
        Dbg::error ("Application exception :"); \
        Dbg::error ("    - Type   : {}", typeid(e).name()); \
        Dbg::error ("    - Reason : {}", e.what()); \
    }

namespace ICONation::SDK::Tests
{
    const int TEST_PORT = 33060;
    const std::string TEST_HOST = "localhost";
    const std::string TEST_USER = "icon";
    const std::string TEST_PASSWORD = "icon";
    const std::string TEST_SCHEMA = "iconation_test";

    Db::Db db (TEST_HOST, TEST_PORT, TEST_USER, TEST_PASSWORD, TEST_SCHEMA);
    SDK::Client client ("http://iconation.team:9000/api/v3", EULJIRO);

    TEST (Db, InsertTransactionMessage)
    {
        Block block = client.get_block_by_height (56189);
        db.disable_foreign_checks();
        db.block_insert (block);
        db.enable_foreign_checks();
    }
}

int main (int argc, char **argv)
{
    using ICONation::SDK::Tests::db;
    using ICONation::SDK::Tests::client;
    using ICONation::SDK::Tests::TEST_SCHEMA;
 
    try {
        Dbg::warn ("Preparing tests...");
        db.clear (TEST_SCHEMA);
        ICX::Loop icxSupply = client.get_total_supply();
        db.bootstrap (ICX ("ICX", "ICX", icxSupply, 18));

        testing::InitGoogleTest (&argc, argv);
        return RUN_ALL_TESTS();
    } catch (const std::exception &e) {
        Dbg::error ("Application exception :");
        Dbg::error ("    - Type   : {}", typeid(e).name());
        Dbg::error ("    - Reason : {}", e.what());
    }

    return 0;
}