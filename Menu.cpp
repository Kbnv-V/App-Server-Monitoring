#include "Menu.h"
#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>
using namespace std;

Menu::Menu()
{
	//создание сокета
	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == -1)
	{
		cout << "Ошибка при создании сокета";
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(666);
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr); //преобразовываем адрес IP4 в универсальный под linux

	//подключение к серверу
	if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		cout << "Приложение не доступно!" << endl;
		close(sock);
		sock = -1;
	}
	else
	{
		cout << "Подключение к приложению установлено" << endl;
	}
}

Menu::~Menu()
{
	//закрытие сокета
	if (sock >= 0)
	{
		close(sock);
		cout << "Сокет закрыт." << endl;
	}
}

void Menu::StartModule()
{

	//system("Clear"); //очистка  текста
	
	while (true)
	{
		cout << "=====МЕНЮ=====" << "\n\n";
		cout << "1. Мониторинг свободного места на диске." << "\n";
		cout << "2. Мониторинг логов системы." << "\n";
		cout << "3. Статус служб." << "\n\n";
		cout << "Ввод: ";

		cin >> Menu::itemMenu;

		if (itemMenu < 1 || itemMenu > 3)
		{
			system("clear");
			cout << "Такой пункт меню отсутсвует!" << "\n\n";
		}

		// --- МЕНЮ МОНИТОРИНГА СВОБОДНОГО МЕСТА ---
		if (itemMenu == 1)
		{
			system("clear");
			cout << "Служба мониторинга свободного места:\n\n";
			cout << "1. Включить службу." << "\n";
			cout << "2. Выключить службу." << "\n";
			cout << "3. Вернутся в главное меню." << "\n\n";
			cout << "Ввод: ";

			cin >> itemMenu;

			if (itemMenu < 1 || itemMenu > 3)
			{
				system("clear");
				cout << "Такой пункт меню отсутсвует!\n\n";
			}

			//запуск мониторинга свободного места
			if (itemMenu == 1)
			{
				system("clear");
				SendCommand("disk_monitor_start");
				cout << response << "\n\n";
			}

			//остановка службы мониторинга свободного места
			if (itemMenu == 2)
			{
				system("clear");
				SendCommand("disk_monitor_stop");
				cout << response << "\n\n";
			}

			//выход в главное меню
			if (itemMenu == 3)
			{
			}

			itemMenu = 0;

		}

		// --- МЕНЮ МОНИТОРИНГА МОНИТОРИНГА ЛОГОВ ---
		if (itemMenu == 2)
		{
			system("clear");
			cout << "Служба мониторинга логов:\n\n";
			cout << "1. Включить службу." << "\n";
			cout << "2. Выключить службу." << "\n";
			cout << "3. Вернутся в главное меню." << "\n\n";
			cout << "Ввод: ";

			cin >> itemMenu;

			if (itemMenu < 1 || itemMenu > 3)
			{
				system("clear");
				cout << "Такой пункт меню отсутсвует!\n\n";
			}

			//запуск мониторинга логов
			if (itemMenu == 1)
			{
				system("clear");
				SendCommand("log_monitor_start");
				cout << response << "\n\n";
			}

			//остановка службы мониторинга свободного места
			if (itemMenu == 2)
			{
				system("clear");
				SendCommand("log_monitor_stop");
				cout << response << "\n\n";
			}

			//выход в главное меню
			if (itemMenu == 3)
			{
			}

			itemMenu = 0;
		}

		//получение статусов работы модулей
		if (itemMenu == 3)
		{
			system("clear");
			SendCommand("service_status");
			cout << response << "\n\n";
		}
	}
}

void Menu::SendCommand(string command)
{
	response.clear(); //читсим переменную

	//проверка сокета
	if (sock < 0)
	{
		cout << "Нет ответа от приложения!" << endl;
	}

	send(sock, command.c_str(), command.size(), 0); //отправка сообщения через созданный сокет

	memset(revBuffer, 0, sizeof(revBuffer)); //memset заполняет массив нулями, чтобы после ответа не было никакого мусора

	int bytes = recv(sock, revBuffer, sizeof(revBuffer) - 1, 0);//получаем ответ на созданный сокет
	//sizeof(revBuffer) - 1 нужен для того, чтобы в конце у нас оставался терминирующий ноль, который означает конец строки

	if (bytes > 0)
	{
		response = string(revBuffer, bytes); //записываем ответ в переменную. Берем массив char и количество байтов, и все забираем
	}
	else
	{
		response = "Приложение не отвечает!";
	}
}
