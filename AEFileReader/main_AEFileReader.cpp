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

	bool bl = false;

	cout << int(myfr.readBoolString(bl)) << endl;
	cout << bl << endl;
	
    return 0;
}
