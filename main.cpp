#include <thread>
#include <iostream>
#include <atomic>
#include <mutex>
//#include "AELog.hpp"
//#include "AEFileReader.hpp"
#include "AEFileWriter.hpp"
#include "AESleep.hpp"
using std::cout;


int main(int argc, char** argv)
{
	AEFileWriter myfw("temp", AEFW_FLAG_TRUNCATE);
	myfw.write(12345.6789111);
	std::cin.get();
	return 0;
}
