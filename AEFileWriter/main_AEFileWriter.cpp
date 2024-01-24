#include "include/AEFileWriter.hpp"
#include "include/AETypedefs.hpp"
#include <iostream>
#include <iomanip>	
#include <cstdio>
#include <ctime>
using namespace std;


struct Tempstruct {
	short a = 12345;
	unsigned short b = 54321;
	char c = 'A';
	float d = 12345.67890f;
};


int main() {


	std::vector<char> a{ 'a','b','c','\0','d','e','f','\0' };
	std::string b = "abc\0def";
	const char* c = "abc\0def";
	const Tempstruct d;
	std::srand(std::time(nullptr));

	AEFileWriter myfw;
	myfw.openFile("./folder/new/test/hello.txt", AEFW_FLAG_TRUNCATE, 1);


	cout << "full name: " << myfw.getFullFileName() << endl;
	cout << "relative path: " << myfw.getRelativePath() << endl;
	cout << "name: " << myfw.getFileName() << endl;
	cout << "full path: " << myfw.getFullPath() << endl;
	myfw.flushFile();
	myfw.closeFile();

	cin.get();
	
	return 0;
}