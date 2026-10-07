#include "Database.h"
#include <Windows.h>
#include <fstream>

Database::Database()
{
    std::ifstream file("db_config.txt");
    std::string password;

    std::getline(file, password);

    conn = std::make_unique<pqxx::connection>(
        "dbname=Lost&Found_MS user=postgres password=" + password +
        " host=127.0.0.1 port=5432"
    );
}


// Singleton Pattern: ensures that all classes use the same Database object
// instead of creating a new database connection for each class.
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
        pqxx::work txn(*conn);  // Starts a database transaction
        txn.exec(sql);         // Executes the SQL query
        txn.commit();          // Saves the changes to the database.
        return true;           // Indicates that the operation was successful
    }

    // Handles database errors and displays the error message to the user.
    catch (const std::exception& e)
    {
        MessageBoxA(
            nullptr,
            e.what(),
            "Database Error",
            MB_OK
        );

        return false;
    }
}