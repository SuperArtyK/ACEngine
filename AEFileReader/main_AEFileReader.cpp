#include <iostream>
#include "AEFileReader.hpp"
using namespace std;

struct ab {
	char a[16]{};
};


std::string outputVec(std::vector<char>& a) {

	std::string ret;
	for (std::size_t i = 0; i < a.size(); i++) {
		ret.push_back(a[i]);
	}
	return ret;
}


int main(){

    AEFileReader myfr("test.txt");

	std::string a;
	std::vector<char>b;

	cout << int(myfr.readStringNL(a, 16000)) << endl;
	cout << a << endl;
	cout << int(myfr.readStringNL(a, 16000)) << endl;
	cout << a << endl;

    return 0;
}
