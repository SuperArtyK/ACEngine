#include "include/AEFileWriter.hpp"
#include <iostream>
#include <iomanip>	
using namespace std;



template<typename T1, typename T2>
bool is_almost_same_v = std::is_same<const T1, const T2>::value;

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


	AEFileWriter myfw;
	myfw.open("hello.txt", AEFW_FLAG_TRUNCATE, 1);

	myfw.write("The name of the module is: ");
	myfw.write(myfw.getModuleName());
	myfw.write("\nAnd the alive amount of such modules is: ");
	myfw.write(myfw.getModuleAmount());
	myfw.write('\n');

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
	myfw.write((unsigned char)0xAE);
	myfw.write('\n');
	myfw.write(a);
	myfw.write('\n');
	myfw.write(b);
	myfw.write('\n');
	myfw.write(c);
	myfw.write('\n');
	myfw.write(sizeof(Tempstruct));
	myfw.write('\n');
	myfw.write(d);
	myfw.write('\n');
	myfw.flushFile();
	myfw.closeFile();
	return 0;
}