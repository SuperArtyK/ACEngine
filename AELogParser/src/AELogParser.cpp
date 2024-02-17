
/*! @file AELogParser/include/AELogParser.hpp
 *  This file contains the definitions of the engine's [stuff]
 *
 *  Should not cause everything to break :)
 */

#include "../include/AELogParser.hpp"
#include <functional>

cint AELogParser::openLog(const std::string_view fname) {
	if (this->isOpen()) {
		return AEFR_ERR_OPEN_FILE_ALREADY_OPENED;
	}
	const cint ret = this->m_frLogReader.openFile(fname);
	if (ret != AELP_ERR_NOERROR) {
		return ret;
	}

	// and index/parse log
	char fileline[AELE_FORMAT_MAX_SIZE + 1]{}; //+1 more character to determine the validity)
	cint fileRead = AEFR_ERR_NOERROR;
	cint parsedEntry = AELP_ERR_NOERROR;
	llint cursor = 0;
	AELogEntry dummyentry{};
	short mnameIndex = 0;


	m_vecEntryIndices.reserve(AELOG_DEFAULT_QUEUE_SIZE * 10);
	m_vecInvalidEntryIndices.reserve(AELOG_DEFAULT_QUEUE_SIZE);
	m_mapModuleNames.reserve(AELOG_DEFAULT_QUEUE_SIZE);

	while (1) {
		cursor = this->m_frLogReader.getCursorPos();
		fileRead = this->m_frLogReader.readStringNL(fileline, sizeof(fileline) - 1);

		if (fileline[0] == '\n') { // it's an empty line -- skipping...
			break;
		}
		parsedEntry = AELogEntry::parseStringEntry(dummyentry, fileline, AELE_PARSE_STRING_TYPE | AELE_PARSE_STRING_MNAME);


		switch (parsedEntry) {
			case AELP_ERR_NOERROR:
				if (this->m_mapModuleNames.contains(dummyentry.m_sModuleName)) {
					this->m_vecEntryIndices.emplace_back(cursor, this->m_mapModuleNames[dummyentry.m_sModuleName].second, dummyentry.m_cLogType);
				}
				else {
					this->m_mapModuleNames[dummyentry.m_sModuleName].second = mnameIndex++;
					this->m_vecEntryIndices.emplace_back(cursor, mnameIndex, dummyentry.m_cLogType); // a bit of optimisation
				}
				
				this->m_mapModuleNames[dummyentry.m_sModuleName].first++;
				this->m_arrEntryAmount[dummyentry.m_cLogType + 1]++;
				break;

			default:
				if (fileRead != AEFR_ERR_NOERROR) {
					return ret;
				}
				this->m_vecInvalidEntryIndices.push_back(cursor);
				this->m_arrEntryAmount[0]++;
				break;
		}

	}

	return ret;
}


cint AELogParser::nextEntry(AELogEntry& entry, const cint severity, const std::string_view mname, const bool strictSeverity) {

	const AELogEntryInfo leInfo = this->findNextEntry(severity, mname, strictSeverity);
	
	cint readret = this->errorFromAELEI(leInfo);
	if (readret != AELP_ERR_NOERROR) {
		return readret; // return the error code from the AELEI
	}

	char str[AELE_FORMAT_MAX_SIZE + 2]{}; // 1 character more than the log entry - to determine the validity with size

	this->m_frLogReader.setCursorPos(this->m_vecEntryIndices[this->m_ullCurrentEntry].cursorIndex, SEEK_SET);

	readret = this->m_frLogReader.readStringNL(str, sizeof(str) - 1);
	if (readret == AEFR_ERR_NOERROR || readret == AEFR_ERR_READ_EOF) { // check even if it's EOF. It's not an error, but a warning from file reader
		return AELogEntry::parseStringEntry(entry, str, AELE_PARSE_STRING_FULL);
	}
	
	return readret;
}


cint AELogParser::logToQueueType(AELogEntry*& begin, const cint severity, const bool strictSeverity) {
	_AELP_CHECK_IF_FILE_OPEN;
	
	AELogEntry* ptr = begin = AELogEntry::makeQueue((strictSeverity) ? this->amountEntriesType(severity) : this->amountEntriesValid(severity), nullptr, false);
	cint retval = 0;
	// Yes, this is..weird, But in nextEntry it will pre-increment it before checking
	// whether it's EOF or not. So...set it to 1 less from the start, instead of subtracting 1 every time
	// Just an optimisation, uint over/underflow is well-defined anyway
	this->m_ullCurrentEntry = -1; 
	
	int test = 0;

	while (ptr && (retval = this->nextEntry(*ptr, severity, AELP_NO_MODULENAME, strictSeverity)) == AELP_ERR_NOERROR) {
		ptr = ptr->m_pNextNode;
		test++;
	}

	return retval;
}


cint AELogParser::logToQueueName(AELogEntry*& begin, const std::string_view mname) {

	_AELP_CHECK_IF_FILE_OPEN;

	const ullint mnameAmount = this->amountEntriesName(mname);

	if (mnameAmount == ULLINT_MAX) {
		return AELP_ERR_INVALID_MODULE_NAME;
	}

	AELogEntry* ptr = begin = AELogEntry::makeQueue(mnameAmount, nullptr, false);
	cint retval = 0;
	// Yes, this is..weird, But in nextEntry it will pre-increment it before checking
	// whether it's EOF or not. So...set it to 1 less from the start, instead of subtracting 1 every time
	// Just an optimisation, uint over/underflow is well-defined anyway
	this->m_ullCurrentEntry = -1;

	int test = 0;

	while (ptr && (retval = this->nextEntry(*ptr, AELP_SEVERITY_ALL, mname, false)) == AELP_ERR_NOERROR) {
		ptr = ptr->m_pNextNode;
		test++;
	}



	return retval;
}



AELogEntryInfo AELogParser::findNextEntry(const cint severity, const std::string_view mname, const bool strictSeverity) {

	if (this->isClosed()) {
		return AELogEntryInfo::invalidEntry();
	}

	std::string mnameStr = mname.data();

	if (!mnameStr.empty() && !this->m_mapModuleNames.contains(mnameStr)) {
		return AELogEntryInfo{.mnameIndex = AELEI_INVLAID_MNAME};
	}

	if (!ace::utils::isInRange(AELOG_TYPE_DEBUG, AELOG_TYPE_FATAL_ERROR, severity)) {
		return AELogEntryInfo{ .logType = AELEI_INVALID_TYPE };
	}

	// ternary with lambda.
	// F to code readability
	// basically a lambda to check severity of passed severity values
	// strictSeverity requires the severities to be the *same*
	// otherwise check if the passed severity is equal or higher than the "filter"
	const auto checkSeverity = (strictSeverity) ?
		[](const cint curEntrySeverity, const cint curSeverity) noexcept { return curEntrySeverity == curSeverity; } : // the strict, *exact* severity check
		[](const cint curEntrySeverity, const cint curSeverity) noexcept { return curEntrySeverity >= curSeverity; }; // the normal severity check

	const short mnameIndex = (!mnameStr.empty()) ? this->m_mapModuleNames[mnameStr].second : -1;

	mnameStr.clear();
	mnameStr.shrink_to_fit();

	const auto checkMName = (mname.empty()) ?
		[](const short& controlIndex, const short& mIndex) noexcept { return true; } : // if the module name is empty -- return true always (we aren't checking for it)
		[](const short& controlIndex, const short& mIndex) noexcept { return (controlIndex == mIndex); };

	while (1) {

		if (++this->m_ullCurrentEntry >= this->m_vecEntryIndices.size()) {
			return AELogEntryInfo { .cursorIndex = AELEI_INVALID_CURSOR};
		}
		if (checkSeverity(this->m_vecEntryIndices[this->m_ullCurrentEntry].logType, severity) && //check the severity with lambdas
			checkMName(mnameIndex, this->m_vecEntryIndices[this->m_ullCurrentEntry].mnameIndex)) {

			break;
		}
	}

	return this->m_vecEntryIndices[this->m_ullCurrentEntry];

}