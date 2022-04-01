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
	AEFileWriter myfw("temp");
	myfw.writeString("This is an apple");
	myfw.setCursorPos(9, SEEK_SET);
	myfw.writeString(" sam");
	std::cin.get();
	return 0;
}
