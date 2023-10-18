
/** @file AELogParser/include/AELogParser.hpp
 *  This file contains the [stuff]
 *
 *  Should not cause everything to break :)
 */

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
#define AELP_ERR_INVALID_ENTRY 2
#define AELP_ERR_INVALID_TIME 3
#define AELP_ERR_INVALID_FORMAT 4




/// @todo add function to parse 1 line
/// @todo add a way to filter the logs by severity
/// @todo add a way to error out about invalid entry
/// @todo add reading entries from the whole file and record their order
/// @todo add filtering entries from the whole file
/// @todo add a way to validate the entry without reading it (if possible) -> populate the entry index array
/// @todo add reading of the entry upon request, by giving it's order number in the log
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

		char str[AELOG_ENTRY_MAX_SIZE + 2]{}; // 1 character more than the log entry - to determine the size
		const std::string_view logformat = "[%Y-%m-%d.%X] [] [%]: \n"; //format

		char curChar = NULL;

		this->m_frLogReader.readStringNL(str, sizeof(str)-1);
		const std::string_view logstr(str);
		const std::size_t len = std::strlen(str);
		if (len == AELOG_ENTRY_MAX_SIZE + 1) { // it is more then the max size -- invalid or...somehow modified
			return AELP_ERR_INVALID_LENGTH;
		}


		std::time_t entryTime = ace::utils::stringToDate(logstr.substr(0, 40), "[%Y-%m-%d.%X] ");
		if (entryTime == -1) {
			return AELP_ERR_INVALID_TIME;
		}
		entry.m_tmLogTime = entryTime;


		


		
			
	}


	void parseEntries(const cint filterFlags) {






	}



private:
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
