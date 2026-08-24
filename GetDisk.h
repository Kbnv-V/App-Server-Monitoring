#include<iostream>
#include<thread>
#include <mutex>
#include<condition_variable>

class GetDisk
{
public:
	GetDisk();
	~GetDisk();

	bool SetStatusModule(bool value);
	bool GetStatusModule();
	void StartDiskMonitoring();
	void StopDiskMonitoring();

private:
	bool statusModule = false;
	void GetFreeSpace();
	void Pause();
	std::thread diskThread;
	std::mutex mtx;
	std::condition_variable cv; //для умной блокировки потока
};

