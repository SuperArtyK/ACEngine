#include <thread>
#include <iostream>
#include <atomic>
#include <mutex>
#include "AELog.hpp"
using std::cout;




int main()
{

	time_t now = time(0);
	struct tm tstruct;
	char buff[80];
	tstruct = *localtime(&now);
	strftime(buff, sizeof(buff), "[ %Y-%m-%d.%X ]", &tstruct);
	cout << sizeof(biguint)<<' '<<sizeof(std::size_t);
	// cout<<"Press enter to continue . . .";
	// std::cin.get();
	return 0;
}
