#include "include/AEFileWriter.hpp"
#include <iostream>
#include <iomanip>
using namespace std;



template<typename T1, typename T2>
bool is_almost_same_v = std::is_same<const T1, const T2>::value;


int main() {

	AEFileWriter myfw;
	myfw.open("hello.txt");

	myfw.write(10); // 10
	myfw.write('\n');
	myfw.write(LLONG_MIN); // -2^63
	myfw.write('\n');
	myfw.write(LLONG_MAX); // 2^63-1
	myfw.write('\n');
	myfw.write(0); // 0
	myfw.write('\n');
	myfw.write(ULLONG_MAX); // 2^64-1
	myfw.write('\n');
	myfw.write(12.0f); //12.000000
	myfw.write('\n');
	myfw.write(FLT_MAX);
	myfw.write('\n');
	myfw.write(-FLT_MAX);
	myfw.write('\n');
	myfw.write(DBL_MAX);
	myfw.write('\n');
	myfw.write(-DBL_MAX);
	myfw.write('\n');
	myfw.write(LDBL_MAX);
	myfw.write('\n');
	myfw.write(-LDBL_MAX);
	myfw.write('\n');
	myfw.write(false);
	myfw.write('\n');
	myfw.write(true);
	myfw.write('\n');
	myfw.flushFile();
	myfw.closeFile();
	return 0;
}