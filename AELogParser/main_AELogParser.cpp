

#include <iostream>
#include "AELogParser.hpp"
using std::cout;
using std::cin;

int main() {


	cout << sizeof(AELogEntry) << ": aele size in bytes\n";
	AELogParser mylp("log.txt");
	AELogEntry testing{};
	

// 	while (mylp.parseEntry(testing) == AELP_ERR_NOERROR) {
// 		cout << formatLogEntry(testing);
// 		AELogEntry::clearEntry(&testing);
// 	}
	
	AELogEntry* entryptr = nullptr;

	mylp.logToQueue(entryptr);
	//mylp.parseLogFile();

	cout << "Parsed entries: " << mylp.amountValidEntries() << NLC;
	for (int i = -1; i < 8; i++) {
		cout << "Parsed entries of type \"" << AELogEntry::typeToString(i) << "\": " << mylp.amountTypeEntries(i)<<NLC;
	}
	
	char estr[AELE_FORMAT_MAX_SIZE]{};
	while (entryptr) {
		std::memset(estr, NULL, AELE_FORMAT_MAX_SIZE);
		AELogEntry::formatEntry(estr, *entryptr);
		cout << estr;
		entryptr = entryptr->m_pNextNode;
	}


	delete[] entryptr;

	// 39 + 1('[') + mname size + 3("]: ") + 1(message)
	return 0;
}
