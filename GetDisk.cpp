#pragma once
#include "GetDisk.h"
#include "iostream"
#include "filesystem"
#include "chrono"
#include<thread>
#include<ctime>
#include <fstream>
#include <mutex>

using namespace std;
using namespace chrono;

GetDisk::GetDisk()
{
	setlocale(LC_ALL, "ru-RU.UTF-8");
}

GetDisk::~GetDisk()
{
	StopDiskMonitoring();
}

bool GetDisk::SetStatusModule(bool value)
{
	statusModule = value;

	return true;
}

bool GetDisk::GetStatusModule()
{
	return statusModule;
}

void GetDisk::Pause()
{
	/*
	unique_lock<mutex> uLock(mtx, defer_lock);
	uLock.lock();

	cv.wait_for(uLock, chrono::seconds(10), [this]() {return !statusModule; });
	cout << "Pause" << endl;
	this_thread::sleep_for(seconds(10)); // Пауза для тестирования
	*/
	
	auto nowTime = system_clock::now(); //получаем текущее время. Возвращает в UTC
	
	//cout << "Текущее время [nowTime] - " << nowTime << endl;

	//тут формируем время, с которым потом будет сравнивать текущее
	auto today = floor<days>(nowTime); //округляем до указанных единиц
	//прибавляем время с поправкой на UTC, чтобы попасть в нужное время по времени МСК
	auto todayStartMorning = today + hours(7);
	auto todayStartEvening = today + hours(14);
	auto tomorrowStart = todayStartMorning + hours(24);

	system_clock::time_point resTime;

	if (nowTime < todayStartMorning)
	{
		resTime = todayStartMorning;
	}
	else if (nowTime > todayStartMorning && nowTime < todayStartEvening)
	{
		resTime = todayStartEvening;
	}
	else
	{
		resTime = tomorrowStart;
	}

	//cout << "До следующего запуска осталось " << duration_cast<hours>(resTime - nowTime).count() << " ч. " << duration_cast<minutes>(resTime - nowTime).count() % 60 << " мин." << "\n";

	//this_thread::sleep_until(resTime);
	
	unique_lock<mutex> uLock(mtx, defer_lock);
	uLock.lock();

	cv.wait_until(uLock, resTime, [this]() {return !statusModule; });
}

void GetDisk::GetFreeSpace()
{
	//ofstream test("test.txt");
	//int i = 0;

	while (statusModule)
	{
		//ИЗМЕНИТЬ ПУТЬ
		auto info = filesystem::space("/");

		double freeGb = info.free / 1073741824.00;

		stringstream strFreeGb;
		strFreeGb << freeGb;
		string stringFreeGb = strFreeGb.str();

		string message = "Свободное место на диске: " + stringFreeGb;

		string command = "php /home/bitrix/www/local/server_monitoring/send_mess.php \"" + message + " Гб.\"";

		int send_comm = system(command.c_str());

		Pause();
	}
}

void GetDisk::StartDiskMonitoring()
{
	statusModule = true;

	GetFreeSpace();
}

void GetDisk::StopDiskMonitoring()
{
	statusModule = false;

	cv.notify_all(); //будим поток от паузы для его остановки
}

