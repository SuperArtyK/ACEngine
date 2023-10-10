
/** @file AELogger/include/AELogger.hpp
 *  This file contains the code for the engine's logger module, err...for logging data to file.
 *
 *  Should not cause everything to break :)
 */

#pragma once

#ifndef ENGINE_AELOGGER_HPP
#define ENGINE_AELOGGER_HPP

#include "include/AEModuleBase.hpp"
#include "include/AEFileWriter.hpp"
#include "include/AETypedefs.hpp"
#include "include/AEFlags.hpp"
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>
#include <string_view>


//log types
/// Macro for the AELogger's debug entry.
#define AELOG_TYPE_DEBUG 0
/// Macro for the AELogger's informational entry.
#define AELOG_TYPE_INFO 1
/// Macro for the AELogger's warning entry (something isn't right).
#define AELOG_TYPE_WARN 2
/// Macro for the AELogger's severe warning entry (something is wrong but it's not an error).
#define AELOG_TYPE_SEVERE_WARN 3
/// Macro for the AELogger's OK entry (something is good, like a check).
#define AELOG_TYPE_OK 4
/// Macro for the AELogger's successful entry (something done is successfull..like a module start).
#define AELOG_TYPE_SUCCESS 5
/// Macro for the AELogger's errorneus entry (something is bad..it's an error).
#define AELOG_TYPE_ERROR 6
/// Macro for the AELogger's fatally-errorenous entry (something critical is fatally-bad).
#define AELOG_TYPE_FATAL_ERROR 7

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



/// <summary>
/// The structure for the log entry data in the queue of AELogger.
/// </summary>
struct AELogEntry {

	/// The message of the log entry
	char m_sLogMessage[512]{};
	/// The name of the module that issued the log entry
	char m_sModuleName[32]{};
	/// The time when this log entry was created
	std::time_t m_tmLogTime = 0;
	/// The order(or queue) number of the log entry, safety measure for consequent writing and thread-safety
	/// @note The log-writing thread will ignore this entry, if it is set to AELOG_ENTRY_INVALID_ORDERNUM
	std::atomic<ullint> m_ullOrderNum = AELOG_ENTRY_INVALID_ORDERNUM;
	/// The pointer to the next log entry in the queue
	AELogEntry* m_lepNextNode = nullptr;
	/// The status flag in the entry to show if the entry is ready, being read/set, or is invalid
	std::atomic<cint> m_cStatus = AELOG_ENTRY_STATUS_INVALID;
	/// The type of the log entry
	/// @see AELOG_TYPE_*
	cint m_cLogType = AELOG_TYPE_DEBUG;

	
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
		case AELOG_TYPE_DEBUG: /*same as the default / invalid value*/ default: return "DEBUG";
		}
	}

	/// <summary>
	/// Allocates the queue of the given size on the heap and returns the pointer to it's first node.
	/// Optionally may loop the newly-allocated queue to the old queue.
	/// @note You should delete[] the pointer after you're done using it (unless you like mem-leaks)
	/// @note If the amt is 0, throws the std::runtime exception
	/// </summary>
	/// <param name="amt">The amount of entries in the queue(size)</param>
	/// <param name="oldqueue">The pointer to the old queue to loop the new queue to.</param>
	/// <returns>Pointer to the first node of the allocated queue</returns>
	static AELogEntry* makeQueue(const std::size_t amt, AELogEntry* oldqueue = nullptr);
};


// queue decrease algorithm
// since we have the access to the maximum queue value, and it is being checked in the 
// ptrFromIndex(), we have a way to restrict the access to certain nodes for the writeToLog()
// All by changing the max queue value to the value of the second-to-last node in the allocation vector
// That way, when we wrap the index around next time, it wraps around the nodes that we want to keep
// And completely ignores the last nodes we want to delete.
// Though how to delete information from those delete-me nodes, since the writer-thread is the only thing
// That still has access to those nodes. And we don't want to delete the unfilled ones



/// <summary>
/// @brief The ArtyK's Engine's Logger module -- it manages the writing to the log files.
/// 
/// The log is written by requesting and filling the entry in the queue.
/// The AELogger instance launches the separate thread that looks at the entries in the queue,
/// retrieves and formats the data in them, and writes it to the file. Afterwards that entry in the queue is cleared.
/// The queue can expand if it's too little. But....I don't know how to shrink it. 
/// </summary>
/// @todo Implement copy constructors and copy assignment
/// @todo Add the ability to open the same log file/redirect the instance requests to the one that has it open first.
/// @todo Add the ability to change the default log file folder to...whatever user wants (maybe a constructor)
class AELogger : public __AEModuleBase<AELogger> {

public:

//constructors
	/// <summary>
	/// Class constructor -- it opens the file to start the logging process.
	/// </summary>
	/// <param name="fname">Name of the log file</param>
	/// <param name="clearLog">Flag to clear the log file if it exists instead of appending it</param>
	/// <param name="queuesize">The size of the queue to create when creating AELogger instance</param>
	explicit AELogger(const std::string_view fname, const bool clearLog = false, const ullint queuesize = AELOG_DEFAULT_QUEUE_SIZE);

	AELogger(const std::string_view logpath, const std::string_view fname, const bool clearLog = false, const ullint queuesize = AELOG_DEFAULT_QUEUE_SIZE) : 
		AELogger(std::string(logpath)+std::string(fname), clearLog, queuesize) {
	}


	/// <summary>
	/// Class destructor.
	/// </summary>
	~AELogger();


	// we don't need the copy-constructor nor copy-assignment operators
	// Okay, okay, fine, we don't need it *for now* 
	// I'll implement multithreading and multiple instances later
	
	/// Deleted (for now) copy-constructor.
	AELogger(const AELogger&) = delete;
	/// Deleted (for now) copy-assignment.
	AELogger& operator=(const AELogger&) = delete;


//util functions(threads, files)
	/// <summary>
	/// Starts the log-writing thread.
	/// </summary>
	void startWriter(void);

	/// <summary>
	/// Stops the log-writing thread (after flushing the log queue).
	/// </summary>
	void stopWriter(void);

	/// <summary>
	/// Open the file to start logging.
	/// </summary>
	/// <param name="fname">Name of the log file</param>
	/// <param name="clearLog">Flag to clear the log file if it exists instead of appending it</param>
	inline void openLog(const std::string_view fname, const bool clearLog = false) {
		if (this->m_fwLogger.openFile(fname, !clearLog * AEFW_FLAG_APPEND, 1)) {
			this->writeToLog("Opened the log session in the file: \"" + std::string(fname) + '\"', AELOG_TYPE_OK, this->m_sModulename);
			this->startWriter();
		}
		
	}

	/// <summary>
	/// Close the file, if it was opened. That's it.
	/// </summary>
	inline void closeLog(void) {
		this->writeToLog("Closing the log session in the file: \"" + this->m_fwLogger.getFullFileName() + '\"', AELOG_TYPE_OK, this->m_sModulename);
		this->stopWriter();
		this->m_fwLogger.closeFile();
	}


//main utility function (bruh)
	/// <summary>
	/// Request a log entry to be written to the opened log file.
	/// @note See AELOG_TYPE_* flags
	/// </summary>
	/// <param name="logmessg">The message of the requested log entry</param>
	/// <param name="logtype">The type of the log entry</param>
	/// <param name="logmodule">The name of the module that invoked this request</param>
	void writeToLog(const std::string_view logmessg, const cint logtype = AELOG_TYPE_INFO, const std::string_view logmodule = AELOG_DEFAULT_MODULE_NAME);

	/// <summary>
	/// Request a debug log entry to be written to the opened log file.
	/// @note See AELOG_TYPE_* flags
	/// @note If ENGINE_DEBUG flag is not set, doesn't do anything
	/// @see AELogger::writeToLog()
	/// </summary>
	/// <param name="logmessg">The message of the requested log entry</param>
	/// <param name="logtype">The type of the log entry</param>
	/// <param name="logmodule">The name of the module that invoked this request</param>
	inline void writeToLogDebug(const std::string_view logmessg, const cint logtype = AELOG_TYPE_DEBUG, const std::string_view logmodule = AELOG_DEFAULT_MODULE_NAME) {
#ifdef ENGINE_DEBUG
		this->writeToLog("DEBUG->" + std::string(logmessg), logtype, logmodule);
#endif // ENGINE_DEBUG
	}

//getters of info
	/// <summary>
	/// Get the name of the log file.
	/// </summary>
	/// <returns>std::string of the name of opened log file</returns>
	inline std::string getLogName(void) const noexcept {
		return this->m_fwLogger.getFullFileName();
	}

	/// <summary>
	/// Get the path of the log file.
	/// </summary>
	/// <returns>std::string of the path of the opened log file</returns>
	inline std::string getLogPath(void) const noexcept {
		return this->m_fwLogger.getRelativePath();
	}

	/// <summary>
	/// Get the full/absolute path of the log file.
	/// </summary>
	/// <returns>std::string of the absolute path of the opened log file</returns>
	inline std::string getLogAbsolutePath(void) const noexcept {
		return this->m_fwLogger.getFullPath();
	}

	/// <summary>
	/// Get the amount of log entries done to an opened log file.
	/// </summary>
	/// <returns>ullint of the amount of times logger written to a file</returns>
	inline ullint getEntryCount(void) const noexcept {
		return this->m_ullLogOrderNum.load();
	}

	/// <summary>
	/// Get the last error status code.
	/// </summary>
	/// <returns>ucint of the error code by the filewriter/logger</returns>
	inline cint getLastError(void) const noexcept {
		return this->m_fwLogger.getLastError();
	}

	/// <summary>
	/// Checks if the current log file is open.
	/// </summary>
	/// <returns>True if the file is open for writing, false otherwise</returns>
	inline bool isOpen(void) const noexcept {
		return this->m_fwLogger.isOpen();
	}

	/// <summary>
	/// Checks if the log-writing thread is running.
	/// </summary>
	/// <returns>True if it is working(was launched), false otherwise</returns>
	inline bool isWriting(void) const noexcept {
		return this->m_bRunTrd;
	}
	
	/// <summary>
	/// Writes the current status of the file logger instance (file name, and entries written).
	/// </summary>
	inline void writeStatus(void) {
		this->writeToLog("AELogger's status: log file: \"" + this->m_fwLogger.getFullFileName() + "\"; entries written: " + std::to_string(this->getEntryCount()) + "(+1)",
			AELOG_TYPE_INFO, m_sModulename);
	}

	static inline std::string genLogName(const std::string_view logpath, const std::string_view logpref, const std::string_view logext = ".log") {
		char logname[256]{};
		snprintf(logname, 255, "%s%s_%s%s", logpath.data(), logpref.data(), (ace::utils::getCurrentDate().substr(0, 10)).c_str(), logext.data());
		return logname;
	}

private:
//functions
	/// <summary>
	/// Checks for the given index number and returns the pointer in the queue of log entries.
	/// @note The index is wrapped around the max queue size.
	/// </summary>
	/// <param name="num">The index number of the log entry</param>
	/// <returns>Pointer to the node of that index</returns>
	AELogEntry* ptrFromIndex(ullint num) noexcept;

	/// <summary>
	/// The function of the log writing thread to...read the entries, format them, write them, and clear them.
	/// </summary>
	void logWriterThread(void);

//variables
	/// The file writer to actually write text to opened log file
	AEFileWriter m_fwLogger;
	/// The mutex to lock when allocating new queue chunks
	std::mutex m_mtxAllocLock;
	/// The vector to all the allocated queue pieces and total amounts allocated
	std::vector<std::pair<ullint, AELogEntry*>> m_vAllocTable;
	/// The thread object for the file writing thread to...write a log file separately
	std::thread m_trdWriter;
	/// The order number of the current node;
	std::atomic<ullint> m_ullLogOrderNum;
	/// The amount of nodes filled (should we allocate more?)
	std::atomic<ullint> m_ullFilledCount;
	/// The current node number the writeToLog is working with
	std::atomic<ullint> m_ullNodeNumber;
	/// The node order number for the writing thread.
	ullint m_ullWriterOrderNum;
	/// The amount of entry spaces/size of the queue
	ullint m_ullQueueSize;
	/// The pointer to the first item in the queue
	AELogEntry* m_lepQueue;
	/// The pointer to the last item in the queue
	AELogEntry* m_lepLastNode;
	/// The flag to run the log-writing thread
	std::atomic<bool> m_bRunTrd;

	// And we don't need to keep the separate variables for the name of the file
	// and amount of log entries we did, etc.
	// All because we: 1) already have it through AEFileWriter
	// And 2) we'll write to file only once, after formatting the strings
};

//aaand we have to register it too
REGISTER_CLASS(AELogger);


#ifdef ENGINE_ENABLE_GLOBAL_MODULES

/// The global logger of the engine to log engine-wide events.
/// It starts the writing thread and logging as soon as the program starts.
inline AELogger globalLogger(AELogger::genLogName(AELOG_DEFAULT_LOG_PATH, "LOG", ".log"));

#endif // ENGINE_ENABLE_GLOBAL_MODULES


#endif // !ENGINE_AELOGGER_HPP
