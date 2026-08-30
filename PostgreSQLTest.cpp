#pragma once

#include <string>
#include <iostream>
#include <locale>

#include <pqxx/pqxx>

#include "ClientData.h"

#pragma comment(lib, "ws2_32.lib")

int main()
{
    setlocale(LC_ALL, "Russian");

    try
    {
        // Подключение к PostgreSQL

        pqxx::connection connection(
            "host=localhost "
            "port=5432 "
            "dbname=postgres "
            "user=postgres "
            "password=timoha2211"
        );

        std::cout << "Подключение успешно!"
            << std::endl;

        // Создаём объект класса ClientDatabase

        ClientDatabase db(connection);

        // 1. Создание таблиц

        db.createTables();

        // 2. Добавление клиентов

        int client1 = db.addClient(
            "Иван",
            "Иванов",
            "ivan@example.com"
        );

        int client2 = db.addClient(
            "Пётр",
            "Петров",
            "petr@example.com"
        );

        int client3 = db.addClient(
            "Анна",
            "Смирнова",
            "anna@example.com"
        );

        // 3. Добавление телефонов

        // У Ивана два телефона
        db.addPhone(
            client1,
            "+7-900-111-11-11"
        );

        db.addPhone(
            client1,
            "+7-900-222-22-22"
        );

        // У Петра один телефон
        db.addPhone(
            client2,
            "+7-900-333-33-33"
        );

        // У Анны телефона нет.
        // Это разрешено условием задания.
       
        // Показываем всех клиентов

        db.showAllClients();

        // 4. Изменение данных клиента

        db.updateClient(
            client1,
            "Иван",
            "Иванов",
            "ivanov@example.com"
        );

        // 7. Поиск клиента

        std::cout << "\nПоиск по имени:"
            << std::endl;

        db.findClient("Иван");

        std::cout << "\nПоиск по фамилии:"
            << std::endl;

        db.findClient("Петров");

        std::cout << "\nПоиск по email:"
            << std::endl;

        db.findClient("anna@example.com");

        std::cout << "\nПоиск по телефону:"
            << std::endl;

        db.findClient("333-33-33");

        // 5. Удаление телефона

        // В нашей демонстрации телефон с ID = 1
        // принадлежит Ивану.

        db.deletePhone(1);

        // 6. Удаление клиента

        // Удаляем Петра.
        // Его телефон также удалится автоматически,
        // благодаря ON DELETE CASCADE.

        db.deleteClient(client2);

        // Финальный вывод


        db.showAllClients();
    }
    catch (const pqxx::sql_error& e)
    {
        std::cerr << "\nОшибка SQL: "
            << e.what()
            << std::endl;

        std::cerr << "SQL-запрос: "
            << e.query()
            << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "\nОшибка: "
            << e.what()
            << std::endl;
    }

    return 0;
}




