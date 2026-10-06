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
    pqxx::result executeQuery(std::string sql);
    bool executeNonSelect(std::string sql);

private:
    Database();
    std::unique_ptr<pqxx::connection> conn;
};