
#include <iostream>
#include "AELogger.hpp"
#include "include/AEUtils.hpp"
#include "include/AEFrame.hpp"
using std::cout;


AELogger mylog("log.txt", true);

constexpr int fpsval = 8000;


void logfiller(const std::string name) {

	return;

	AEFrame a(fpsval);
	std::cout << "Entered logfiller with a thread" << NLC;
	for (int i = 0;; i++) {
		mylog.writeToLog("Hello " + std::string(__FUNCTION__) + "! (log entry " + std::to_string(i + 1) + ")", AELOG_TYPE_INFO, __FUNCTION__);
		mylog.writeToLogDebug("Working in the " + std::string(__FUNCTION__), AELOG_TYPE_DEBUG, __FUNCTION__);
		mylog.writeToLog("Something isn't right in " + std::string(__FUNCTION__), AELOG_TYPE_WARN, __FUNCTION__);
		mylog.writeToLog("Something is very not good in " + std::string(__FUNCTION__), AELOG_TYPE_SEVERE_WARN, __FUNCTION__);
		mylog.writeToLog("Started writing to log in" + std::string(__FUNCTION__), AELOG_TYPE_OK, __FUNCTION__);
		mylog.writeToLog("Success, we wrote to log in" + std::string(__FUNCTION__) + "!", AELOG_TYPE_SUCCESS, __FUNCTION__);
		mylog.writeToLog("Something is wrong in " + std::string(__FUNCTION__), AELOG_TYPE_ERROR, __FUNCTION__);
		mylog.writeToLog("Something is very very wrong in " + std::string(__FUNCTION__), AELOG_TYPE_FATAL_ERROR, __FUNCTION__);
		a.sleep();
	}


}

int main()
{
	
	std::cout << sizeof(AELogEntry) << "; " << sizeof(AELogEntry) * 65535 << NLC;

	globalLogger.writeStatus();

	std::thread t1(&logfiller, "logfil1");



	std::thread t2(&logfiller, "logfil2");
	AEFrame a(fpsval);
	for (int i = 0; ; i++) {
		mylog.writeToLog("Hello " + std::string(__FUNCTION__) + "! (log entry " + std::to_string(i + 1) + ")", AELOG_TYPE_INFO, __FUNCTION__);
		mylog.writeToLogDebug("Working in the " + std::string(__FUNCTION__), AELOG_TYPE_DEBUG, __FUNCTION__);
		mylog.writeToLog("Something isn't right in " + std::string(__FUNCTION__), AELOG_TYPE_WARN, __FUNCTION__);
		mylog.writeToLog("Something is very not good in " + std::string(__FUNCTION__), AELOG_TYPE_SEVERE_WARN, __FUNCTION__);
		mylog.writeToLog("Started writing to log in" + std::string(__FUNCTION__), AELOG_TYPE_OK, __FUNCTION__);
		mylog.writeToLog("Success, we wrote to log in" + std::string(__FUNCTION__)+"!", AELOG_TYPE_SUCCESS, __FUNCTION__);
		mylog.writeToLog("Something is wrong in " + std::string(__FUNCTION__), AELOG_TYPE_ERROR, __FUNCTION__);
		mylog.writeToLog("Something is very very wrong in " + std::string(__FUNCTION__), AELOG_TYPE_FATAL_ERROR, __FUNCTION__);

		a.sleep();
	}


	//mylog.parseEntries();
	t1.join();
	t2.join();
	std::cin.get();
	return 0;
}


