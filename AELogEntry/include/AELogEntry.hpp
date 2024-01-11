
/** @file AECustomTypes/include/AELogEntry.hpp
 *  This file contains the code for the log entry -- struct containing data from the entry of the log file.
 *  And the code for it's basic functions, like copying, clearing, formatting to string and back,
 *  deducing its type to string and making the queue
 *
 *  Should not cause everything to break :)
 */

#pragma once

#pragma warning(disable : 4996)

#ifndef ENGINE_AELOGENTRY_HPP
#define ENGINE_AELOGENTRY_HPP

#include "include/AETypedefs.hpp"
#include "include/AEFlags.hpp"
#include "include/AEUtils.hpp"
#include "include/AELogTypes.hpp"
#include "include/AEModuleBase.hpp" // just for the ENGINE_MODULE_ERR_NOERROR flag; AELE isn't a full module
#include <ctime>
#include <atomic>
#include <iostream>


//log entry stuff
/// Macro for the AELogEntry's status to be: invalid.
#define AELE_STATUS_INVALID 0
/// Macro for the AELogEntry's status to be: currently being set up and written to.
#define AELE_STATUS_SETTING 1
/// Macro for the AELogEntry's status to be: currently ready to be parsed and written to file.
#define AELE_STATUS_READY 2 
/// Macro for the AELogEntry's status to be: currently being read by the log-writing thread.
#define AELE_STATUS_READING 3
/// Macro for the maximum size of the formatted AELogEntry as a string
#define AELE_FORMAT_MAX_SIZE (AELE_MESSAGE_SIZE + AELE_MODULENAME_SIZE + sizeof("[YYYY-MM-DD.HH:mm:SS] [] []: DEBUG->\n"))
/// Macro for the snprintf format of the AELogEntry, when converting it to a string
#define AELE_FORMAT_STRING "[%s] [%-14s] [%s]: %s\n"
/// Macro for the snprintf format of the AELogEntry (of type DEBUG), when converting it to a string
#define AELE_FORMAT_STRING_DEBUG "[%s] [%-14s] [%s]: DEBUG->%s\n"

#define AELE_ERR_NOERROR ENGINE_MODULE_ERR_NOERROR
#define AELE_ERR_INVALID_LENGTH -11
#define AELE_ERR_INVALID_TIME -12
#define AELE_ERR_INVALID_TYPE -13
#define AELE_ERR_INVALID_MNAME -14
#define AELE_ERR_INVALID_FORMAT -15
#define AELE_PARSE_STRING_JUST_VALIDATE 0b0000000	//decimal: 0;  hex: 0
#define AELE_PARSE_STRING_WRITE_MESSG   0b0000001	//decimal: 1;  hex: 1
#define AELE_PARSE_STRING_WRITE_MNAME   0b0000010	//decimal: 2;  hex: 2
#define AELE_PARSE_STRING_WRITE_TIME    0b0000100	//decimal: 4;  hex: 4
#define AELE_PARSE_STRING_WRITE_TYPE    0b0001000	//decimal: 8;  hex: 8
#define AELE_PARSE_STRING_FULL          0b0001111	//decimal: 15; hex: F



/// <summary>
/// The structure for the log entry data in the queue of AELogger.
/// </summary>
struct AELogEntry {

	/// The message of the log entry
	char m_sLogMessage[AELE_MESSAGE_SIZE + 1]{}; //+1 for the last null termination
	/// The name of the module that issued the log entry
	char m_sModuleName[AELE_MODULENAME_SIZE + 1]{}; //+1 for the last null termination
	/// The time when this log entry was created
	std::time_t m_tmLogTime = NULL;
	/// The pointer to the next log entry in the queue
	AELogEntry* m_pNextNode = nullptr;
	/// The status flag in the entry to show if the entry is ready, being read/set, or is invalid
	std::atomic<cint> m_cStatus = AELE_STATUS_INVALID;
	/// The type of the log entry
	/// @see AELogTypes.hpp
	cint m_cLogType = -1; //means invalid entry. don't touch!

	/// <summary>
	/// Copy assignment operator -- copies data from the passed node
	/// @note The pointer to the next node is omitted from the operation;
	/// </summary>
	/// <param name="entry"></param>
	/// <returns></returns>
	AELogEntry& operator=(const AELogEntry& entry) {
		this->copyEntry(entry);
	}

	// makes full copy of the given entry
	// ctrl-c ctrl-v
	inline void copyEntryFull(const AELogEntry& entry) {
		std::memcpy(this, &entry, sizeof(AELogEntry));
	}

	// makes normal copy of the given entry
	// only omits the pointer to the next one
	// (you dont need it anyway)
	inline void copyEntry(const AELogEntry& entry) {
		std::memcpy(this, &entry, aeoffsetof(AELogEntry, m_pNextNode));
		this->m_cLogType = entry.m_cLogType;
		this->m_cStatus = entry.m_cStatus.load();
	}

	// makes a reduced data copy of the given entry
	// omits the next node pointer and status
	inline void copyEntryReduced(const AELogEntry& entry) {
		std::memcpy(this, &entry, aeoffsetof(AELogEntry, m_pNextNode));
		this->m_cLogType = entry.m_cLogType;
	}

	/// <summary>
	/// Clears the current entry and sets its values to zero/invalid
	/// </summary>
	/// <param name="entry">The entry to clear</param>
	static inline void clearEntry(AELogEntry* const entry) noexcept {
		std::memset(entry, NULL, aeoffsetof(AELogEntry, m_pNextNode));
		entry->m_cLogType = -1;
		entry->m_cStatus = AELE_STATUS_INVALID;
		
	}	

	/// <summary>
	/// Allocates the queue of the given size on the heap and returns the pointer to it's first node.
	/// Optionally may loop the newly-allocated queue to the old queue.
	/// @note You should delete[] the pointer after you're done using it (unless you like mem-leaks)
	/// @note If the amt is 0, throws the std::runtime exception
	/// </summary>
	/// <param name="amt">The amount of entries in the queue(size)</param>
	/// <param name="oldqueue">The pointer to the old queue to loop the new queue to.</param>
	/// <param name="loopQueue">The flag to indicate whether to loop the generated queue at all</param>
	/// <returns>Pointer to the first node of the allocated queue</returns>
	static inline AELogEntry* makeQueue(const std::size_t amt, AELogEntry* oldqueue = nullptr, const bool loopQueue = true) {

		//assert that amt is not 0. This is never supposed to trigger
		//Otherwise, we're in for a hell of a ride
		aceAssert(amt != 0, "The queue size to allocate is 0!");

		//allocate new log entry list
		AELogEntry* leptr = new AELogEntry[amt]{};
		for (std::size_t i = 0; i < amt-1; i++) {
			leptr[i].m_pNextNode = leptr + i + 1; //set the next node pointers for our linked list
		}
		//loop the last log entry to the beginning

		if (loopQueue) {
			if (oldqueue) {
				leptr[amt - 1].m_pNextNode = oldqueue; //beginning of old queue
			}
			else {
				leptr[amt - 1].m_pNextNode = leptr; //beginning of itself
			}
		}

		return leptr;
	}

	//the size of char array (string together with null terminator) must be of AELE_FORMAT_MAX_SIZE
	static inline void formatEntry(char* const str, const AELogEntry& entry) {
		//string that stores the date and time formatted string
		char timestr[DATETIME_STRING_SIZE]{};


		if (entry.m_cLogType == AELOG_TYPE_DEBUG) {
			snprintf(str, AELE_FORMAT_MAX_SIZE, AELE_FORMAT_STRING_DEBUG, ace::utils::formatDate(entry.m_tmLogTime, timestr), AELogEntry::typeToString(entry.m_cLogType), entry.m_sModuleName, entry.m_sLogMessage);
		}
		else {
			snprintf(str, AELE_FORMAT_MAX_SIZE, AELE_FORMAT_STRING, ace::utils::formatDate(entry.m_tmLogTime, timestr), AELogEntry::typeToString(entry.m_cLogType), entry.m_sModuleName, entry.m_sLogMessage);
		}



	}
	
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

	static constexpr cint stringToType(const std::string_view str) {
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


	//parse a log entry from a string, and write it into given entry
	static inline cint parseStringEntry(AELogEntry& entry, const std::string_view entryString, const cint flags = AELE_PARSE_STRING_FULL) {

		constexpr std::size_t POS_TYPE = 22, POS_MNAME = 39;
		if (entryString.size() == AELE_FORMAT_MAX_SIZE + 1 || entryString.size() < 47) { // it is more then the max size or less than possible size -- invalid or...somehow modified
			return AELE_ERR_INVALID_LENGTH;
		}

		char logmessage[AELE_MESSAGE_SIZE + sizeof("DEBUG->")]{}; //log message, exactly 518 characters, you know, in case it's a debug string
		char mname[AELE_MODULENAME_SIZE + 2]{}; // +1 more character to see if it's not correct format
		char logType[15]{}; //14 characters guaranteed

		//check the log entry's time
		const std::time_t entryTime = ace::utils::stringToDate(entryString.data(), "[%Y-%m-%d.%X");
		if (entryTime == -1) {
			return AELE_ERR_INVALID_TIME;
		}

		// check for the "] " between the time and type
		if (std::memcmp(entryString.data() + POS_TYPE - 2, "] ", 2)) {
			return AELE_ERR_INVALID_FORMAT;
		}

		// read the possible type
		if (sscanf(entryString.data() + POS_TYPE, "[%14[^]] ", logType) != 1) {
			return AELE_ERR_INVALID_TYPE; //oops!
		}

		// check for the valid log entry's type
		const cint entryType = AELogEntry::stringToType(logType);
		if (entryType == -1) {
			return AELE_ERR_INVALID_TYPE; //oops!
		}


		// check for the "] " between the type and module name
		if (std::memcmp(entryString.data() + POS_MNAME - 2, "] ", 2)) {
			return AELE_ERR_INVALID_FORMAT;
		}

		// read the possible module name
		if (sscanf(entryString.data() + POS_MNAME, "[%32[^]] ", mname) != 1) {
			return AELE_ERR_INVALID_MNAME;
		}

		// check for the correct module name
		const std::string_view strvMname(mname);
		if (strvMname.size() > 31 || !ace::utils::isAlNumUs(strvMname)) {
			return AELE_ERR_INVALID_MNAME;
		}

		//okay. we might have read that.
		//Since the module name is variable and cannot be predicted
		//We might have read untill the end of the "entry"
		//so check it!
		if (entryString.size() < (POS_MNAME + strvMname.size() + 5)) { //position of module name (39)+ size of the mname + 5 characters forward (leading [, trailing "]: ", and 1 character for message)
			return AELE_ERR_INVALID_LENGTH;
		}

		//cool, now onwards to text
		//check for the existing boundary between module name and message
		if (std::memcmp(entryString.data() + POS_MNAME + strvMname.size() + 1, "]: ", 3)) {
			return AELE_ERR_INVALID_FORMAT; //oops
		}

		if (flags == AELE_PARSE_STRING_JUST_VALIDATE) {
			return AELE_ERR_NOERROR;
		}

		//time to write stuff
		AELogEntry::clearEntry(&entry);

		if (flags & AELE_PARSE_STRING_WRITE_MESSG) {
			//cool, passed. now read untill the end
			//the newline and or the size is guaranteed to exist
			sscanf(entryString.data() + POS_MNAME + strvMname.size() + 4, "%518[^\n]", logmessage);
			// what if the entry is a debug one?
			if (entryType == AELOG_TYPE_DEBUG && std::strstr(logmessage, "DEBUG->") != nullptr) {
				std::memcpy(entry.m_sLogMessage, logmessage + sizeof("DEBUG->") - 1, AELE_MESSAGE_SIZE - sizeof("DEBUG->") + 1);
			}
			else {
				std::memcpy(entry.m_sLogMessage, logmessage, AELE_MESSAGE_SIZE);
			}

		}

		if (flags & AELE_PARSE_STRING_WRITE_MNAME) {
			std::memcpy(entry.m_sModuleName, mname, strvMname.size()); //module name
		}
		if (flags & AELE_PARSE_STRING_WRITE_TIME) {
			entry.m_tmLogTime = entryTime; //time
		}
		if (flags & AELE_PARSE_STRING_WRITE_TYPE) {
			entry.m_cLogType = entryType; //type
		}


		if (flags & AELE_PARSE_STRING_FULL) {
			entry.m_cStatus = AELE_STATUS_READY; //status
		}

		return AELE_ERR_NOERROR;
	}
};


#pragma warning(enable : 4996)

#endif // !ENGINE_AELOGENTRY_HPP
