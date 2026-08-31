
#pragma once

#include <string>
#include <vector>
#include <pqxx/pqxx>


// Структура для хранения информации о клиенте
struct Client
{
    int id;
    std::string firstName;
    std::string lastName;
    std::string email;

    // У клиента может быть несколько телефонов
    std::vector<std::string> phones;
};


class ClientDatabase
{
private:
    pqxx::connection& connection;

public:
    // Конструктор
    ClientDatabase(pqxx::connection& conn);


    // 1. Создание структуры БД
    void createTables();


    // 2. Добавление нового клиента
    int addClient(
        const std::string& firstName,
        const std::string& lastName,
        const std::string& email
    );


    // 3. Добавление телефона существующему клиенту
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


    // 7. Поиск клиентов
    // Метод возвращает найденных клиентов
    std::vector<Client> findClient(
        const std::string& searchValue
    );


    // Дополнительный метод
    // Получение всех клиентов
    std::vector<Client> getAllClients();
};
