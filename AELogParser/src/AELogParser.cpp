
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
					this->m_mapModuleNames[dummyentry.m_sModuleName].second = ++mnameIndex;
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

	m_vecEntryIndices.shrink_to_fit();
	m_vecInvalidEntryIndices.shrink_to_fit();

	return ret;
}


cint AELogParser::nextEntry(AELogEntry& entry, const cint severity, const std::string_view mname, const bool strictSeverity) {

	const AELogEntryInfo leInfo = this->findNextEntry(severity, mname, strictSeverity);
	
	cint readret = AELogParser::errorFromAELEI(leInfo);
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
	
	AELogEntry* ptr = begin = AELogEntry::makeQueue((strictSeverity) ? this->amountEntriesType(severity) : this->amountEntriesValid(severity), false, nullptr);
	cint retval = 0;
	// Yes, this is..weird, But in nextEntry it will pre-increment it before checking
	// whether it's EOF or not. So...set it to 1 less from the start, instead of subtracting 1 every time
	// Just an optimisation, uint over/underflow is well-defined anyway
	const std::size_t entrynum = this->m_ullCurrentEntry;
	this->m_ullCurrentEntry = -1; 
	
	int test = 0;

	while (ptr && (retval = this->nextEntry(*ptr, severity, AELP_NO_MODULENAME, strictSeverity)) == AELP_ERR_NOERROR) {
		ptr = ptr->m_pNextNode;
		test++;
	}

	this->m_ullCurrentEntry = entrynum;

	return retval;
}



cint AELogParser::logToQueueName(AELogEntry*& begin, const std::string_view mname) {

	_AELP_CHECK_IF_FILE_OPEN;

	const ullint mnameAmount = this->amountEntriesName(mname);

	if (mnameAmount == ULLINT_MAX) {
		return AELP_ERR_INVALID_MODULE_NAME;
	}

	AELogEntry* ptr = begin = AELogEntry::makeQueue(mnameAmount, false, nullptr);
	cint retval = 0;
	// Yes, this is..weird, But in nextEntry it will pre-increment it before checking
	// whether it's EOF or not. So...set it to 1 less from the start, instead of subtracting 1 every time
	// Just an optimisation, uint over/underflow is well-defined anyway
	const std::size_t entrynum = this->m_ullCurrentEntry;
	this->m_ullCurrentEntry = -1;

	int test = 0;

	while (ptr && (retval = this->nextEntry(*ptr, AELP_SEVERITY_ALL, mname, false)) == AELP_ERR_NOERROR) {
		ptr = ptr->m_pNextNode;
		test++;
	}

	this->m_ullCurrentEntry = entrynum;


	return retval;
}

cint AELogParser::filterQueueType(AELogEntry*& ptr, const cint severity, const bool strictSeverity) {

	AELogEntry* cur = ptr;
	AELogEntry kludge{ .m_pNextNode = ptr }; // a kludge variable for the pastEntry to work
	AELogEntry* past = &kludge;
	std::size_t newQueueSize = 0;

	if (ptr == nullptr) {
		return AELP_ERR_INVALID_QUEUE;
	}


	//check if it even has the severity
	while (cur) {
		if (AELogParser::checkSeverity(cur->m_cLogType, severity, strictSeverity)) {
			goto foundSeverity; // we found the severity, break the loop
			break;
		}

		cur = cur->m_pNextNode;
	}

	// there is no log entry with the given severity
	return AELP_ERR_INVALID_SEVERITY;


foundSeverity: //welp, we found it

	while (cur) {
		if (AELogParser::checkSeverity(cur->m_cLogType, severity, strictSeverity)) {
			past = past->m_pNextNode;
			past->copyEntry(*cur);
			newQueueSize++;
		}
		cur = cur->m_pNextNode;

	}


	past->m_pNextNode = nullptr;

	//clean up the queue
	//or...make a new queue instead!

	cur = ptr;

	AELogEntry* const newQueue = AELogEntry::makeQueue(newQueueSize, false);
	AELogEntry* iter = newQueue;
	while (cur) {
		iter->copyEntry(*cur);
		iter = iter->m_pNextNode;
		cur = cur->m_pNextNode;
	}

	delete[] ptr;

	ptr = newQueue;

}

AELogEntryInfo AELogParser::findNextEntry(const cint severity, const std::string_view mname, const bool strictSeverity) {

	if (this->isClosed()) {
		return AELogEntryInfo::invalidEntry();
	}

	std::string mnameStr = mname.data();

	if (!mname.empty() && !this->m_mapModuleNames.contains(mnameStr)) {
		return AELogEntryInfo{ .mnameIndex = AELEI_INVLAID_MNAME };
	}

	if (!ace::utils::isInRange(AELP_SEVERITY_ALL, AELOG_TYPE_FATAL_ERROR, severity) || //is in range at all?
		this->containsSeverity(severity, strictSeverity)) { // does this exist in the log (accounting for strict severity filter)
		return AELogEntryInfo{ .logType = AELEI_INVALID_TYPE };
	}

	
	const short filterMnameIndex = (!mname.empty()) ? this->m_mapModuleNames[mnameStr].second : 0;

	mnameStr.clear();
	mnameStr.shrink_to_fit(); //not necessary anymore

	const std::size_t entryNum = m_ullCurrentEntry; //write down the cursor value

	//searching time
	while (1) {

		if (++this->m_ullCurrentEntry >= this->m_vecEntryIndices.size()) {
			// check if cursor was already out of bounds
			if ((entryNum + 1) >= this->m_vecEntryIndices.size()) {
				return AELogEntryInfo{ .cursorIndex = AELEI_INVALID_CURSOR };
			}
			//nope, we got out of bounds ourselves
			//because of invalid filter!
			
			this->m_ullCurrentEntry = entryNum; // return the cursor to the previous position
			return AELogEntryInfo { 
				//god damn it msvc. Why is it an error to do it *that* way, while initialising it with a number outside of ternary is okay
				.mnameIndex = (short) ((filterMnameIndex) ? AELEI_INVLAID_MNAME : 0), 
				.logType = (cint) ((severity != AELP_SEVERITY_ALL) ? AELEI_INVALID_TYPE : 0)
			};

		}
		if (AELogParser::checkSeverity(this->m_vecEntryIndices[this->m_ullCurrentEntry].logType, severity, strictSeverity) && //check the severity with lambdas
			AELogParser::checkMName(this->m_vecEntryIndices[this->m_ullCurrentEntry].mnameIndex, filterMnameIndex)) {

			break;
		}
	}

	return this->m_vecEntryIndices[this->m_ullCurrentEntry];

}