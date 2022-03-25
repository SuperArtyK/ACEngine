#include <thread>
#include <iostream>
#include <atomic>
#include <mutex>
#include "AELog.hpp"
using std::cout;




int main()
{

	AELog mylg("templog");
	mylg.writeToLog("Hello AELog!", AELOG_TYPE_INFO);
	mylg.writeToLog(("First module of the ArtyK's Console Engine is working!"), AELOG_TYPE_SUCCESS);
	mylg.writeToLog("Coding at night is great!", AELOG_TYPE_WARN, "ArtyK's Brain");


	//cout<<"Press enter to continue . . .";
	//std::cin.get();
	return 0;
}
