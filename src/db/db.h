#pragma once

#include "sdk/sdk.h"
#include "sql.h"
#include <cstdint>

namespace ICONation::Aegis::Db
{
    class Db
    {
        // Db specific types
        public:
            typedef uint64_t Id;

        public:
            Db (const std::string &host=DEFAULT_HOST, const int port=DEFAULT_PORT, 
                const std::string &user=DEFAULT_USER, const std::string &password=DEFAULT_PASSWORD, 
                const std::string &schema=DEFAULT_SCHEMA);
            ~Db (void) = default;

        // States
        public:
            bool need_bootstrap (void);
            void bootstrap (const SDK::Blockchain::ICX &icx);
            void clear (const std::string &schema=DEFAULT_SCHEMA);
            void start_transaction (void);
            void commit (void);
            void rollback (void);

        // Account
        public:
            Id account_insert (const SDK::Blockchain::Account &account);
            Id account_id (const SDK::Blockchain::Address &address);
            
        // Transaction
        public:
            Id transaction_id (const SDK::Blockchain::Transaction &transaction, const Id &parentBlock);
            Id transaction_insert (const SDK::Blockchain::Transaction &transaction, const Id &parentBlock);
            
        // Internal Transaction
        public:
            Id internal_transaction_insert (const SDK::Blockchain::InternalTransaction &internalTransaction, const Id &parentTransaction);
            
        // Transaction message
        public:
            void transaction_message_insert (const std::vector<unsigned char> &message, const Id &parentTransaction);

        // Block
        public:
            // Normal block
            Id block_id (const SDK::Blockchain::Block &block);
            SDK::Blockchain::Block block_get (const Db::Id &blockId);
            Id block_insert (const SDK::Blockchain::Block &block);
            // Genesis block
            Id genesis_insert (const SDK::Blockchain::GenesisBlock &block);
        private:
            Id block_insert_query (const SDK::Blockchain::Block &block);

        // Token
        public:
            Id token_insert (const SDK::Blockchain::Token &token);
        private:
            Id token_irc2_insert (const SDK::Blockchain::IRC2 &irc2);
            Id token_icx_insert (const SDK::Blockchain::ICX &icx);
        private:
            Id token_irc2_id (const SDK::Blockchain::IRC2 &irc2);
            Id token_icx_id (const SDK::Blockchain::ICX &icx);
            Id token_id (const SDK::Blockchain::Token &token);
        
        // State
        public:
            void enable_foreign_checks (void);
            void disable_foreign_checks (void);

        // Meta
        public:
            Id last_block_id (void);

        // Database
        private:
            std::unique_ptr<SQL> m_sql;
    };
}