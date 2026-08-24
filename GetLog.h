#pragma once
#include <thread>

class GetLog
{
public:
	GetLog();
	~GetLog();

	bool SetStatusModule(bool value);
	bool GetStatusModule();
	void StartLogMonitoring();
	void StopLogMonitoring();

private:
	bool statusModule = false;
	void GetAllLogs();
	std::thread logThread;
};

