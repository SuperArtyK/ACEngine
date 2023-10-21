

#include <iostream>
#include "AELogParser.hpp"
using std::cout;
using std::cin;

/// <summary>
/// Deduces the entry's log type and returns a c-string of it.
/// </summary>
/// <param name="logtype">The value of the log type</param>
/// <returns>c-string of the type</returns>
static constexpr const char* typeToString(const cint logtype) noexcept {
	switch (logtype) {
	case AELOG_TYPE_INFO: return "INFO";
	case AELOG_TYPE_WARN: return "WARNING"; case AELOG_TYPE_SEVERE_WARN: return "SEVERE_WARNING";
	case AELOG_TYPE_OK: return "OK"; case AELOG_TYPE_SUCCESS: return "SUCCESS";
	case AELOG_TYPE_ERROR: return "ERROR"; case AELOG_TYPE_FATAL_ERROR: return "FATAL_ERROR";
	case AELOG_TYPE_DEBUG: return "DEBUG"; default: return "WRONG_TYPE!";
	}
}


inline std::string formatLogEntry(const AELogEntry& entry) {
	//string that stores the date and time formatted string
	char timestr[20]{};
	//the final message to output
	char str[AELOG_ENTRY_MAX_SIZE]{};
	//formatting and writing
	ace::utils::formatDate(entry.m_tmLogTime, timestr);
	if (entry.m_cLogType == AELOG_TYPE_DEBUG) {
		snprintf(str, sizeof(str), "[%s] [%-14s] [%s]: DEBUG->%s", timestr, typeToString(entry.m_cLogType), entry.m_sModuleName, entry.m_sLogMessage);
	}
	else {
		snprintf(str, sizeof(str), "[%s] [%-14s] [%s]: %s", timestr, typeToString(entry.m_cLogType), entry.m_sModuleName, entry.m_sLogMessage);
	}

	//cout << str << "#\n";
	//cout << std::strcmp(str, R"([2023-10-18.13:43:51] [OK            ] [AELogger]: Created the AELogger instance and opened the log session in the file: "C:/Users/artem/CODING/ACEngine/x64/Debug/log.txt")") << NLC;

	return str;
}

int main() {

	AELogParser mylp("log.txt");
	AELogEntry testing{};
	

	while (mylp.parseEntry(testing) == AELP_ERR_NOERROR) {
		cout << formatLogEntry(testing) << NLC;
		AELogEntry::clearEntry(testing);
	}
	

	cout << "Parsed entries: " << mylp.amountValidEntries() << NLC;
	for (int i = 0; i < 8; i++) {
		cout << "Parsed entries of type \"" << typeToString(i) << "\": " << mylp.amountEntries(i)<<NLC;
	}
	
	// 39 + 1('[') + mname size + 3("]: ") + 1(message)
	return 0;
}
