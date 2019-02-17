#pragma once

#include <mysqlx/xdevapi.h>
#include <memory>

namespace ICONation::Aegis::Db
{
    const int DEFAULT_PORT = 33060;
    const std::string DEFAULT_HOST = "localhost";
    const std::string DEFAULT_USER = "icon";
    const std::string DEFAULT_PASSWORD = "icon";
    const std::string DEFAULT_SCHEMA = "iconation";

    class SQL
    {
        public:
            SQL (
                const std::string &host, 
                const unsigned port, 
                const mysqlx::string &user, 
                const std::string &password,
                const std::string &schema
            );

            ~SQL() = default;

        // MySQL Session
        public:
            std::unique_ptr<mysqlx::Session> &session (void) { return m_session; }
        private:
            std::unique_ptr<mysqlx::Session> m_session;

        // MySQL Schema
        public:
            std::unique_ptr<mysqlx::Schema> &schema (void) { return m_schema; }
        private:
            std::unique_ptr<mysqlx::Schema> m_schema;
    };
}