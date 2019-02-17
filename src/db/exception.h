#pragma once

#include <exception>
#include <string>
#include <fmt/format.h>
#include "blockchain/address.h"
#include "blockchain/block.h"
#include "db/db.h"

namespace ICONation::Aegis::Db::Exception
{
    struct BlockDoesntExist : public std::runtime_error
    {
        BlockDoesntExist (const SDK::Blockchain::Block::Hash &hash) throw()
        :   std::runtime_error (fmt::format ("Block '{}' doesn't exist", hash)) {}

        BlockDoesntExist (const Db::Db::Id &id) throw()
        :   std::runtime_error (fmt::format ("Block ID '{}' doesn't exist", id)) {}
    };

    struct TokenDoesntExist : public std::runtime_error
    {
        TokenDoesntExist (const SDK::Blockchain::Token::Name &name) throw()
        :   std::runtime_error (fmt::format ("Token '{}' doesn't exist", name)) {}

        TokenDoesntExist (const Db::Db::Id &id) throw()
        :   std::runtime_error (fmt::format ("Token ID '{}' doesn't exist", id)) {}
    };
}
