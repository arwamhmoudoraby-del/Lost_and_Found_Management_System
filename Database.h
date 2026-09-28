#pragma  once    // <-- ??? ??? ?? ?????? ??? ?? include
#include <pqxx/pqxx>
#include <memory>

class Database
{
public:
    static Database& GetInstance();
    pqxx::connection& GetConnection();

private:
    Database();
    std::unique_ptr<pqxx::connection> conn;
};