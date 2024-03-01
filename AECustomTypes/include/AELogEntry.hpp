
/*! @file AELogEntry/include/AELogEntry.hpp
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

#include "AELogTypes.hpp"
#include "include/AEModuleBase.hpp" // just for the ENGINE_MODULE_ERR_NOERROR flag; AELE isn't a full module
#include "include/AEUtils.hpp"
#include <atomic>


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
/// Macro for the minimum size of the formatted AELogEntry as a string
#define AELE_FORMAT_MIN_SIZE (sizeof("[YYYY-MM-DD.HH:mm:SS] [INFO          ] [A]: B"))
/// The size of the AELogEntry debug message in characters (with "DEBUG->" in front) (excluding null termination)
#define AELE_MESSAGE_DEBUG_SIZE (AELE_MESSAGE_SIZE + sizeof("DEBUG->") - 1)

/// Macro for the snprintf format of the AELogEntry, when converting it to a string
#define AELE_FORMAT_STRING "[%s] [%-14s] [%s]: %s\n"
/// Macro for the snprintf format of the AELogEntry (of type DEBUG), when converting it to a string
#define AELE_FORMAT_STRING_DEBUG "[%s] [%-14s] [%s]: DEBUG->%s\n"


//Error flags
/// Macro for the indicator that everything is good/no error was encountered in the process
#define AELE_ERR_NOERROR ENGINE_MODULE_ERR_NOERROR
/// Macro for the error that occurs if the passed log string has an invalid length (bigger than maximum, smaller than minimum)
#define AELE_ERR_INVALID_LENGTH -11
/// Macro for the error that occurs if the passed log string has an invalid text time format
#define AELE_ERR_INVALID_TIME -12
/// Macro for the error that occurs if the passed log string has an invalid type/severity
#define AELE_ERR_INVALID_TYPE -13
/// Macro for the error that occurs if the passed log string has an invalid module name
/// Invalid module name is more than 31 characters long, and/or isn't alpha-numeric with underscores (has special symbols)
#define AELE_ERR_INVALID_MNAME -14
/// Macro for the error that occurs if the passed log string has an invalid format overall
/// Invalid format is not having "][" between parts, and not corresponding to position values
#define AELE_ERR_INVALID_FORMAT -15

//Parse string flags
/// Macro-flag for the parseStringEntry() to not write the data to the given entry object (essentially, just validate it)
#define AELE_PARSE_STRING_JUST_VALIDATE 0b0000000	//decimal: 0;  hex: 0
/// Macro-flag for the parseStringEntry() to write just the entry's message to the given entry object
#define AELE_PARSE_STRING_MESSG   0b0000001	//decimal: 1;  hex: 1
/// Macro-flag for the parseStringEntry() to write just the entry's module name to the given entry object
#define AELE_PARSE_STRING_MNAME   0b0000010	//decimal: 2;  hex: 2
/// Macro-flag for the parseStringEntry() to write just the entry's timestamp to the given entry object
#define AELE_PARSE_STRING_TIME    0b0000100	//decimal: 4;  hex: 4
/// Macro-flag for the parseStringEntry() to write just the entry's type to the given entry object
#define AELE_PARSE_STRING_TYPE    0b0001000	//decimal: 8;  hex: 8
/// Macro-flag for the parseStringEntry() to parse the log entry and write all the data to the given entry object (default behaviour)
#define AELE_PARSE_STRING_FULL          0b0001111	//decimal: 15; hex: F



/// <summary>
/// The structure for the data of the log entry to be manipulated with the log files.
/// The flags start with AELE_
/// </summary>
struct AELogEntry {

	/// The message of the log entry
	char m_sLogMessage[AELE_MESSAGE_SIZE + 1]{}; //+1 for the last null termination
	/// The name of the module that issued the log entry
	char m_sModuleName[AELE_MODULENAME_SIZE + 1]{}; //+1 for the last null termination
	/// The time when this log entry was created
	std::time_t m_tmLogTime = AENULL; //looong time ago, lol
	/// The pointer to the next log entry in the queue
	AELogEntry* m_pNextNode = nullptr;
	/// The type of the log entry
	/// @see AELogTypes.hpp
	cint m_cLogType = -1; //means invalid entry. don't touch!
	/// The status flag in the entry to show if the entry is ready, being read/set, or is invalid
	std::atomic<cint> m_cStatus = AELE_STATUS_INVALID;


	/// <summary>
	/// Copy assignment operator -- copies data from the passed node
	/// @note The pointer to the next node is omitted from the operation;
	/// @note Uses the AELogEntry::copyEntry()
	/// </summary>
	/// <param name="entry"></param>
	/// <returns></returns>
	AELogEntry& operator=(const AELogEntry& entry) noexcept {
		this->copyEntry(entry);
		return *this;
	}

	/// <summary>
	/// Makes a full copy of the log entry, including the pointer to the next node
	/// </summary>
	/// <param name="entry">The entry to copy data from</param>
	inline void copyEntryFull(const AELogEntry& entry) noexcept {
		if (this == &entry) {
			return; // we're copying ourselves!
		}

		std::memcpy(this, &entry, aeoffsetof(AELogEntry, m_cStatus));
		this->m_cStatus.store(entry.m_cStatus);
	}

	/// <summary>
	/// Makes a normal copy of the log entry, copying everything but the pointer to the next node
	/// </summary>
	/// <param name="entry">The entry to copy data from</param>
	inline void copyEntry(const AELogEntry& entry) noexcept {
		if (this == &entry) {
			return; // we're copying ourselves!
		}

		std::memcpy(this, &entry, aeoffsetof(AELogEntry, m_pNextNode));
		this->m_cLogType = entry.m_cLogType;
		this->m_cStatus.store(entry.m_cStatus);
	}

	/// <summary>
	/// Makes a reduced copy of the log entry, including only the message, module name, and type.
	/// </summary>
	/// <param name="entry">The entry to copy data from</param>
	inline void copyEntryReduced(const AELogEntry& entry) noexcept {
		if (this == &entry) {
			return; // we're copying ourselves!
		}

		std::memcpy(this, &entry, aeoffsetof(AELogEntry, m_pNextNode));
		this->m_cLogType = entry.m_cLogType;
	}

	/// <summary>
	/// Clears the current entry and sets its values to zero/invalid
	/// </summary>
	/// <param name="entry">The entry to clear</param>
	static inline void clearEntry(AELogEntry& entry) noexcept {
		std::memset(&entry, AENULL, aeoffsetof(AELogEntry, m_pNextNode));
		entry.m_cLogType = -1;
		entry.m_cStatus = AELE_STATUS_INVALID;
	}

	/// <summary>
	/// Fully clears the entry, resetting it to the default values
	/// </summary>
	/// <param name="entry"></param>
	static inline void clearEntryFull(AELogEntry& entry) noexcept {
		entry.copyEntryFull(AELogEntry::emptyEntry());
	}

	/// <summary>
	/// Allocates the queue of the given size on the heap and returns the pointer to it's first node.
	/// Optionally may loop the newly-allocated queue to the old queue.
	/// @note You should delete[] the pointer after you're done using it (unless you like mem-leaks)
	/// @note If the amt is 0, throws the std::runtime exception
	/// </summary>
	/// <param name="amt">The amount of entries in the queue(size)</param>
	/// <param name="loopQueue">The flag to indicate whether to loop the generated queue at all</param>
	/// <param name="oldqueue">The pointer to the old queue to loop the new queue to.</param>
	/// <returns>Pointer to the first node of the allocated queue</returns>
	static inline AELogEntry* makeQueue(const std::size_t amt, const bool loopQueue = true, AELogEntry * oldqueue = nullptr) {

		//assert that amt is not 0. This is never supposed to trigger
		//Otherwise, we're in for a hell of a ride
		aceAssert(amt != 0, "The queue size to allocate is 0!");

		//allocate new log entry list
		AELogEntry* leptr = new AELogEntry[amt]{};
		for (std::size_t i = 0; i < amt - 1; i++) {
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

	/// <summary>
	/// Formats the passed entry to the given c-string
	/// @note The size of the c-string must be of AELE_FORMAT_MAX_SIZE !
	/// </summary>
	/// <param name="str">The c-string to format the data to</param>
	/// <param name="entry">The entry object to format its data to string</param>
	/// <returns>The length of the resulting string</returns>
	static inline int formatEntry(char* const str, const AELogEntry& entry) noexcept {
		//string that stores the date and time formatted string
		char timestr[DATETIME_STRING_SIZE]{};

		if (entry.m_cLogType == AELOG_TYPE_DEBUG) {
			return snprintf(str, AELE_FORMAT_MAX_SIZE, AELE_FORMAT_STRING_DEBUG, ace::utils::formatDate(entry.m_tmLogTime, timestr), AELogEntry::typeToString(entry.m_cLogType), entry.m_sModuleName, entry.m_sLogMessage);
		}
		else {
			return snprintf(str, AELE_FORMAT_MAX_SIZE, AELE_FORMAT_STRING, ace::utils::formatDate(entry.m_tmLogTime, timestr), AELogEntry::typeToString(entry.m_cLogType), entry.m_sModuleName, entry.m_sLogMessage);
		}

	}

	/// <summary>
	/// Deduces the entry's log type and returns a c-string of it.
	/// </summary>
	/// <param name="logtype">The value of the log type</param>
	/// <returns>c-string of the type</returns>
	static constexpr const char* const typeToString(const cint logtype) noexcept {
		switch (logtype) {
			case AELOG_TYPE_INFO: return "INFO";
			case AELOG_TYPE_WARN: return "WARNING"; case AELOG_TYPE_SEVERE_WARN: return "SEVERE_WARNING";
			case AELOG_TYPE_OK: return "OK"; case AELOG_TYPE_SUCCESS: return "SUCCESS";
			case AELOG_TYPE_ERROR: return "ERROR"; case AELOG_TYPE_FATAL_ERROR: return "FATAL_ERROR";
			case AELOG_TYPE_DEBUG: return "DEBUG"; default: return "WRONG_TYPE!";
		}
	}

	/// <summary>
	/// Deduces the string representation of a type (as appears in the log file) to a corresponding type number
	/// @note the size of the string needs to be 14 characters total, format: [TYPE IN CAPS][remaining space padding to make it 14 characters]
	/// </summary>
	/// <param name="str">The string to check</param>
	/// <returns>The number corresponding to the deduced type; AELOG_TYPE_INVALID otherwise</returns>
	static constexpr cint stringToType(const std::string_view str) noexcept {
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
		
		return AELOG_TYPE_INVALID;
	}

	/// <summary>
	/// Parses a given string as a log entry, and write it into passed entry object
	/// @note If the string is less than AELE_FORMAT_MIN_SIZE or more than AELE_FORMAT_MAX_SIZE, fails the check with AELE_ERR_INVALID_LENGTH
	/// </summary>
	/// <param name="entry">The log entry object to write data to</param>
	/// <param name="entryString">The string to parse</param>
	/// <param name="flags">The flags for parsing. Refer to AELE_PARSE_STRING_* (like AELE_PARSE_STRING_JUST_VALIDATE)</param>
	/// <returns>AELE_ERR_NOERROR (0) on success, other AELE error flags otherwise</returns>
	static inline cint parseStringEntry(AELogEntry& entry, const std::string_view entryString, const cint flags = AELE_PARSE_STRING_FULL) {

		constexpr std::size_t POS_TYPE = 22, POS_MNAME = 39;
		if (entryString.size() > AELE_FORMAT_MAX_SIZE || entryString.size() < AELE_FORMAT_MIN_SIZE) { // it is more then the max size or less than possible size -- invalid or...somehow modified
			return AELE_ERR_INVALID_LENGTH;
		}

		char logmessage[AELE_MESSAGE_DEBUG_SIZE+1]{}; //log message, exactly 518 characters, you know, in case it's a debug string
		char mname[AELE_MODULENAME_SIZE + 2]{}; // +1 more character to see if it's not correct format
		char logType[15]{}; //14 characters guaranteed

		char scanfFormat[16]{}; //the format string for the sscanf. Since (sigh) macros dont evaluate while in preprocessor stage

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
		if (std::sscanf(entryString.data() + POS_TYPE, "[%14[^]] ", logType) != 1) {
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
		std::snprintf(scanfFormat, sizeof(scanfFormat), "[%%%llu[^]]", ullint(AELE_MODULENAME_SIZE + 1));
		if (std::sscanf(entryString.data() + POS_MNAME, scanfFormat, mname) != 1) {
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
		AELogEntry::clearEntry(entry);

		if (flags & AELE_PARSE_STRING_MESSG) {
			//cool, passed. now read untill the end
			//the newline and or the size is guaranteed to exist
			std::snprintf(scanfFormat, sizeof(scanfFormat), "%%%llu[^\n]", AELE_MESSAGE_DEBUG_SIZE);
			std::sscanf(entryString.data() + POS_MNAME + strvMname.size() + 4, scanfFormat, logmessage);
			// what if the entry is a debug one?
			if (entryType == AELOG_TYPE_DEBUG && std::strstr(logmessage, "DEBUG->") != nullptr) {
				std::memcpy(entry.m_sLogMessage, logmessage + sizeof("DEBUG->") - 1, AELE_MESSAGE_SIZE - sizeof("DEBUG->") + 1);
			}
			else {
				std::memcpy(entry.m_sLogMessage, logmessage, AELE_MESSAGE_SIZE);
			}

		}

		if (flags & AELE_PARSE_STRING_MNAME) {
			std::memcpy(entry.m_sModuleName, mname, strvMname.size()); //module name
		}
		if (flags & AELE_PARSE_STRING_TIME) {
			entry.m_tmLogTime = entryTime; //time
		}
		if (flags & AELE_PARSE_STRING_TYPE) {
			entry.m_cLogType = entryType; //type
		}


		if (flags & AELE_PARSE_STRING_FULL) {
			entry.m_cStatus = AELE_STATUS_READY; //status
		}

		return AELE_ERR_NOERROR;
	}

	/// <summary>
	/// Format the entry and convert it into std::string
	/// </summary>
	/// <returns>std::string of the converted entry</returns>
	std::string toString() {
		char temp[AELE_FORMAT_MAX_SIZE];
		AELogEntry::formatEntry(temp, *this);

		return temp;
	}

	/// <summary>
	/// Returns the empty, invalid entry, with no data
	/// </summary>
	/// <returns>AELogEntry instance of an empty entry</returns>
	static constexpr AELogEntry emptyEntry() noexcept {
		return AELogEntry{};
	}

};

#endif // !ENGINE_AELOGENTRY_HPP
