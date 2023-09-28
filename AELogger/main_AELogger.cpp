
#include <iostream>
#include "AELogger.hpp"
#include "include/AEUtils.hpp"
#include "include/AEFrame.hpp"
using std::cout;


AELogger mylog("log.txt", true);


void logfiller() {

	AEFrame a(60);
	std::cout << "Entered logfiller with a thread" << NLC;
	for (int i = 0;; i++) {
		mylog.writeToLog("Hello " + std::string(__FUNCTION__) + "! (log entry " + std::to_string(i + 1) + ")");
		a.sleep();
	}


}

int main()
{
	
	std::cout << sizeof(AELogEntry) << "; " << sizeof(AELogEntry) * 65535 << NLC;

	std::thread t1(&logfiller);
	//std::thread t2(&logfiller);
	AEFrame a(60);
	for (int i = 0;; i++) {
		mylog.writeToLog("Hello " + std::string(__FUNCTION__) + "! (log entry " + std::to_string(i + 1) + ")");
		a.sleep();
	}

	//mylog.parseEntries();
	t1.join();
	//t2.join();
	std::cin.get();
	return 0;
}
