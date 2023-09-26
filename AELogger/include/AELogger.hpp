#pragma once

#ifndef ENGINE_AELOGGER_HPP
#define ENGINE_AELOGGER_HPP

#include "include/AEModuleBase.hpp"
#include "include/AEFileWriter.hpp"
#include <ctime>

//log types
#define AELOG_TYPE_DEBUG 0
#define AELOG_TYPE_INFO 1
#define AELOG_TYPE_WARN 2
#define AELOG_TYPE_SEVERE_WARN 3
#define AELOG_TYPE_OK 4
#define AELOG_TYPE_SUCCESS 5
#define AELOG_TYPE_ERROR 6
#define AELOG_TYPE_FATAL_ERROR 7

//log entry ordernum
#define AELOG_ENTRY_INVALID_ORDERNUM -1

//log entry status

//log entry is invalid
#define AELOG_ENTRY_STATUS_INVALID 0
//log entry is currently being set up and written to
#define AELOG_ENTRY_STATUS_SETTING 1
//log entry is ready to be read
#define AELOG_ENTRY_STATUS_READY 2 
//log entry is currently being read by the thread
#define AELOG_ENTRY_STATUS_READING 3


struct AELogEntry {

	/// The message of the log entry
	char m_sLogMessage[1024]{};
	/// The name of the module that issued the log entry
	char m_sModuleName[64]{};
	/// The time when this log entry was created
	std::time_t m_tmLogTime = 0;
	/// The order(or queue) number of the log entry, safety measure for consequent writing and thread-safety
	/// @note The log-writing thread will ignore this entry, if it is set to AELOG_ENTRY_INVALID_ORDERNUM
	std::atomic<ullint> m_ullOrderNum = AELOG_ENTRY_INVALID_ORDERNUM;
	/// The pointer to the next log entry in the queue
	AELogEntry* m_lepNextNode = nullptr;
	/// The type of the log entry
	/// @see AELOG_TYPE_*
	ucint m_ucLogType = AELOG_TYPE_DEBUG;
	/// The status flag in the entry to show if the entry is ready, being read/set, or is invalid
	std::atomic<cint> m_bStatus = AELOG_ENTRY_STATUS_INVALID;

	/// <summary>
	/// Returns the AELogEntry with nothing in it
	/// </summary>
	/// <returns>Empty and invalid AELogEntry object</returns>
	const static AELogEntry clear() {
		return { {}, {}, 0, ullint(AELOG_ENTRY_INVALID_ORDERNUM), nullptr, AELOG_TYPE_DEBUG, AELOG_ENTRY_STATUS_INVALID };
	}
	
};


class AELogger : public __AEModuleBase<AELogger> {

public:

//constructors
	
	/// <summary>
	/// Class constructor. It opens the file to start the logging process
	/// </summary>
	/// <param name="fname">Name of the log file</param>
	/// <param name="clearLog">Flag to clear the log file if it exists instead of appending it</param>
	AELogger(const std::string& fname, const bool clearLog = false) : 
		m_fwLogger(fname, !clearLog * AEFW_FLAG_APPEND /* Funny magic with bool-int conversion */, 1) {
		
		//do something
	}

	// we don't need the copy-constructor nor copy-assignment operators
	// Okay, okay, fine, we don't need it *for now* 
	// I'll implement multithreading and multiple instances later
	AELogger(const AELogger&) = delete;
	AELogger& operator=(const AELogger&) = delete;

	/// <summary>
	/// Open the file to start logging
	/// </summary>
	/// <param name="fname">Name of the log file</param>
	/// <param name="clearLog">Flag to clear the log file if it exists instead of appending it</param>
	void openLog(const std::string& fname, const bool clearLog = false) {
		this->m_fwLogger.open(fname, !clearLog * AEFW_FLAG_APPEND, 1);
	}

	/// <summary>
	/// Close the file, if it was opened. That's it.
	/// </summary>
	void closeLog(void) {
		this->m_fwLogger.closeFile();
	}

	/// <summary>
	/// Get the name of the log file
	/// </summary>
	/// <returns>std::string of the name of opened log file</returns>
	std::string getLogName(void) const {
		return m_fwLogger.getFileName();
	}
	
	/// <summary>
	/// Get the amount of log entries done to an opened log file
	/// </summary>
	/// <returns>ullint of the amount of times logger written to a file</returns>
	ullint getEntryCount(void) const {
		return m_fwLogger.getTotalWrites();
	}

	/// <summary>
	/// Get the last error status code
	/// </summary>
	/// <returns>ucint of the error code by the filewriter/logger</returns>
	ucint getLastError(void) const {
		return m_fwLogger.getLastError();
	}

private:
	/// <summary>
	/// The file writer to actually write text to opened log file
	/// </summary>
	AEFileWriter m_fwLogger;


	// we don't need to keep the separate variables for the name of the file
	// and amount of log entries we did.
	// All because we: 1) already have it through AEFileWriter
	// And 2) we'll write to file only once, after formatting the strings
};

//aaand we have to register it too
REGISTER_CLASS(AELogger)


#endif // !ENGINE_AELOGGER_HPP



