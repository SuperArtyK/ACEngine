#include <thread>
#include <iostream>
#include <atomic>
#include <mutex>
#include "AELog.hpp"
using std::cout;




int main()
{

	AELog mylg("templog");
	mylg.writeToLog("hello logger!", AELOG_TYPE_INFO);
	// cout<<"Press enter to continue . . .";
	// std::cin.get();
	return 0;
}
