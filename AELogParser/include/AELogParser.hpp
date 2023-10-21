
/** @file AELogParser/include/AELogParser.hpp
 *  This file contains the [stuff]
 *
 *  Should not cause everything to break :)
 */

#pragma warning(disable : 4996)

#pragma once

#ifndef ENGINE_LOG_PARSER_HPP
#define ENGINE_LOG_PARSER_HPP

#include "include/AEModuleBase.hpp"
#include "include/AETypedefs.hpp"
#include "include/AEUtils.hpp"
#include "include/AEFileReader.hpp"
#include "include/AETypedefs.hpp"
#include "include/AELogTypes.hpp"
#include "include/AEFlags.hpp"
#include "include/AETypes.hpp"
#include <vector>
#include <array>
#include <string_view>

#define AELP_ERR_NOERROR 0
#define AELP_ERR_INVALID_LENGTH 3
#define AELP_ERR_INVALID_TIME 4
#define AELP_ERR_INVALID_TYPE 5
#define AELP_ERR_INVALID_MNAME 6
#define AELP_ERR_INVALID_FORMAT 7

//index for the "entry indices" array to get the std::vector corresponding to the AELogger's invalid type
#define _AELP_INVALID_ENTRY_INDEX 8


/// @todo add function to parse 1 line
/// @todo add a way to filter the logs by severity
/// @todo add a way to error out about invalid entry
/// @todo add reading entries from the whole file and record their order
/// @todo add filtering entries from the whole file
/// @todo add a way to validate the entry without reading it (if possible) -> populate the entry index array
/// @todo add reading of the entry upon request, by giving it's order number in the log
/// @todo fix the mixups of string_view and char array/ptr (...why)
class AELogParser : public __AEModuleBase<AELogParser> {

public:
	explicit AELogParser(const std::string_view fname) : 
		m_frLogReader(fname), m_ullLogEntries(0), m_cLastError(AELP_ERR_NOERROR){

		for (int i = 0; i < m_arrEntryIndices.size(); i++) {
			m_arrEntryIndices[i].reserve(256);
		}
	}



	inline cint openLog(const std::string_view fname) {
		if (this->isOpen()) {
			return AEFR_ERR_FILE_ALREADY_OPEN;
		}
		const cint ret = this->m_frLogReader.openFile(fname);
		if (ret != AELP_ERR_NOERROR) {
			for (int i = 0; i < m_arrEntryIndices.size(); i++) {
				m_arrEntryIndices[i].clear();
				m_arrEntryIndices[i].reserve(256);
			}
		}
		return ret;

	}

	inline cint closeLog(void) {
		return this->m_frLogReader.closeFile();
	}

	//parse the log entry and give data to the given entry
	//if error was encountered 
	inline cint parseEntry(AELogEntry& entry) {
		if (this->isClosed()) {
			return AEFR_ERR_FILE_NOT_OPEN;
		}
		if (this->m_frLogReader.isEOF()) {
			return AEFR_ERR_READING_EOF;
		}

		char str[AELOG_ENTRY_MAX_SIZE + 2]{}; // 1 character more than the log entry - to determine the size

		//initial read of the line
		const llint entrypos = this->m_frLogReader.getCursorPos();
		const cint readret = this->m_frLogReader.readStringNL(str, sizeof(str) - 1);
		if (readret == AEFR_ERR_NOERROR) {
			const cint entryParsed = this->parseEntryString(entry, str);
			if (entryParsed == AELP_ERR_NOERROR) {
				this->m_arrEntryIndices[entry.m_cLogType].push_back(entrypos); // valid entry
			}
			else {
				this->m_arrEntryIndices[8].push_back(entrypos); //invalid entry
			}
		}
		return readret;
	}
	
	cint parseEntryString(AELogEntry& entry, const std::string_view entryString) {
		constexpr std::size_t POS_TYPE = 22, POS_MNAME = 39;
		if (entryString.size() == AELOG_ENTRY_MAX_SIZE + 1 || entryString.size() < 47) { // it is more then the max size or less than possible size -- invalid or...somehow modified
			return AELP_ERR_INVALID_LENGTH;
		}

		char logmessage[512]{}; //log message, exactly 511 characters
		char mname[33]{}; // +1 more character to see if it's not correct format
		char ltype[15]{}; //14 characters guaranteed

		//check the log entry's time
		const std::time_t entryTime = ace::utils::stringToDate(entryString.data(), "[%Y-%m-%d.%X");
		if (entryTime == -1) {
			return AELP_ERR_INVALID_TIME;
		}

		// check for the "] " between the time and type
		if (std::memcmp(entryString.data() + POS_TYPE - 2, "] ", 2)) {
			return AELP_ERR_INVALID_FORMAT;
		}

		// read the possible type
		if (sscanf(entryString.data() + POS_TYPE, "[%14[^]] ", ltype) != 1) {
			return AELP_ERR_INVALID_TYPE; //oops!
		}

		// check for the valid log entry's type
		const cint entryType = AELogParser::matchLogType(ltype);
		if (entryType == -1) {
			return AELP_ERR_INVALID_TYPE; //oops!
		}


		// check for the "] " between the type and module name
		if (std::memcmp(entryString.data() + POS_MNAME - 2, "] ", 2)) {
			return AELP_ERR_INVALID_FORMAT;
		}

		// read the possible module name
		if (sscanf(entryString.data() + POS_MNAME, "[%32[^]] ", mname) != 1) {
			return AELP_ERR_INVALID_MNAME;
		}

		// check for the correct module name
		const std::string_view strvMname(mname);
		if (strvMname.size() > 31 || !ace::utils::isAlNumUs(strvMname)) {
			return AELP_ERR_INVALID_MNAME;
		}

		//okay. we might have read that.
		//Since the module name is variable and cannot be predicted
		//We might have read untill the end of the "entry"
		//so check it!
		if (entryString.size() < (POS_MNAME + strvMname.size() + 5)) { //position of module name (39)+ size of the mname + 5 characters forward (leading [, trailing "]: ", and 1 character for message)
			return AELP_ERR_INVALID_LENGTH;
		}

		//cool, now onwards to text
		//check for the existing boundary between module name and message
		if (std::memcmp(entryString.data() + POS_MNAME + strvMname.size() + 1, "]: ", 3)) {
			return AELP_ERR_INVALID_FORMAT; //oops
		}

		//cool, passed. now read untill the end
		//the newline and or the size is guaranteed to exist
		sscanf(entryString.data() + POS_MNAME + strvMname.size() + 4, "%[^511\n]", logmessage);

		//time to write stuff
		AELogEntry::clearEntry(entry);

		// what if the entry is a debug one?
		if (entryType == AELOG_TYPE_DEBUG && std::strstr(logmessage, "DEBUG->") != nullptr) {
			std::memcpy(entry.m_sLogMessage, logmessage + sizeof("DEBUG->") - 1, AELOG_ENTRY_MESSAGE_SIZE - sizeof("DEBUG->") + 1);
		}
		else {
			std::memcpy(entry.m_sLogMessage, logmessage, AELOG_ENTRY_MESSAGE_SIZE);
		}

		std::memcpy(entry.m_sModuleName, mname, strvMname.size()); //module name
		entry.m_tmLogTime = entryTime; //time
		entry.m_ullOrderNum = m_ullLogEntries++; //order number
		// not doing the queue..because it's the user's concern
		entry.m_cStatus = AELOG_ENTRY_STATUS_READY; //status
		entry.m_cLogType = entryType; //type

		return AELP_ERR_NOERROR;
	}

	inline cint parseEntries(AELogEntry& queue) {

		if (this->isClosed()) {
			return AEFR_ERR_FILE_NOT_OPEN;
		}
		if (this->m_frLogReader.isEOF()) {
			return AEFR_ERR_READING_EOF;
		}


		cint checkres = 0;
		while (checkres != AEFR_ERR_READING_EOF) {

		}



	}

	inline ullint amountValidEntries(void) const noexcept {
		ullint tempres = 0;
		for (int i = 0; i < 8; i++) {
			tempres += m_arrEntryIndices[i].size();
		}
		return tempres;
	}

	inline ullint amountInvalidEntries(void) const noexcept {
		return m_arrEntryIndices[8].size();
	}

	inline ullint amountEntries(const cint logtype) const noexcept {
		if (!ace::utils::isInRange<cint>(AELOG_TYPE_INVALID, AELOG_TYPE_FATAL_ERROR, logtype)) {
			return ULLINT_MAX;
		}
		if (logtype == AELOG_TYPE_INVALID) {
			return this->m_arrEntryIndices[_AELP_INVALID_ENTRY_INDEX].size();
		}
		return this->m_arrEntryIndices[logtype].size();
	}
	
	inline const std::vector<ullint>& getTypeIndices(const cint logtype) const noexcept {
		if (!ace::utils::isInRange<cint>(AELOG_TYPE_INVALID, AELOG_TYPE_FATAL_ERROR, logtype)) {
			return {ULLINT_MAX};
		}
		if (logtype == AELOG_TYPE_INVALID) {
			return this->m_arrEntryIndices[_AELP_INVALID_ENTRY_INDEX];
		}
		return this->m_arrEntryIndices[logtype];
	}

	

	inline bool isOpen(void) const noexcept {
		return this->m_frLogReader.isOpen();
	}

	inline bool isClosed(void) const noexcept {
		return this->m_frLogReader.isClosed();
	}


private:

	static cint matchLogType(const std::string_view str) noexcept {
		if (str.size() != 14) { return AELOG_TYPE_INVALID; }

		if (str == "DEBUG         ") {
			return AELOG_TYPE_DEBUG;
		}
		else if (str == "INFO          ") {
			return AELOG_TYPE_INFO;
		}
		else if (str == "WARNING       ") {
			return AELOG_TYPE_WARN;
		}
		else if (str == "SEVERE_WARNING") {
			return AELOG_TYPE_SEVERE_WARN;
		}
		else if (str == "OK            ") {
			return AELOG_TYPE_OK;
		}
		else if (str == "SUCCESS       ") {
			return AELOG_TYPE_SUCCESS;
		}
		else if (str == "ERROR         ") {
			return AELOG_TYPE_ERROR;
		}
		else if (str == "FATAL_ERROR   ") {
			return AELOG_TYPE_FATAL_ERROR;
		}
		else {
			return AELOG_TYPE_INVALID;
		}
	}



	/// The reader of the opened log file
	AEFileReader m_frLogReader;
	/// The array of the arrays of log entry cursor indices in the file, corresponding to each log type (including "invalid entry" for invalid stuff)
	std::array<std::vector<ullint>, 9> m_arrEntryIndices;
	/// The amount of log entries read in the file;
	ullint m_ullLogEntries;
	cint m_cLastError;
};

//aaaand register it
REGISTER_CLASS(AELogParser)


#endif // !ENGINE_LOG_PARSER_HPP
