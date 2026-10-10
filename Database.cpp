#include "Database.h"
#include <Windows.h>
#include <fstream>
//we used Singleton Pattern
// Singleton Pattern: Ensures a single database connection instance is shared globally to optimize resources.

Database::Database()
{
    std::ifstream file("db_config.txt");
    std::string password;

    std::getline(file, password);
    

    if (!file.is_open())
    {
        MessageBoxA(nullptr,
            "Cannot find db_config.txt in the working directory.",
            "Configuration Error", MB_OK | MB_ICONERROR);
        throw std::runtime_error("Cannot open db_config.txt");
    }

    std::getline(file, password);

    try
    {
        conn = std::make_unique<pqxx::connection>(
            "dbname=Lost&Found_MS user=postgres password=" + password +
            " host=127.0.0.1 port=5432"
        );

        if (!conn->is_open())
            throw std::runtime_error("Connection is not open");
    }

    catch (const std::exception& e)
    {
        MessageBoxA(nullptr, e.what(),
            "PostgreSQL Connection Error", MB_OK | MB_ICONERROR);
        throw;
    }
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