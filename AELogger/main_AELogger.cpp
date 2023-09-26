
#include <iostream>
#include "AELogger.hpp"
#include "include/AEUtils.hpp"
using std::cout;



ullint ordernum = 0;

inline void fillEntry(AELogEntry& entry, const std::string& messg, const ucint logtype, const std::string& modulename) {

	entry.m_ullOrderNum = ordernum++;
	entry.m_bStatus = AELOG_ENTRY_STATUS_SETTING;
	entry.m_tmLogTime = std::time(NULL);
	memcpy(entry.m_sLogMessage, messg.c_str(), (messg.size() < 512) ? messg.size() : 511);
	memcpy(entry.m_sModuleName, modulename.c_str(), (modulename.size() < 32) ? modulename.size() : 31);
	entry.m_ucLogType = logtype;
	entry.m_bStatus = AELOG_ENTRY_STATUS_READY;


}

inline const char* formatEntry(AELogEntry& entry, char temp[]) {

// 	entry.m_bStatus = AELOG_ENTRY_STATUS_READING;
// 	std::string temp;
// 	temp = '[' + ace::utils::formatDate(entry.m_tmLogTime) + "] [" + AELogEntry::typeToString(entry.m_ucLogType) + "] [" + entry.m_sModuleName + "]: " + entry.m_sLogMessage+'\n';
// 	entry.m_bStatus = AELOG_ENTRY_STATUS_INVALID;
// 	entry.m_ullOrderNum = AELOG_ENTRY_INVALID_ORDERNUM;
	snprintf(temp, 588, "[%s] [%s] [%s]: %s\n", ace::utils::formatDate(entry.m_tmLogTime).c_str(), AELogEntry::typeToString(entry.m_ucLogType), entry.m_sModuleName, entry.m_sLogMessage);
	//snprintf(temp, arrsize(temp), "12345!\n");

	return temp;
}

int main()
{
	AELogEntry* queue = AELogEntry::makeQueue(1024);
	cout <<"Size of 1 entry(bytes): " << sizeof(AELogEntry)<<"; Size of 1024 entries: "<< sizeof(AELogEntry)*1024<<NLC;

	char temp[588]{};

	fillEntry(queue[0], "Hello AELOG!", AELOG_TYPE_INFO, "Engine");
	cout << formatEntry(queue[0], temp);
	cout << sizeof(std::vector<AELogEntry*>) << "; " << sizeof(std::thread)<<NLC;


	AELogger mylog("log.txt", true);

	for (int i = 0; i < 512; i++) {
		mylog.writeToLog("Hello ACENGINE! (log entry "+std::to_string(i+1)+")");
	}
	mylog.parseEntries();

	std::cin.get();
	delete[] queue;
	return 0;
}
