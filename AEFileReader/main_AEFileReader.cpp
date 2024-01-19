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


	cout << AEFileReader::getModuleName() << endl;

    AEFileReader myfr("test.txt");

	std::string a,b;

	bool bl = false;

	cout << int(myfr.readStringNL(a, 200)) << endl;
	cout << a;
	myfr.readStringNL(b, 200);
	cout << b << endl;


    return 0;
}
