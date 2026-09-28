#include "Database.h"

Database& Database::GetInstance()
{
    static Database instance;
    return instance;
}

Database::Database()
{
    conn = std::make_unique<pqxx::connection>(
        "dbname=project_db user=postgres password=284pip host=127.0.0.1 port=5432"
    );
}

pqxx::connection& Database::GetConnection()
{
    return *conn;
}