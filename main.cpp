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
	AEFileReader myrw("temp");
	std::vector<char> str;
	myrw.readString(str,10);
	for(int i = 0; i<10; i++){
		cout<<(int)str[i]<<' ';
	}
	std::cin.get();
	return 0;
}
