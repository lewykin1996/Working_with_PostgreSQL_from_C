
#include <iostream>
#include <string>

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


        // Создаём объект для работы с клиентами
        ClientDatabase database(connection);


        // 1. Создание таблиц

        database.createTables();


        // 2. Добавление клиентов

        int client1 = database.addClient(
            "Иван",
            "Иванов",
            "ivan@example.com"
        );


        int client2 = database.addClient(
            "Пётр",
            "Петров",
            "petr@example.com"
        );


        int client3 = database.addClient(
            "Анна",
            "Сидорова",
            "anna@example.com"
        );


        // 3. Добавление телефонов

        database.addPhone(
            client1,
            "+79991234567"
        );


        database.addPhone(
            client1,
            "+79997654321"
        );


        database.addPhone(
            client2,
            "+79995555555"
        );


        // У третьего клиента телефонов нет


        // Вывод всех клиентов

        std::cout
            << "\n===== ВСЕ КЛИЕНТЫ ====="
            << std::endl;


        std::vector<Client> allClients =
            database.getAllClients();


        for (const auto& client : allClients)
        {
            std::cout
                << "\nID: "
                << client.id
                << std::endl;

            std::cout
                << "Имя: "
                << client.firstName
                << std::endl;

            std::cout
                << "Фамилия: "
                << client.lastName
                << std::endl;

            std::cout
                << "Email: "
                << client.email
                << std::endl;

            std::cout << "Телефоны: ";


            if (client.phones.empty())
            {
                std::cout << "нет";
            }
            else
            {
                for (const auto& phone : client.phones)
                {
                    std::cout
                        << phone
                        << " ";
                }
            }

            std::cout << std::endl;
        }


        // 4. Изменение данных клиента

        std::cout
            << "\n===== ИЗМЕНЕНИЕ КЛИЕНТА ====="
            << std::endl;


        database.updateClient(
            client2,
            "Пётр",
            "Петров",
            "new_petr@example.com"
        );


        // 7. Поиск клиента

        std::cout
            << "\n===== ПОИСК КЛИЕНТА ====="
            << std::endl;


        std::vector<Client> foundClients =
            database.findClient("Иван");


        if (foundClients.empty())
        {
            std::cout
                << "Клиенты не найдены!"
                << std::endl;
        }
        else
        {
            for (const auto& client : foundClients)
            {
                std::cout
                    << "\nID: "
                    << client.id
                    << std::endl;

                std::cout
                    << "Имя: "
                    << client.firstName
                    << std::endl;

                std::cout
                    << "Фамилия: "
                    << client.lastName
                    << std::endl;

                std::cout
                    << "Email: "
                    << client.email
                    << std::endl;

                std::cout << "Телефоны: ";


                if (client.phones.empty())
                {
                    std::cout << "нет";
                }
                else
                {
                    for (const auto& phone : client.phones)
                    {
                        std::cout
                            << phone
                            << " ";
                    }
                }

                std::cout << std::endl;
            }
        }


        // 5. Удаление телефона

        // Для демонстрации сначала можно посмотреть
        // ID телефонов в базе данных.


        // 6. Удаление клиента

        // database.deleteClient(client3);


        std::cout
            << "\n===== ПРОГРАММА ЗАВЕРШЕНА ====="
            << std::endl;
    }


    catch (const std::exception& e)
    {
        std::cerr
            << "Ошибка: "
            << e.what()
            << std::endl;
    }


    return 0;
}
