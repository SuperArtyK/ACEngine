
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
#define AELP_ERR_INVALID_LENGTH 1 
#define AELP_ERR_INVALID_TIME 2
#define AELP_ERR_INVALID_TYPE 3
#define AELP_ERR_INVALID_MNAME 4
#define AELP_ERR_INVALID_FORMAT 5




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
	AELogParser(const std::string_view fname) : 
		m_frLogReader(fname), m_ullLogEntries(0), m_cLastError(AELP_ERR_NOERROR){

		for (int i = 0; i < m_arrEntryIndices.size(); i++) {
			m_arrEntryIndices[i].reserve(256);
		}


	}

	//parse the log entry and give data to the given entry
	//and forcefully parse if asked
	
	cint parseEntry(AELogEntry& entry, const bool forceParse = false) {

		constexpr std::size_t POS_TIME = 0, POS_TYPE = 22, POS_MNAME = 39;
		char str[AELOG_ENTRY_MAX_SIZE + 2]{}; // 1 character more than the log entry - to determine the size
		char ltype[15]{}; //14 characters guaranteed
		char mname[33]{}; // +1 more character to see if it's not correct format
		char logmessage[512]{};



		this->m_frLogReader.readStringNL(str, sizeof(str)-1);
		const std::string_view logstr(str);
		const std::size_t len = std::strlen(str);
		if (len == AELOG_ENTRY_MAX_SIZE + 1 || len < 47) { // it is more then the max size or less than possible size -- invalid or...somehow modified
			return AELP_ERR_INVALID_LENGTH;
		}

		//check the log entry's time
		std::time_t entryTime = ace::utils::stringToDate(logstr.substr(POS_TIME, 20), "[%Y-%m-%d.%X");
		if (entryTime == -1) {
			return AELP_ERR_INVALID_TIME;
		}
		

		// check for the "] " between the time and type
		if (std::memcmp(logstr.substr(POS_TYPE-2).data(), "] ", 2)) { 
			return AELP_ERR_INVALID_FORMAT;
		}

		// check the log entry's type
		
		cint entryType = 0;
		if (sscanf(logstr.substr(POS_TYPE).data(), "[%14[^]] ", ltype) == 1 && // read string
			(entryType = AELogParser::matchLogType(ltype)) != -1) { // check for type
			
		}
		else {
			return AELP_ERR_INVALID_TYPE; //oops!
		}
		
		// check for the "] " between the type and module name
		if (std::memcmp(logstr.substr(POS_MNAME - 2).data(), "] ", 2)) { 
			return AELP_ERR_INVALID_FORMAT;
		}
			
		
		std::string_view mname_v;
		if (sscanf(logstr.substr(POS_MNAME).data(), "[%32[^]] ", mname) == 1 && // read string
			(mname_v = mname).size() <= 31 && // see for the length (if we're more than 31 - we certainly failed)
			ace::utils::isAlNumUs(mname_v)) { // check if it's alphanumeric
			
		}
		else {
			return AELP_ERR_INVALID_MNAME;
		}

		//okay. we might have read that.
		//Since the module name is variable and cannot be predicted
		//We might have read untill the end of the "entry"
		//so check it!
		if (logstr.size() < (POS_MNAME + mname_v.size() + 5)) { //position of module name (39)+ size of the mname + 5 characters forward (leading [, trailing "]: ", and 1 character for message)
			return AELP_ERR_INVALID_LENGTH;
		}
		
		//cool, now onwards to text
		//check for the existing boundary between module name and message
		if (std::memcmp(logstr.substr(POS_MNAME + mname_v.size() + 1).data(), "]: ", 3)) {
			return AELP_ERR_INVALID_FORMAT; //oops
		}
		
		//cool, passed. now read untill the end
		//the newline and or the size is guaranteed to exist
		sscanf(logstr.substr(POS_MNAME + mname_v.size() + 4).data(), "%[^511\n]", logmessage);
		


		std::memcpy(entry.m_sLogMessage, logmessage, 511); //message
		std::memcpy(entry.m_sModuleName, mname, 31); //module name
		entry.m_tmLogTime = entryTime; //time
		entry.m_ullOrderNum = m_ullLogEntries++;
		entry.m_cStatus = AELOG_ENTRY_STATUS_READY;
		entry.m_cLogType = entryType; //type
		
		
		




		return AELP_ERR_NOERROR;
	}


	void parseEntries(const cint filterFlags) {






	}



private:

	static cint matchLogType(const std::string_view str) noexcept {

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
	/// The array of the arrays of log entry cursor indices in the file
	std::array<std::vector<ullint>, 8> m_arrEntryIndices;
	/// The amount of log entries read in the file;
	ullint m_ullLogEntries;
	cint m_cLastError;
};

//aaaand register it
REGISTER_CLASS(AELogParser)


#endif // !ENGINE_LOG_PARSER_HPP
