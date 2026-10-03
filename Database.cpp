#include "Database.h"

Database::Database()
{
    conn = std::make_unique<pqxx::connection>(
        "dbname=Lost&Found_System user=postgres password=YOUR_PASSWORD host=127.0.0.1 port=5432"
    );
}

Database& Database::getInstance()
{
    static Database instance;
    return instance;
}

pqxx::connection& Database::getConnection()
{
    return *conn;
}

pqxx::result Database::executeQuery(std::string sql)
{
    pqxx::work txn(*conn);
    pqxx::result r = txn.exec(sql);
    txn.commit();
    return r;
}

bool Database::executeNonSelect(std::string sql)
{
    try
    {
        pqxx::work txn(*conn);
        txn.exec(sql);
        txn.commit();
        return true;
    }
    catch (const std::exception& e)
    {
        return false;
    }
}