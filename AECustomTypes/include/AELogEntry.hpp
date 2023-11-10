
/** @file AECustomTypes/include/AELogEntry.hpp
 *  This file contains the code for the log entry -- struct containing data from the entry of the log file.
 *
 *  Should not cause everything to break :)
 */

#pragma once

#ifndef ENGINE_AELOGENTRY_HPP
#define ENGINE_AELOGENTRY_HPP

#include "include/AETypedefs.hpp"
#include "include/AEFlags.hpp"
#include <ctime>
#include <atomic>


 //log entry stuff
 /// Macro for the AELogEntry's order number to be invalid.
#define AELOG_ENTRY_INVALID_ORDERNUM 0
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
	char m_sLogMessage[AELOG_ENTRY_MESSAGE_SIZE + 1]{}; //+1 for the last null termination
	/// The name of the module that issued the log entry
	char m_sModuleName[AELOG_ENTRY_MODULENAME_SIZE + 1]{}; //+1 for the last null termination
	/// The time when this log entry was created
	std::time_t m_tmLogTime = NULL;
	/// The pointer to the next log entry in the queue
	AELogEntry* m_pNextNode = nullptr;
	/// The status flag in the entry to show if the entry is ready, being read/set, or is invalid
	std::atomic<cint> m_cStatus = AELOG_ENTRY_STATUS_INVALID;
	/// The type of the log entry
	/// @see AELOG_TYPE_*
	cint m_cLogType = -1;


	static inline void clearEntry(AELogEntry* const entry) noexcept {
		AELogEntry* const next = entry->m_pNextNode;
		/*
		std::memset(entry->m_sLogMessage, NULL, AELOG_ENTRY_MESSAGE_SIZE); // clean log message
		std::memset(entry->m_sModuleName, NULL, AELOG_ENTRY_MODULENAME_SIZE); // clean module name
		entry->m_cStatus = AELOG_ENTRY_STATUS_INVALID; // clear the status
		entry->m_ullOrderNum = AELOG_ENTRY_INVALID_ORDERNUM; // clear the order num
		*/

		std::memset(entry, NULL, sizeof(AELogEntry));
		entry->m_pNextNode = next;
		entry->m_cLogType = -1;
	}

};


#endif // !ENGINE_AELOGENTRY_HPP
