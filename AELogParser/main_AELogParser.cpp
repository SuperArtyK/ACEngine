

#include <iostream>
#include "AELogParser.hpp"
using std::cout;
using std::cin;

int main() {


	cout << sizeof(AELogEntry) << ": aele size in bytes\n";
	AELogParser mylp("log.txt");
	AELogEntry testing{};
	

	cout << "Parsed entries: " << mylp.amountEntriesValid() << NLC;
	for (int i = -1; i < 8; i++) {
		cout << "Parsed entries of type \"" << AELogEntry::typeToString(i) << "\": " << mylp.amountEntriesType(i)<<NLC;
	}
	
	AELogEntry* myqueue;
	mylp.logToQueueType(myqueue, AELP_SEVERITY_ALL);

	mylp.filterQueueType(myqueue, AELOG_TYPE_ERROR);

	AELogEntry* iter = myqueue;

	int amt = 0;

	

	while (iter)
	{
		//cout << iter->toString();
		amt++;
		iter = iter->m_pNextNode;
	}

	cout << amt << " is the size of the queue";

	delete[] myqueue;

	// 39 + 1('[') + mname size + 3("]: ") + 1(message)
	return 0;
}
