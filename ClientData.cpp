
#include "ClientData.h"

#include <iostream>


// Конструктор
ClientDatabase::ClientDatabase(pqxx::connection& conn)
    : connection(conn)
{
}


// 1. Создание таблиц

void ClientDatabase::createTables()
{
    pqxx::work transaction(connection);

    // Таблица клиентов
    transaction.exec(R"(
        CREATE TABLE IF NOT EXISTS clients (
            id SERIAL PRIMARY KEY,
            first_name VARCHAR(100) NOT NULL,
            last_name VARCHAR(100) NOT NULL,
            email VARCHAR(255) NOT NULL UNIQUE
        );
    )");


    // Таблица телефонов
    transaction.exec(R"(
        CREATE TABLE IF NOT EXISTS phones (
            id SERIAL PRIMARY KEY,
            client_id INTEGER NOT NULL,
            phone VARCHAR(30) NOT NULL,

            CONSTRAINT fk_client
                FOREIGN KEY (client_id)
                REFERENCES clients(id)
                ON DELETE CASCADE
        );
    )");

    transaction.commit();

    std::cout << "Таблицы успешно созданы!" << std::endl;
}


// 2. Добавление нового клиента

int ClientDatabase::addClient(
    const std::string& firstName,
    const std::string& lastName,
    const std::string& email)
{
    pqxx::work transaction(connection);

    pqxx::params params;
    params.append(firstName);
    params.append(lastName);
    params.append(email);

    pqxx::result result = transaction.exec(
        R"(
            INSERT INTO clients (first_name, last_name, email)
            VALUES ($1, $2, $3)
            RETURNING id;
        )",
        params
    );

    int clientId = result[0]["id"].as<int>();

    transaction.commit();

    std::cout << "Клиент добавлен. ID = "
        << clientId
        << std::endl;

    return clientId;
}


// 3. Добавление телефона существующему клиенту

void ClientDatabase::addPhone(
    int clientId,
    const std::string& phone)
{
    pqxx::work transaction(connection);


    // Проверяем существование клиента
    pqxx::params checkParams;
    checkParams.append(clientId);

    pqxx::result result = transaction.exec(
        "SELECT id FROM clients WHERE id = $1;",
        checkParams
    );


    if (result.empty())
    {
        std::cout << "Клиент с ID "
            << clientId
            << " не найден!"
            << std::endl;

        transaction.abort();

        return;
    }


    // Добавляем телефон
    pqxx::params phoneParams;
    phoneParams.append(clientId);
    phoneParams.append(phone);

    transaction.exec(
        R"(
            INSERT INTO phones (client_id, phone)
            VALUES ($1, $2);
        )",
        phoneParams
    );


    transaction.commit();

    std::cout << "Телефон добавлен!"
        << std::endl;
}


// 4. Изменение данных клиента

void ClientDatabase::updateClient(
    int clientId,
    const std::string& firstName,
    const std::string& lastName,
    const std::string& email)
{
    pqxx::work transaction(connection);

    pqxx::params params;

    params.append(firstName);
    params.append(lastName);
    params.append(email);
    params.append(clientId);


    pqxx::result result = transaction.exec(
        R"(
            UPDATE clients
            SET
                first_name = $1,
                last_name = $2,
                email = $3
            WHERE id = $4;
        )",
        params
    );


    if (result.affected_rows() == 0)
    {
        std::cout << "Клиент с ID "
            << clientId
            << " не найден!"
            << std::endl;

        transaction.abort();

        return;
    }


    transaction.commit();

    std::cout << "Данные клиента изменены!"
        << std::endl;
}


// 5. Удаление телефона

void ClientDatabase::deletePhone(int phoneId)
{
    pqxx::work transaction(connection);

    pqxx::params params;
    params.append(phoneId);


    pqxx::result result = transaction.exec(
        "DELETE FROM phones WHERE id = $1;",
        params
    );


    if (result.affected_rows() == 0)
    {
        std::cout << "Телефон с ID "
            << phoneId
            << " не найден!"
            << std::endl;

        transaction.abort();

        return;
    }


    transaction.commit();

    std::cout << "Телефон удалён!"
        << std::endl;
}


// 6. Удаление клиента

void ClientDatabase::deleteClient(int clientId)
{
    pqxx::work transaction(connection);

    pqxx::params params;
    params.append(clientId);


    pqxx::result result = transaction.exec(
        "DELETE FROM clients WHERE id = $1;",
        params
    );


    if (result.affected_rows() == 0)
    {
        std::cout << "Клиент с ID "
            << clientId
            << " не найден!"
            << std::endl;

        transaction.abort();

        return;
    }


    transaction.commit();

    std::cout << "Клиент удалён!"
        << std::endl;

    std::cout << "Все его телефоны также удалены."
        << std::endl;
}


// 7. Поиск клиента

std::vector<Client> ClientDatabase::findClient(
    const std::string& searchValue)
{
    pqxx::work transaction(connection);


    pqxx::params params;
    params.append(searchValue);


    pqxx::result result = transaction.exec(
        R"(
            SELECT
                c.id,
                c.first_name,
                c.last_name,
                c.email,
                p.phone
            FROM clients c
            LEFT JOIN phones p
                ON c.id = p.client_id
            WHERE
                c.first_name ILIKE '%' || $1 || '%'
                OR c.last_name ILIKE '%' || $1 || '%'
                OR c.email ILIKE '%' || $1 || '%'
                OR p.phone ILIKE '%' || $1 || '%'
            ORDER BY c.id;
        )",
        params
    );


    std::vector<Client> clients;

    int previousClientId = -1;


    for (const auto& row : result)
    {
        int clientId = row["id"].as<int>();


        // Если встретили нового клиента
        if (clientId != previousClientId)
        {
            Client client;

            client.id = clientId;

            client.firstName =
                row["first_name"].c_str();

            client.lastName =
                row["last_name"].c_str();

            client.email =
                row["email"].c_str();


            clients.push_back(client);

            previousClientId = clientId;
        }


        // Добавляем телефон клиенту
        if (!row["phone"].is_null())
        {
            clients.back().phones.push_back(
                row["phone"].c_str()
            );
        }
    }


    transaction.commit();

    // Возвращаем найденных клиентов
    return clients;
}


// Дополнительный метод
// Получение всех клиентов

std::vector<Client> ClientDatabase::getAllClients()
{
    pqxx::work transaction(connection);


    pqxx::result result = transaction.exec(R"(
        SELECT
            c.id,
            c.first_name,
            c.last_name,
            c.email,
            p.phone
        FROM clients c
        LEFT JOIN phones p
            ON c.id = p.client_id
        ORDER BY c.id;
    )");


    std::vector<Client> clients;

    int previousClientId = -1;


    for (const auto& row : result)
    {
        int clientId = row["id"].as<int>();


        // Если новый клиент
        if (clientId != previousClientId)
        {
            Client client;

            client.id = clientId;

            client.firstName =
                row["first_name"].c_str();

            client.lastName =
                row["last_name"].c_str();

            client.email =
                row["email"].c_str();


            clients.push_back(client);

            previousClientId = clientId;
        }


        // Добавляем телефон
        if (!row["phone"].is_null())
        {
            clients.back().phones.push_back(
                row["phone"].c_str()
            );
        }
    }


    transaction.commit();

    return clients;
}

