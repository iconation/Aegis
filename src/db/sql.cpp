#include "sql.h"

namespace ICONation::Aegis::Db
{
    SQL::SQL (
        const std::string &host, 
        const unsigned port, 
        const mysqlx::string &user,
        const std::string &password,
        const std::string &schema
    ) {
        m_session = std::make_unique<mysqlx::Session> (host, port, user, password);
        m_schema = std::make_unique<mysqlx::Schema> (m_session->getSchema (schema));
    }
}