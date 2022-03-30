#include <thread>
#include <iostream>
#include <atomic>
#include <mutex>
//#include "AELog.hpp"
#include "AEFileReader.hpp"
#include "AEFileWriter.hpp"
#include "AESleep.hpp"
using std::cout;


int main(int argc, char** argv)
{
	AEFileWriter myfw("myfile.bin", AEFW_FLAG_TRUNCATE, BUINT_MAX);
	myfw.writeString(std::string("Hello AEFileWriter and AEFileReader!"));
	myfw.closeFile();

	cout << getTypeName<AESleep>() << '\n';

	cout<<"Press enter to continue . . .";

	
	std::cin.get();
	return 0;
}
