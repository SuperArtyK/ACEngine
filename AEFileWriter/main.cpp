#include "AEFileWriter.hpp"
#include <iostream>
#include <iomanip>
using namespace std;


int main() {
	AEFileWriter myfw;
	myfw.open("hello.txt");

	myfw.writeInt(10); // 10
	myfw.writeChar('\n');
	myfw.writeInt(LLONG_MIN); // -2^63
	myfw.writeChar('\n');
	myfw.writeInt(LLONG_MAX); // 2^63-1
	myfw.writeChar('\n');
	myfw.writeUInt(0); // 0
	myfw.writeChar('\n');
	myfw.writeUInt(ULLONG_MAX); // 2^64-1
	myfw.writeChar('\n');
	myfw.writeFloat(12.0f); //12.000000
	myfw.writeChar('\n');
	myfw.writeFloat(FLT_MAX);
	myfw.writeChar('\n');
	myfw.writeFloat(-FLT_MAX);
	myfw.writeChar('\n');
	myfw.writeFloat(DBL_MAX);
	myfw.writeChar('\n');
	myfw.writeFloat(-DBL_MAX);
	myfw.writeChar('\n');
	myfw.writeFloat(LDBL_MAX);
	myfw.writeChar('\n');
	myfw.writeFloat(-LDBL_MAX);
	myfw.writeChar('\n');
	myfw.writeBool(false);
	myfw.writeChar('\n');
	myfw.writeBool(true);
	myfw.writeChar('\n');
	myfw.flushFile();
	myfw.closeFile();
	return 0;
}