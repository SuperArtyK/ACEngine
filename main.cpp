#include <thread>
#include <iostream>
#include <atomic>
#include <mutex>
#include "AELog.hpp"
using std::cout;




int main()
{
	cout<<sizeof(AELogEntry);
	AELog::writerThread();
	// cout<<"Press enter to continue . . .";
	// std::cin.get();
	return 0;
}
