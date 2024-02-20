
#include "AELogger.hpp"
#include "include/AEFrame.hpp"
#include "include/AEUtils.hpp"
#include <iostream>
using std::cout;


AELogger mylog("log.txt", true);

constexpr int fpsval = 1000;

std::atomic<bool> stoptrd = 0;

void logfiller(const std::string name) {


	AEFrame a(fpsval);
	std::cout << "Entered logfiller with a thread" << NLC;
	for (int i = 0; !stoptrd; i++) {
		mylog.writeToLog("Hello " + name + "! (log entry " + std::to_string(i + 1) + ")", AELOG_TYPE_INFO, name);
		mylog.writeToLogDebug("Working in the " + name, name);
		mylog.writeToLog("Something isn't right in " + name, AELOG_TYPE_WARN, name);
		mylog.writeToLog("Something is very not good in " + name, AELOG_TYPE_SEVERE_WARN, name);
		mylog.writeToLog("Started writing to log in" + name, AELOG_TYPE_OK, name);
		mylog.writeToLog("Success, we wrote to log in" + name + "!", AELOG_TYPE_SUCCESS, name);
		mylog.writeToLog("Something is wrong in " + name, AELOG_TYPE_ERROR, name);
		mylog.writeToLog("Something is very very wrong in " + name, AELOG_TYPE_FATAL_ERROR, name);
		a.sleep();
	}


}

int main()
{
	


	std::thread t1(&logfiller, "log1");
	std::thread t2(&logfiller, "log2");
	std::thread t3(&logfiller, "AEFrame");
	std::thread t4(&logfiller, "AEKeyboard");
	std::thread t5(&logfiller, "main");
	std::thread t6(&logfiller, "AEGraphics");


	ace::utils::sleepMS(10000);

	stoptrd = true;

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	t6.join();
	return 0;
}


