
/*! @file AELogger/include/AELogger.hpp
 *  This file contains the code for the engine's logger module, err...for logging data to file.
 *
 *  Should not cause everything to break :)
 */

#pragma once

#ifndef ENGINE_AELOGGER_HPP
#define ENGINE_AELOGGER_HPP

#include "include/AEFileWriter.hpp"
#include "include/AELogEntry.hpp"
#include <atomic>
#include <mutex>
#include <string_view>
#include <thread>
#include <vector>


// queue decrease algorithm
// since we have the access to the maximum queue value, and it is being checked in the
// ptrFromIndex(), we have a way to restrict the access to certain nodes for the writeToLog()
// All by changing the max queue value to the value of the second-to-last node in the allocation vector
// That way, when we wrap the index around next time, it wraps around the nodes that we want to keep
// And completely ignores the last nodes we want to delete.
// Though how to delete information from those delete-me nodes, since the writer-thread is the only thing
// That still has access to those nodes. And we don't want to delete the unfilled ones



//Error flags
/// Macro for the indicator that everything is good/no error was encountered in the process
#define AELOG_ERR_NOERROR ENGINE_MODULE_ERR_NOERROR

/// Macro for the error that's raised when the log-writing-thread cannot be started
#define AELOG_ERR_UNABLE_START_THREAD -11

/// Macro for the error that's raised when the log-writing-thread is attempted to be started again, while it's already running
#define AELOG_ERR_THREAD_ALREADY_RUNNING -12

/// Macro for the error that's raised when the log-writing-thread is attempted to be stopped again, while it was already stopped
#define AELOG_ERR_THREAD_ALREADY_STOPPED -13

/// Macro for the error that's raised when invalid data is passed to AELogger::writeToLog() (like empty message, empty/non-alphanumeric module name, type outside of range)
#define AELOG_ERR_INVALID_ENTRY_DATA -14



/// <summary>
/// @brief The ArtyK's Engine's Logger module -- it manages the writing to the log files.
/// A wrapper around AEFW functionality for writing, and AELogEntry for mass-formatting of entries to text
///
/// The log is written by requesting and filling the entry in the queue.
/// The AELogger instance launches the separate thread that looks at the entries in the queue,
/// retrieves and formats the data in them, and writes it to the file. Afterwards that entry in the queue is cleared.
///
/// Hungarian notation is lg. (m_lgMyLogger)
/// </summary>
/// @todo Implement copy constructors and copy assignment
/// @todo Add the ability to open the same log file/redirect the instance requests to the one that has it open first.
/// @bug The queue can expand if it's too little. But....I don't know how to shrink it. (working on it)
class AELogger : public __AEModuleBase<AELogger> {

public:

//constructors
	/// <summary>
	/// Class constructor -- it opens the file with the given name  and starts logging to it.
	/// @note Puts the file into the default log path location (AELOG_DEFAULT_LOG_PATH)
	/// </summary>
	/// <param name="fname">Name of the log file</param>
	/// <param name="clearLog">Flag to clear the log file if it exists instead of appending it</param>
	/// <param name="queuesize">The size of the queue to create when creating AELogger instance</param>
	explicit AELogger(const std::string fname, const bool clearLog = false, const ullint queuesize = AELOG_DEFAULT_QUEUE_SIZE) :
		AELogger(AELOG_DEFAULT_LOG_PATH, fname, clearLog, queuesize) {}

	/// <summary>
	/// Class constructor -- it opens the file with the given path and name and start logging to it.
	/// </summary>
	/// <param name="logpath">The path of the log file to open it in</param>
	/// <param name="fname">Name of the log file</param>
	/// <param name="clearLog">Flag to clear the log file if it exists instead of appending it</param>
	/// <param name="queuesize">The size of the queue to create when creating AELogger instance</param>
	explicit AELogger(const std::string logpath, const std::string fname, const bool clearLog = false, const ullint queuesize = AELOG_DEFAULT_QUEUE_SIZE);

	/// <summary>
	/// Default class constructor -- doesn't open the file, but setups the instance for one.
	/// </summary>
	explicit AELogger(void) : m_fwLogger(), m_ullFilledCount(0), m_ullNodeNumber(0),
		m_ullQueueSize(AELOG_DEFAULT_QUEUE_SIZE), m_lepQueue(AELogEntry::makeQueue(AELOG_DEFAULT_QUEUE_SIZE, true, nullptr)),
		m_lepLastNode(m_lepQueue + AELOG_DEFAULT_QUEUE_SIZE - 1), m_bRunTrd(false), m_bQueueFilled(false) {

		this->m_vAllocTable.reserve(AELOG_DEFAULT_ALLOC_VECTOR_RESERVE);
		this->m_vAllocTable.emplace_back(m_ullQueueSize, this->m_lepQueue);
	}

	/// <summary>
	/// Class destructor.
	/// </summary>
	~AELogger(void);


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
	/// <returns>AELOG_ERR_NOERROR on success; otherwise AELOG_ERR_THREAD_ALREADY_RUNNING if thread already was running, AELOG_ERR_UNABLE_START_THREAD if error happened (+ std::runtime_error() exception)</returns>
	cint startWriter(void);

	/// <summary>
	/// Stops the log-writing thread (after flushing the log queue).
	/// </summary>
	/// <returns>AELOG_ERR_NOERROR on success; otherwise AELOG_ERR_THREAD_ALREADY_STOPPED if thread already was stopped</returns>
	cint stopWriter(void);

	/// <summary>
	/// Open the file to start logging.
	/// </summary>
	/// <param name="fpath">Path to put the log file in</param>
	/// <param name="fname">Name of the log file</param>
	/// <param name="clearLog">Flag to clear the log file if it exists instead of appending it</param>
	/// <returns>AELOG_ERR_NOERROR on success; otherwise return values of AEFileWriter::openFile() or AELogger::startWriter()</returns>
	inline cint openLog(const std::string fpath, const std::string fname, const bool clearLog = false) {
		const cint ret = this->m_fwLogger.openFile(fpath + fname, !clearLog * AEFW_FLAG_APPEND);
		if (ret != AEFW_ERR_NOERROR) {
			return ret;
		}
		this->writeToLog("Opened the log session in the file: \"" + std::string(fname) + '\"', AELOG_TYPE_SUCCESS, this->getModuleName());
		return this->startWriter();
	}

	/// <summary>
	/// Open the file to start logging.
	/// @note Puts the file into the default log path location (AELOG_DEFAULT_LOG_PATH)
	/// </summary>
	/// <param name="fname">Name of the log file</param>
	/// <param name="clearLog">Flag to clear the log file if it exists instead of appending it</param>
	/// <returns>AELOG_ERR_NOERROR on success; otherwise return values of AEFileWriter::openFile() or AELogger::startWriter()</returns>
	inline cint openLog(const std::string fname, const bool clearLog = false) {
		return this->openLog(AELOG_DEFAULT_LOG_PATH, fname, clearLog);
	}

	/// <summary>
	/// Open the file to start logging.
	/// </summary>
	/// <param name="fpath">Path of directory to put the log file in (include trailing '/' character)</param>
	/// <param name="fname">Name of the log file</param>
	/// <param name="clearLog">Flag to clear the log file if it exists instead of appending it</param>
	/// <returns>AELOG_ERR_NOERROR on success; otherwise return values of AEFileWriter::openFile() or AELogger::startWriter()</returns>
	inline cint openLog(const std::string_view fpath, const std::string_view fname, const bool clearLog = false) {
		return this->openLog(std::string(fpath) + std::string(fname), clearLog);
	}

	/// <summary>
	/// Close the file, if it was opened. That's it.
	/// </summary>
	/// <returns>AELOG_ERR_NOERROR on success; otherwise AEFW_ERR_FILE_NOT_OPEN if file isn't open, </returns>
	inline cint closeLog(void) {
		if (this->isClosed()) {
			return AEFW_ERR_FILE_NOT_OPEN;
		}
		this->writeToLog("Closing the log session in the file: \"" + this->m_fwLogger.getFullFileName() + '\"', AELOG_TYPE_INFO, this->getModuleName());
		this->stopWriter();

		this->writeToLogDirectly("Successfully closed the log session in the file: \"" + this->m_fwLogger.getFullFileName() + '\"', AELOG_TYPE_SUCCESS, this->getModuleName());
		this->m_fwLogger.closeFile();
		return AELOG_ERR_NOERROR;
	}


//main utility function (bruh)
	/// <summary>
	/// Request a log entry to be written to the opened log file.
	/// @note See AELOG_TYPE_* flags
	/// @note The module name should contain only alphanumeric characters or underscores (no spaces), otherwise it fails
	/// </summary>
	/// <param name="logmessg">The message of the requested log entry</param>
	/// <param name="logtype">The type of the log entry</param>
	/// <param name="logmodule">The name of the module that invoked this request</param>
	/// <param name="logTime">The custom time of the log entry to insert, *if you really need that*!</param>
	/// <returns>AELOG_ERR_NOERROR on success; otherwise AEFW_ERR_FILE_NOT_OPEN if log file isn't open, AELOG_ERR_INVALID_ENTRY_DATA if passed data isn't of proper format</returns>
	cint writeToLog(const std::string_view logmessg, const cint logtype = AELOG_TYPE_INFO, const std::string_view logmodule = AELOG_DEFAULT_MODULE_NAME, const std::time_t logTime = 0);

	/// <summary>
	/// Request a log entry to be written to the opened log file, using data from a log entry type.
	/// </summary>
	/// <param name="entry">The log entry to write to the file</param>
	/// <param name="useCurrentTime">Flag to use current time for the log entry, or use timestamp in the provided entry</param>
	/// <returns>AELOG_ERR_NOERROR on success; otherwise AEFW_ERR_FILE_NOT_OPEN if log file isn't open</returns>
	inline cint writeToLog(const AELogEntry& entry, const bool useCurrentTime = true) {
		this->writeToLog(entry.m_sLogMessage, entry.m_cLogType, entry.m_sModuleName, ((useCurrentTime) ? 0 : entry.m_tmLogTime ));
	}

	/// <summary>
	/// Request a debug log entry to be written to the opened log file.
	/// @note See AELOG_TYPE_* flags
	/// @note If ENGINE_DEBUG flag is not set, doesn't do anything
	/// @see AELogger::writeToLog()
	/// </summary>
	/// <param name="logmessg">The message of the requested log entry</param>
	/// <param name="logmodule">The name of the module that invoked this request</param>
	/// <returns>AELOG_ERR_NOERROR on success; otherwise AEFW_ERR_FILE_NOT_OPEN if log file isn't open, AELOG_ERR_INVALID_ENTRY_DATA if passed data isn't of proper format</returns>
	inline cint writeToLogDebug(const std::string_view logmessg, const std::string_view logmodule = AELOG_DEFAULT_MODULE_NAME) {
#if ENGINE_DEBUG
		// the code for formatting the log entry (adding that DEBUG-> thing) will be executed in the logWriterThread()
		return this->writeToLog(logmessg, AELOG_TYPE_DEBUG, logmodule);
#endif // ENGINE_DEBUG
	}

	/// <summary>
	/// Writes the current status of the file logger instance (file name, and entries written).
	/// </summary>
	/// <returns>AELOG_ERR_NOERROR on success; otherwise AEFW_ERR_FILE_NOT_OPEN if log file isn't open, AELOG_ERR_INVALID_ENTRY_DATA if passed data isn't of proper format</returns>
	inline cint writeStatus(void) {
		return this->writeToLog("AELogger's status: log file: \"" + this->m_fwLogger.getFullFileName() + "\"; entries written: " + std::to_string(this->getEntryCount()) + "(+1)",
			AELOG_TYPE_INFO, this->getModuleName());
	}

//getters of info
	/// <summary>
	/// Get the name of the log file.
	/// </summary>
	/// <returns>std::string of the name of opened log file; otherwise values from AEFileWriter::getFullFileName()</returns>
	inline std::string getLogName(void) const noexcept {
		return this->m_fwLogger.getFullFileName();
	}

	/// <summary>
	/// Get the path of the log file.
	/// </summary>
	/// <returns>std::string of the path of the opened log file; otherwise values from AEFileWriter::getRelativePath()</returns>
	inline std::string getLogPath(void) const {
		return this->m_fwLogger.getRelativePath();
	}

	/// <summary>
	/// Get the full/absolute path of the log file.
	/// </summary>
	/// <returns>std::string of the absolute path of the opened log file; otherwise values from AEFileWriter::getFullPath()</returns>
	inline std::string getLogAbsolutePath(void) const {
		return this->m_fwLogger.getFullPath();
	}

	/// <summary>
	/// Get the amount of log entries done to an opened log file.
	/// </summary>
	/// <returns>ullint of the amount of times logger written to a file</returns>
	inline ullint getEntryCount(void) const noexcept {
		return this->m_fwLogger.getTotalWrites();
	}

	/// <summary>
	/// Checks if the current log file is open.
	/// </summary>
	/// <returns>True if the file is open for writing, false otherwise</returns>
	inline bool isOpen(void) const noexcept {
		return this->m_fwLogger.isOpen();
	}

	/// <summary>
	/// Checks if a log file isn't open by this logger.
	/// </summary>
	/// <returns>True if log file is closed/not open, false if otherwise</returns>
	inline bool isClosed(void) const noexcept {
		return this->m_fwLogger.isClosed();
	}

	/// <summary>
	/// Checks if the log-writing thread is running.
	/// </summary>
	/// <returns>True if it is working(was launched), false otherwise</returns>
	inline bool isWriting(void) const noexcept {
		return this->m_bRunTrd;
	}

	/// <summary>
	/// Creates a full name of the log file to open to be fed to the logger, in a format [prefix]_[current date][.extension] in a given directory
	/// </summary>
	/// <param name="logpref">The prefix of log file</param>
	/// <param name="logext">The extension of the log file. Include the period before the extension.</param>
	/// <returns>std::string of the file name to feed to the logger for opening</returns>
	static inline std::string genLogName(const std::string_view logpref, const std::string_view logext = ".log") {
		char logname[256]{};
		snprintf(logname, 255, "%s_%s%s", logpref.data(), (ace::utils::getCurrentDate().substr(0, 10)).c_str(), logext.data());
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
	/// Writes to log directly, without thinking of the queue
	/// @warning Use it with caution, when you sure that it won't compromise the log integrity (you know, race conditions with fwrite() in the AELogger::logWriterThread())
	/// </summary>
	/// <param name="logmessg">Message of the log</param>
	/// <param name="logtype">the type/severity of the log</param>
	/// <param name="logmodule">the module name that requested the log</param>
	inline void writeToLogDirectly(const std::string_view logmessg, const cint logtype = AELOG_TYPE_INFO, const std::string_view logmodule = AELOG_DEFAULT_MODULE_NAME) noexcept {
		AELogEntry lEntry{
			"", // message will be written to later
			"", // modulename will be written to later
			std::time(nullptr), // m_tmLogTime
			nullptr, // m_pNextNode
			logtype, // m_cLogType
			//AELE_STATUS_INVALID, // m_cStatus; isn't needed since the function doesn't check it anyway
		};

		std::memcpy(lEntry.m_sLogMessage, logmessg.data(), (logmessg.size() < AELE_MESSAGE_SIZE)? logmessg.size() : AELE_MESSAGE_SIZE);
		std::memcpy(lEntry.m_sModuleName, logmodule.data(), (logmodule.size() < AELE_MODULENAME_SIZE) ? logmodule.size() : AELE_MODULENAME_SIZE);
		char str[AELE_FORMAT_MAX_SIZE]{};
		//AELogEntry::formatEntry(str, lEntry);
		this->m_fwLogger.writeData_ptr(str, AELogEntry::formatEntry(str, lEntry), 1, false);
		this->m_fwLogger.flushFile();

	}

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
	/// The amount of nodes filled (should we allocate more?)
	std::atomic<ullint> m_ullFilledCount;
	/// The current node number the writeToLog is working with
	std::atomic<ullint> m_ullNodeNumber;
	/// The amount of entry spaces/size of the queue
	ullint m_ullQueueSize;
	/// The pointer to the first item in the queue
	AELogEntry* m_lepQueue;
	/// The pointer to the last item in the queue
	AELogEntry* m_lepLastNode;

	/// The flag to run the log-writing thread
	std::atomic<bool> m_bRunTrd;
	/// The flag to show that the queue is populated
	std::atomic<bool> m_bQueueFilled;

	// And we don't need to keep the separate variables for the name of the file
	// and amount of log entries we did, etc.
	// All because we: 1) already have it through AEFileWriter
	// And 2) we'll write to file only once, after formatting the strings
	
	//aaand we have to register it too
	REGISTER_MODULE(AELogger);
};


#if ENGINE_ENABLE_GLOBAL_MODULES

/// The global logger of the engine to log engine-wide events.
/// It starts the writing thread and logging as soon as the program starts.
inline AELogger globalLogger(AELogger::genLogName("LOG", ".log"));

#endif // ENGINE_ENABLE_GLOBAL_MODULES


#endif // !ENGINE_AELOGGER_HPP
