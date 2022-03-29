#include <thread>
#include <iostream>
#include <atomic>
#include <mutex>
#include "AELog.hpp"
#include "AESleep.hpp"
using std::cout;




int main(int argc, char** argv)
{
	cout << getTypeName<AEFileWriter>()<<'\n';
	
	cout<<"Press enter to continue . . .";

	
	std::cin.get();
	return 0;
}
