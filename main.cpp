#include <thread>
#include <iostream>
#include <atomic>
#include <mutex>
#include "AELog.hpp"
using std::cout;




int main()
{

	
	AEFileWriter myfw("path/to/log/temp.txt");
	myfw.writeString("it works123!\nHello AEFileWriter!");
	myfw.closeFile();

	// cout<<"Press enter to continue . . .";
	// std::cin.get();
	return 0;
}
