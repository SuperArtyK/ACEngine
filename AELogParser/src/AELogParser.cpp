
/*! @file AELogParser/include/AELogParser.hpp
 *  This file contains the definitions of the engine's [stuff]
 *
 *  Should not cause everything to break :)
 */

#include "../include/AELogParser.hpp"

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

	while (1) {
		cursor = this->m_frLogReader.getCursorPos();
		fileRead = this->m_frLogReader.readStringNL(fileline, sizeof(fileline) - 1);

		if (fileline[0] == '\n') { //first character is an empty line -- skipping...
			break;
		}
		parsedEntry = AELogEntry::parseStringEntry(dummyentry, fileline, AELE_PARSE_STRING_WRITE_TYPE);

		switch (parsedEntry) {
			case AELP_ERR_NOERROR:
				this->m_vecEntryIndices.emplace_back(cursor, dummyentry.m_cLogType);
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


cint AELogParser::nextEntry(AELogEntry& entry, const cint severity) {
	if (this->isClosed()) {
		return AEFR_ERR_FILE_NOT_OPEN;
	}

	char str[AELE_FORMAT_MAX_SIZE + 2]{}; // 1 character more than the log entry - to determine the validity with size

	//cycle the current entry number untill we find an entry with proper severity level
	while (1) {

		if (++this->m_ullCurrentEntry >= this->m_vecEntryIndices.size()) {
			return AEFR_ERR_READ_EOF;
		}
		if (this->m_vecEntryIndices[this->m_ullCurrentEntry].second >= severity) {
			break;
		}
	}

	this->m_frLogReader.setCursorPos(this->m_vecEntryIndices[this->m_ullCurrentEntry].first, SEEK_SET);

	const cint readret = this->m_frLogReader.readStringNL(str, sizeof(str) - 1);
	if (readret == AEFR_ERR_NOERROR || readret == AEFR_ERR_READ_EOF) { // check even if it's EOF. It's not an error, but a warning from file reader
		return AELogEntry::parseStringEntry(entry, str, AELE_PARSE_STRING_FULL);
	}
	
	return readret;
}


cint AELogParser::logToQueue(AELogEntry*& begin, const cint severity) {
	if (this->isClosed()) {
		return AEFR_ERR_FILE_NOT_OPEN;
	}
	
	AELogEntry* ptr = begin = AELogEntry::makeQueue(this->amountValidEntries(severity), nullptr, false);
	cint retval = 0;
	// Yes, this is..weird, But in nextEntry it will pre-increment it before checking
	// whether it's EOF or not. So...set it to 1 less from the start, instead of subtracting 1 every time
	// Just an optimisation, uint over/underflow is well-defined anyway
	this->m_ullCurrentEntry = -1; 

	
	int test = 0;

	while (ptr && (retval = this->nextEntry(*ptr, severity)) == AELP_ERR_NOERROR) {
		ptr = ptr->m_pNextNode;
		test++;
	}



	return retval;
}
