#pragma once

#include <string>
#include <pqxx/pqxx>

class ClientDatabase
{
private:
    pqxx::connection& connection;

public:
    // Конструктор
    ClientDatabase(pqxx::connection& conn);

    // 1. Создание таблиц
    void createTables();

    // 2. Добавление клиента
    int addClient(
        const std::string& firstName,
        const std::string& lastName,
        const std::string& email
    );

    // 3. Добавление телефона
    void addPhone(
        int clientId,
        const std::string& phone
    );

    // 4. Изменение данных клиента
    void updateClient(
        int clientId,
        const std::string& firstName,
        const std::string& lastName,
        const std::string& email
    );

    // 5. Удаление телефона
    void deletePhone(int phoneId);

    // 6. Удаление клиента
    void deleteClient(int clientId);

    // 7. Поиск клиента
    void findClient(const std::string& searchValue);

    // Дополнительный метод
    void showAllClients();
};