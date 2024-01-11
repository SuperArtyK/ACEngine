
/** @file AELogParser/include/AELogParser.hpp
 *  This file contains the definitions of the engine's [stuff]
 *
 *  Should not cause everything to break :)
 */

#include "../include/AELogParser.hpp"


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
	// Yes, this will underflow But in nextEntry it will pre-increment it before checking
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
