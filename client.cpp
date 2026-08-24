#include <iostream>
#include <chrono>
#include <thread>
#include<fstream>
#include "Menu.h"

using namespace std;

Menu* globalMenu = nullptr;

int main()
{
    setlocale(LC_ALL, "RU");

    Menu newMenu;

    system("clear");

    newMenu.StartModule();

    return 0;
}
