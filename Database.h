#pragma once
#define _SILENCE_NONFLOATING_COMPLEX_DEPRECATION_WARNING
#include <pqxx/pqxx>
#include <memory>
#include <string>


class Database
{
public:
    static Database& getInstance();
    pqxx::connection& getConnection();

    // Used for SELECT queries: executes the SQL statement within a transaction and returns the resulting dataset (pqxx::result)
    pqxx::result executeQuery(std::string sql);

    // Used for INSERT, UPDATE, and DELETE queries: modifies data, commits the transaction, and returns true on success or false on error
    bool executeNonSelect(std::string sql);

private:
    Database();
    std::unique_ptr<pqxx::connection> conn;
};