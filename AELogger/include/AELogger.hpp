#pragma once

#ifndef ENGINE_AELOGGER_HPP
#define ENGINE_AELOGGER_HPP

#include "include/AEModuleBase.hpp"
#include "include/AEFileWriter.hpp"


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



