#include "db.h"
#include "common/dbg/dbg.h"
#include "common/exception/exception.h"
#include "exception.h"

using namespace ICONation::SDK::Blockchain;
using namespace mysqlx;

namespace ICONation::Aegis::Db
{
    Db::Db (void)
    {
        m_sql = std::make_unique<SQL> (
           DEFAULT_HOST, DEFAULT_PORT, DEFAULT_USER, DEFAULT_PASSWORD, DEFAULT_SCHEMA);
    }

    static void reset_table (std::unique_ptr<SQL> &sql, const std::string &table)
    {
    }

    void Db::clear (void)
    {
        m_sql->session()->sql ("SET FOREIGN_KEY_CHECKS=0;").execute();
        m_sql->session()->sql (fmt::format ("USE {};", DEFAULT_SCHEMA)).execute();

        // Clear all these tables
        std::vector<std::string> tables = {
            "account", 
            "balance", 
            "block", 
            "icx", 
            "internal_transaction", 
            "irc2", 
            "transaction"
        };

        std::for_each (tables.begin(), tables.end(), [this] (const std::string &table) {
            m_sql->schema()->getTable(table).remove().execute();
            m_sql->session()->sql (fmt::format ("ALTER TABLE {} AUTO_INCREMENT = 1;", table)).execute();
        });

        m_sql->session()->sql ("SET FOREIGN_KEY_CHECKS=1;").execute();
    }

    void Db::start_transaction (void)
    {
        m_sql->session()->startTransaction();
    }

    void Db::commit (void)
    {
        m_sql->session()->commit();
    }

    void Db::rollback (void)
    {
        m_sql->session()->rollback();
    }

    bool Db::need_bootstrap (void)
    {
        return m_sql->schema()->getTable("block").select("id").execute().count() == 0;
    }

    void Db::bootstrap (const ICX &icx)
    {
        m_sql->session()->sql ("SET FOREIGN_KEY_CHECKS=0;").execute();

        // Insert native ICX token
        m_sql->schema()->getTable ("icx")
            .insert ("name", "symbol", "total_supply", "decimals")
            .values (icx.name(), icx.symbol(), icx.totalSupply().get_str(), icx.decimals())
            .execute();

        m_sql->session()->sql ("SET FOREIGN_KEY_CHECKS=1;").execute();
    }

    // === Account =================================================================================
    Db::Id Db::account_insert (const Account &account)
    {
        return m_sql->schema()->getTable ("account")
            .insert ("hash")
            .values (account.address().repr())
            .execute()
            .getAutoIncrementValue();
    }

    Db::Id Db::account_id (const Address &address)
    {
        Row row = m_sql->schema()->getTable ("account")
            .select ("id")
            .where ("hash = :hash")
            .bind ("hash", address.repr())
            .execute().fetchOne();
        
        if (row.isNull()) {
            // Create a new account
            return account_insert (Account (address));
        }

        return row[0].get<Id>();
    }

    // === Token =================================================================================
    Db::Id Db::token_irc2_insert (const IRC2 &irc2)
    {
        return m_sql->schema()->getTable ("irc2")
            .insert ("score", "name", "symbol", "total_supply", "decimals")
            .values (
                irc2.score().repr(),
                irc2.name(), 
                irc2.symbol(), 
                irc2.totalSupply().get_str(), 
                irc2.decimals())
            .execute()
            .getAutoIncrementValue();
    }

    Db::Id Db::token_icx_insert (const ICX &icx)
    {
        return m_sql->schema()->getTable ("icx")
            .insert ("name", "symbol", "total_supply", "decimals")
            .values (
                icx.name(), 
                icx.symbol(), 
                icx.totalSupply().get_str(), 
                icx.decimals())
            .execute()
            .getAutoIncrementValue();
    }

    Db::Id Db::token_irc2_id (const IRC2 &irc2)
    {
        Row row = m_sql->schema()->getTable ("irc2")
            .select ("id")
            .where ("score = :score")
            .bind ("score", irc2.score().repr())
            .execute().fetchOne();
        
        if (row.isNull()) {
            // Create a new token
            return token_irc2_insert (irc2);
        }

        return row[0].get<Id>();
    }

    Db::Id Db::token_icx_id (const ICX &icx)
    {
        Row row = m_sql->schema()->getTable ("icx")
            .select ("id")
            .where ("name = :name")
            .bind ("name", icx.name())
            .execute().fetchOne();
        
        if (row.isNull()) {
            // Create a new token
            return token_icx_insert (icx);
        }

        return row[0].get<Id>();
    }

    Db::Id Db::token_insert (const Token &token)
    {
        switch (token.type())
        {
            case Token::Type::ICX: {
                const ICX &icx = dynamic_cast<const ICX &>(token);
                return token_icx_insert (icx);
            } break;

            case Token::Type::IRC2: {
                const IRC2 &irc2 = dynamic_cast<const IRC2 &>(token);
                return token_irc2_insert (irc2);
            } break;

            default:
                throw Exception::TokenDoesntExist (token.name());
        }
    }

    Db::Id Db::token_id (const Token &token)
    {
        switch (token.type())
        {
            case Token::Type::ICX: {
                const ICX &icx = dynamic_cast<const ICX &>(token);
                return token_icx_id (icx);
            } break;

            case Token::Type::IRC2: {
                const IRC2 &irc2 = dynamic_cast<const IRC2 &>(token);
                return token_irc2_id (irc2);
            } break;

            default:
                throw Exception::TokenDoesntExist (token.name());
        }
    }

    // === Transaction =================================================================================
    Db::Id Db::transaction_insert (const Transaction &transaction, const Id &parentBlock)
    {
        // Make sure the destination account has been created
        account_id (transaction.to());

        Id transactionId = m_sql->schema()->getTable ("transaction")
            .insert (
                "block", "from", "to", "hash", "amount", "step_limit", "step_used", "step_price")
            .values (
                parentBlock, 
                transaction.from().repr(),
                transaction.to().repr(),
                transaction.hash().value(),
                transaction.amount().get_str(),
                transaction.stepLimit().get_ui(),
                transaction.stepUsed().get_ui(),
                transaction.stepPrice().get_ui())
            .execute()
            .getAutoIncrementValue();

        // Insert internal transactions inside the transaction
        for (const auto &internalTx : transaction.internalTransactions()) {
            internal_transaction_insert (internalTx, transactionId);
        }

        return transactionId;
    }
    
    // === Internal Transaction =================================================================================
    Db::Id Db::internal_transaction_insert (const InternalTransaction &internalTransaction, const Id &parentTransaction)
    {
        // Make sure the destination account has been created
        account_id (internalTransaction.to());

        // Get the token ID
        Id tokenId = token_id (internalTransaction.token());

        Id transactionId = m_sql->schema()->getTable ("internal_transaction")
            .insert (
                "transaction", "from", "to", "token", "token_type", "amount")
            .values (
                parentTransaction,
                internalTransaction.from().repr(),
                internalTransaction.to().repr(),
                tokenId,
                (int) internalTransaction.token().type(),
                internalTransaction.amount().get_str())
            .execute()
            .getAutoIncrementValue();

        return transactionId;
    }

    // === Block =================================================================================
    Db::Id Db::block_id (const Block &block)
    {
        Row row = m_sql->schema()->getTable ("block")
            .select ("id")
            .where ("hash = :hash")
            .bind ("hash", block.hash().repr())
            .execute().fetchOne();

        if (row.isNull()) {
            // Create a new block
            block_insert (block);
        }

        return row[0].get<Id>();
    }

    Block Db::block_get (const Db::Id &blockId)
    {
        Row row = m_sql->schema()->getTable ("block")
            .select ("height", "hash", "previous_block", "timestamp")
            .where ("id = :id")
            .bind ("id", blockId)
            .execute().fetchOne();

        if (row.isNull()) {
            throw Exception::BlockDoesntExist (blockId);
        }

        Block::Height height = row[0].get<Block::Height>();
        Block::Hash hash (row[1].get<SDK::Blockchain::Hash::Value>());
        Block::Hash previous (row[2].get<SDK::Blockchain::Hash::Value>());
        Block::Timestamp timestamp = row[3].get<Block::Timestamp>();

        return Block (height, hash, previous, timestamp);
    }

    Db::Id Db::block_insert_query (const Block &block)
    {
        return m_sql->schema()->getTable ("block")
            .insert ("previous_block", "hash", "timestamp", "height")
            .values (block.hashPreviousBlock().value(), block.hash().value(), block.timestamp(), block.height())
            .execute()
            .getAutoIncrementValue();
    }

    Db::Id Db::block_insert (const Block &block)
    {
        Result result;

        // Insert block
        Id blockId = block_insert_query (block);

        // Insert transaction inside the block
        for (const auto &tx : block.transactions()) {
            transaction_insert (tx, blockId);
        }

        return blockId;
    }

    // === Genesis =================================================================================
    Db::Id Db::genesis_insert (const GenesisBlock &genesis)
    {
        // Genesis block doesn't respect the previous block hash definition
        // Disable the foreign key checks temporarly before inserting
        m_sql->session()->sql ("SET FOREIGN_KEY_CHECKS=0;").execute();
        Id blockId = block_insert_query (genesis);
        m_sql->session()->sql ("SET FOREIGN_KEY_CHECKS=1;").execute();

        // Insert transaction inside the block
        for (const auto &account : genesis.accounts()) {
            account_insert (account);
        }

        return blockId;
    }

    // === Meta =================================================================================
    Db::Id Db::last_block_id (void)
    {
        Row row = m_sql->schema()->getTable ("block").select("max(id)").execute().fetchOne();
        return row[0].get<Id>();
    }
}