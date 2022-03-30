#include <thread>
#include <iostream>
#include <atomic>
#include <mutex>
//#include "AELog.hpp"
#include "AEFileReader.hpp"
#include "AESleep.hpp"
using std::cout;


int main(int argc, char** argv)
{
	cout << getTypeName<AESleep>()<<'\n';
	
	cout<<"Press enter to continue . . .";

	
	std::cin.get();
	return 0;
}
