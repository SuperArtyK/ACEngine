
/** @file AEMisc/include/AETypes.hpp
 *  This file contains custom types and structs used throughout the engine
 *
 *  Should not cause everything to break :)
 */

#pragma once

#ifndef ENGINE_TYPES_HPP
#define ENGINE_TYPES_HPP

#include "AETypedefs.hpp"
#include "AEFlags.hpp"
#include <ctime>
#include <atomic>


//log entry stuff
/// Macro for the AELogEntry's order number to be invalid.
#define AELOG_ENTRY_INVALID_ORDERNUM ullint(-1)
/// Macro for the AELogEntry's status to be: invalid.
#define AELOG_ENTRY_STATUS_INVALID 0
/// Macro for the AELogEntry's status to be: currently being set up and written to.
#define AELOG_ENTRY_STATUS_SETTING 1
/// Macro for the AELogEntry's status to be: currently read.
#define AELOG_ENTRY_STATUS_READY 2 
/// Macro for the AELogEntry's status to be: currently being read by the log-writing thread.
#define AELOG_ENTRY_STATUS_READING 3

#define AELOG_ENTRY_MAX_SIZE (AELOG_ENTRY_MESSAGE_SIZE + AELOG_ENTRY_MODULENAME_SIZE + sizeof("[YYYY-MM-DD.HH:mm:SS] [] []: \n"))

/// <summary>
/// The structure for the log entry data in the queue of AELogger.
/// </summary>
struct AELogEntry {

	/// The message of the log entry
	char m_sLogMessage[AELOG_ENTRY_MESSAGE_SIZE+1]{}; //+1 for the last null termination
	/// The name of the module that issued the log entry
	char m_sModuleName[AELOG_ENTRY_MODULENAME_SIZE+1]{}; //+1 for the last null termination
	/// The time when this log entry was created
	std::time_t m_tmLogTime = NULL;
	/// The order(or queue) number of the log entry, safety measure for consequent writing and thread-safety
	/// @note The log-writing thread will ignore this entry, if it is set to AELOG_ENTRY_INVALID_ORDERNUM
	std::atomic<ullint> m_ullOrderNum = AELOG_ENTRY_INVALID_ORDERNUM;
	/// The pointer to the next log entry in the queue
	AELogEntry* m_pNextNode = nullptr;
	/// The status flag in the entry to show if the entry is ready, being read/set, or is invalid
	std::atomic<cint> m_cStatus = AELOG_ENTRY_STATUS_INVALID;
	/// The type of the log entry
	/// @see AELOG_TYPE_*
	cint m_cLogType = -1;


	static inline void clearEntry(AELogEntry* entry) {
		std::memset(entry->m_sLogMessage, NULL, AELOG_ENTRY_MESSAGE_SIZE); // clean log message
		std::memset(entry->m_sModuleName, NULL, AELOG_ENTRY_MODULENAME_SIZE); // clean module name
		entry->m_cStatus = AELOG_ENTRY_STATUS_INVALID; // clear the status
		entry->m_ullOrderNum = AELOG_ENTRY_INVALID_ORDERNUM; // clear the order num
	}


	static inline void clearEntry2(AELogEntry* entry) {
		std::memset(entry->m_sLogMessage, NULL, AELOG_ENTRY_MESSAGE_SIZE); // clean log message
		std::memset(entry->m_sModuleName, NULL, AELOG_ENTRY_MODULENAME_SIZE); // clean module name
		entry->m_pNextNode = nullptr;
		entry->m_cStatus = AELOG_ENTRY_STATUS_INVALID; // clear the status
		entry->m_ullOrderNum = AELOG_ENTRY_INVALID_ORDERNUM; // clear the order num
	}
};


#endif // !ENGINE_TYPES_HPP
