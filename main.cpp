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
	AEFileReader myfw("temp");
	int temp;
	myfw.read(temp);
	cout<<temp;
	std::cin.get();
	return 0;
}
