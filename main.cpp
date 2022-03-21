#include <thread>
#include <iostream>
#include <atomic>
#include <mutex>
#include "AELog.hpp"
using std::cout;




int main()
{

	AELog mylg("temp",1);
	for (int i = 0; i < 20; i++){
		mylg.writeToLog("12345", AELOG_TYPE_INFO, "Engine");
	}


	// cout<<"Press enter to continue . . .";
	// std::cin.get();
	return 0;
}
