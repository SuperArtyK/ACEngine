#include <iostream>
#include "AELogEntry.hpp"
using std::cout;
using std::cin;


int main() {

	AELogEntry myentry{};


	cint errorlist = AELogEntry::parseStringEntry(myentry, "[2024-02-17.15:20:20] [INFO          ] [AELogger]: Attempting to start the log-writing thread");
	if (errorlist != AELE_ERR_NOERROR) {
		cout << "Errors encountered: " << errorlist << NLC;
	}

	cout << myentry.toString();

	return 0;
}
