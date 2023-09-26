#pragma once

#ifndef ENGINE_AELOGGER_HPP
#define ENGINE_AELOGGER_HPP

#include "include/AEModuleBase.hpp"
#include "include/AEFileWriter.hpp"
#include <ctime>
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>
#include "include/AEFrame.hpp"
#include <iostream>

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
#define AELOG_ENTRY_INVALID_ORDERNUM ullint(-1)

//log entry status

//log entry is invalid
#define AELOG_ENTRY_STATUS_INVALID 0
//log entry is currently being set up and written to
#define AELOG_ENTRY_STATUS_SETTING 1
//log entry is ready to be read
#define AELOG_ENTRY_STATUS_READY 2 
//log entry is currently being read by the thread
#define AELOG_ENTRY_STATUS_READING 3

#define AELOG_DEFAULT_QUEUE_SIZE 512

#define AELOG_DEFAULT_MODULE_NAME "ACEngine"

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
	/// The type of the log entry
	/// @see AELOG_TYPE_*
	ucint m_ucLogType = AELOG_TYPE_DEBUG;
	/// The status flag in the entry to show if the entry is ready, being read/set, or is invalid
	std::atomic<cint> m_bStatus = AELOG_ENTRY_STATUS_INVALID;
	
	/// <summary>
	/// Deduces the entry's log type and returns a c-string of it
	/// </summary>
	/// <param name="logtype">The value of the log type</param>
	/// <returns>c-string of the type</returns>
	static constexpr const char* typeToString(const cint logtype) {
		switch (logtype) {
		case AELOG_TYPE_INFO: return "INFO";
		case AELOG_TYPE_WARN: return "WARNING"; case AELOG_TYPE_SEVERE_WARN: return "SEVERE_WARNING";
		case AELOG_TYPE_OK: return "OK"; case AELOG_TYPE_SUCCESS: return "SUCCESS";
		case AELOG_TYPE_ERROR: return "ERROR"; case AELOG_TYPE_FATAL_ERROR: return "FATAL_ERROR";
		case AELOG_TYPE_DEBUG: /*same as the default / invalid value*/ default: return "DEBUG";
		}
	}
	
	static inline AELogEntry* makeQueue(const std::size_t amt, AELogEntry* const oldqueue = nullptr) {
		//damn, amt is really 0; get null!
		if (amt == 0) {
			return nullptr;
		}
		//allocate new log entry list
		AELogEntry* leptr = new AELogEntry[amt]{};
		for (std::size_t i = 0; i < amt; i++) {
			leptr[i].m_lepNextNode = leptr + i + 1; //set the next node pointers for our linked list
		}
		//loop the last log entry to the beginning
		if (oldqueue) {
			leptr[amt - 1].m_lepNextNode = oldqueue; //beginning of old queue
		}
		else {
			leptr[amt - 1].m_lepNextNode = leptr; //beginning of itself
		}

		return leptr;
	}
};



//TODO: implement copy constructors and copy assignment
//TODO: add threads to the AELogger


class AELogger : public __AEModuleBase<AELogger> {

public:

//constructors
	
	/// <summary>
	/// Class constructor. It opens the file to start the logging process
	/// </summary>
	/// <param name="fname">Name of the log file</param>
	/// <param name="clearLog">Flag to clear the log file if it exists instead of appending it</param>
	AELogger(const std::string& fname, const bool clearLog = false) : 
		m_fwLogger(fname, !clearLog * AEFW_FLAG_APPEND /* Funny magic with bool-int conversion */, 1),
		/*m_trdWriter definition,*/ m_ullOrderNum(0), m_lepQueue(AELogEntry::makeQueue(AELOG_DEFAULT_QUEUE_SIZE)), m_lepCurrentNode(m_lepQueue.load()),
		m_lepLastNode(m_lepQueue.load()+AELOG_DEFAULT_QUEUE_SIZE), m_ullFilledCount(0), m_ullQueueSize(AELOG_DEFAULT_QUEUE_SIZE), m_bStopTrd(false){
		
		

		//add the allocated queue pointed to the list
		//so we can free them later without memory leaks
		this->m_vAllocTable.reserve(16);
		this->m_vAllocTable.push_back(m_lepQueue);
	}

	~AELogger() {
		this->m_bStopTrd = false;
		if (this->m_trdWriter.joinable()) {
			this->m_trdWriter.join();
		}

		for (int i = 0; i < this->m_vAllocTable.size(); i++) {
			delete[] this->m_vAllocTable[i];
		}

		this->m_fwLogger.closeFile();
		this->m_vAllocTable.clear();
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
	inline void openLog(const std::string& fname, const bool clearLog = false) {
		this->m_fwLogger.open(fname, !clearLog * AEFW_FLAG_APPEND, 1);
	}

	/// <summary>
	/// Close the file, if it was opened. That's it.
	/// </summary>
	inline void closeLog(void) {
		this->m_fwLogger.closeFile();
	}

	/// <summary>
	/// Get the name of the log file
	/// </summary>
	/// <returns>std::string of the name of opened log file</returns>
	inline std::string getLogName(void) const {
		return this->m_fwLogger.getFileName();
	}
	
	/// <summary>
	/// Get the amount of log entries done to an opened log file
	/// </summary>
	/// <returns>ullint of the amount of times logger written to a file</returns>
	inline ullint getEntryCount(void) const {
		return this->m_fwLogger.getTotalWrites();
	}

	/// <summary>
	/// Get the last error status code
	/// </summary>
	/// <returns>ucint of the error code by the filewriter/logger</returns>
	inline ucint getLastError(void) const {
		return this->m_fwLogger.getLastError();
	}

	
	void writeToLog(const std::string& logmessg, const ucint logtype = AELOG_TYPE_INFO, const std::string& logmodule = AELOG_DEFAULT_MODULE_NAME) {
		if (this->m_ullFilledCount <= this->m_ullQueueSize) {
			if (logmessg.empty()) {
				return;
			}
			this->m_ullFilledCount++;
			AELogEntry* ptr = this->m_lepCurrentNode.load();	
			this->m_lepCurrentNode = this->m_lepCurrentNode.load()->m_lepNextNode;
			ptr->m_ullOrderNum = this->m_ullOrderNum++;
			ptr->m_bStatus = AELOG_ENTRY_STATUS_SETTING; //alright boys, we're setting this one up

			ptr->m_tmLogTime = std::time(NULL);
			memcpy(ptr->m_sLogMessage, logmessg.c_str(), (logmessg.size() <= 511) ? logmessg.size() : 511);
			memcpy(ptr->m_sModuleName, logmodule.c_str(), (logmodule.size() <= 32) ? logmodule.size() : 31);
			ptr->m_ucLogType = logtype;
			ptr->m_bStatus = AELOG_ENTRY_STATUS_READY;
			
		}
		else {
			throw std::runtime_error("AELog Queue is filled up!!!");
		}
	}

	void parseEntries(void) {
		
		AELogEntry* ePtr = this->m_lepQueue.load();
		ullint orderNum = 0;
		char str[588]{};
		AEFrame myfr(60);
		while (this->m_ullFilledCount) {
			
			if (ePtr->m_ullOrderNum == orderNum) {
				while (ePtr->m_bStatus != AELOG_ENTRY_STATUS_READY) {
					myfr.sleep();
				}
				ePtr->m_bStatus = AELOG_ENTRY_STATUS_READING;


				snprintf(str, 588, "[%s] [%s] [%s]: %s\n", ace::utils::formatDate(ePtr->m_tmLogTime).c_str(), AELogEntry::typeToString(ePtr->m_ucLogType), ePtr->m_sModuleName, ePtr->m_sLogMessage);
				

				std::cout << str;
				
				std::memset(str, NULL, 588); // clean the formatting buffer
				std::memset(ePtr->m_sLogMessage, NULL, 512); // clean log message
				std::memset(ePtr->m_sModuleName, NULL, 32); // clean module name
				ePtr->m_ullOrderNum = AELOG_ENTRY_INVALID_ORDERNUM;
				ePtr->m_bStatus = AELOG_ENTRY_STATUS_INVALID;
				this->m_ullFilledCount--;
				orderNum++;
			}

			ePtr = ePtr->m_lepNextNode;
			
		}


	}

private:

	/// The file writer to actually write text to opened log file
	AEFileWriter m_fwLogger;
	/// The mutex to lock when allocating new queue chunks
	std::mutex m_mtxAllocLock;
	/// The vector to all the allocated queue pieces
	std::vector<AELogEntry*> m_vAllocTable;
	std::thread m_trdWriter;
	std::atomic<ullint> m_ullOrderNum;
	std::atomic<ullint> m_ullFilledCount;
	std::atomic<AELogEntry*> m_lepQueue;
	std::atomic<AELogEntry*> m_lepCurrentNode;
	AELogEntry* m_lepLastNode;
	ullint m_ullQueueSize;
	std::atomic<bool> m_bStopTrd;

	// we don't need to keep the separate variables for the name of the file
	// and amount of log entries we did.
	// All because we: 1) already have it through AEFileWriter
	// And 2) we'll write to file only once, after formatting the strings
};

//aaand we have to register it too
REGISTER_CLASS(AELogger)


#endif // !ENGINE_AELOGGER_HPP



