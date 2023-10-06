#include <iostream>
#include "AEFileReader.hpp"
using namespace std;

struct ab {
	char a[16]{};
};


int main(){

    AEFileReader myfr("test.txt");

	float b = 0;

	cout << int(myfr.readFloatString(b)) << endl;
	cout << b << endl;

	cout << int(myfr.readFloatString(b)) << endl;
	cout << b << endl;







    return 0;
}
