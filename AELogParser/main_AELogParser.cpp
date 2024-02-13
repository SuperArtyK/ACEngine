

#include <iostream>
#include "AELogParser.hpp"
using std::cout;
using std::cin;

int main() {


	cout << sizeof(AELogEntry) << ": aele size in bytes\n";
	AELogParser mylp("log.txt");
	AELogEntry testing{};
	

	cout << "Parsed entries: " << mylp.amountValidEntries() << NLC;
	for (int i = -1; i < 8; i++) {
		cout << "Parsed entries of type \"" << AELogEntry::typeToString(i) << "\": " << mylp.amountTypeEntries(i)<<NLC;
	}
	
	char estr[AELE_FORMAT_MAX_SIZE]{};
	while (mylp.nextEntry(testing, AELP_SEVERITY_ALL, "AELogger") == AEFR_ERR_NOERROR) {
		std::memset(estr, AENULL, AELE_FORMAT_MAX_SIZE);
		AELogEntry::formatEntry(estr, testing);
		cout << estr;
	}

	// 39 + 1('[') + mname size + 3("]: ") + 1(message)
	return 0;
}
