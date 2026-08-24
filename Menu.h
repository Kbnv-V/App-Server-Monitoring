#pragma once
#include"GetDisk.h"
#include"GetLog.h"
#include<thread>
#include<string>

using namespace std;

class Menu
{
public:
	void StartModule();
	Menu();
	~Menu();

private:
	int itemMenu;
	void SendCommand(string command);
	int sock = -1;
	string message;
	char sendBuffer[1024];
	char revBuffer[1024];
	string response;
};

