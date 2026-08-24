#include "GetLog.h"
#include <iostream>
#include <fstream>
#include <sys/inotify.h>
#include <vector>
#include <map>
#include <limits.h> //для NAME_MAX
#include <unistd.h> //для read
using namespace std;

GetLog::GetLog()
{
	setlocale(LC_ALL, "ru-RU.UTF-8");
}

GetLog::~GetLog()
{
	StopLogMonitoring();
}

bool GetLog::SetStatusModule(bool value)
{
	statusModule = value;

	return true;
}

bool GetLog::GetStatusModule()
{
	return statusModule;
}

void GetLog::StartLogMonitoring()
{
	statusModule = true;

	GetAllLogs();
	//logThread = thread([this] {GetAllLogs(); });
	//logThread.detach();
}

void GetLog::StopLogMonitoring()
{
	if (logThread.joinable())
	{
		logThread.join();
	}

	statusModule = false;
}

void GetLog::GetAllLogs()
{
	string smtpLogPatch = "/home/bitrix/www/bitrix/mailer.txt"; //путь до файла с логами smtp
	string ldapLogPatch = "/home/bitrix/www/bitrix/ldap_logger.txt"; //путь до файла с логами ldap

	vector<string> filesPatch = { smtpLogPatch, ldapLogPatch };

	map<int, string> mapWatch; //тут храним пары id дескриптора => путь до файла
	map <string, ifstream> filesStreams; //тут храним потоки по открытым файлам

	int fileDeskriptor = inotify_init(); //получаем файловый дескриптор

	char buffer[sizeof(struct inotify_event) + NAME_MAX + 1];
	//размер буффера зависит от размера inotify_event + место для имена файла (NAME_MAX + 1)
	//далее по коду в buffer будет писать read(). Туда он будет писать структуру событий, которые произошли

	//проверяем на ошибки
	if (fileDeskriptor == -1)
	{
		return;
	}

	for (auto filePatch : filesPatch)
	{
		int wd = inotify_add_watch(fileDeskriptor, filePatch.c_str(), IN_MODIFY); //в параметры передаем файловый дескриптор, путь до файла и событие

		if (wd == -1)
		{
			ofstream logger;

			logger.open("logMonitor_logger.txt");

			if (logger.is_open())
			{
				logger << "Не удалось добавить в мониторинг файл " << filePatch << "\n";
			}

			continue;
		}

		mapWatch[wd] = filePatch;

		filesStreams[filePatch].open(filePatch, ios::ate); //тут мне создаем ключ и сразу можем вызвать у него метод open
		//так как мы обращаемся к ключу в мап, а у него значение ifstream, то можем вызвать метод и сразу присвоить значение
	}
	
	while (statusModule)
	{
		//read() блокирует поток до наступления события в любом файле
		read(fileDeskriptor, buffer, sizeof(buffer));

		//создаем указатель event и присваиваем ему адрес на buffer с новым типом данных
		struct inotify_event* event = (struct inotify_event*)buffer;

		string filePatch = mapWatch[event->wd]; //определяем измененный файл

		string line;
		auto& fileStream = filesStreams[filePatch];
		fileStream.clear(); //сбрасываем EOF (End of file). Это состояние файлового потока. Говорит, что файл прочитан до конца

		string fullLog;
		string message;

		while (getline(fileStream, line))
		{
			fullLog += line + "\n"; //формируем целый лог
		}

		if (fullLog.find("error") != string::npos || fullLog.find("failed") != string::npos)
		{
			message = "В логе " + filePatch + " обнаружена запись с ошибкой!";
			string command = "php /home/bitrix/www/local/server_monitoring/send_mess.php \"" + message + "\"";
			int send_comm = system(command.c_str());

			fullLog.clear();
		}
	}

	close(fileDeskriptor);
	return;
}


