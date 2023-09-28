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
#include <iostream>
#include <utility>


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

#define AELOG_DEFAULT_QUEUE_SIZE 1024

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
	cint m_cLogType = AELOG_TYPE_DEBUG;
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
	
	static inline AELogEntry* makeQueue(const std::size_t amt, AELogEntry* oldqueue = nullptr) {
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
	AELogger(const std::string& fname, const bool clearLog = false, const ullint queuesize = AELOG_DEFAULT_QUEUE_SIZE) :
		m_fwLogger(fname, !clearLog * AEFW_FLAG_APPEND /* Funny magic with bool-int conversion */, 1), m_ullLogOrderNum(0), m_ullFilledCount(0), m_ullNodeNumber(0), m_lepQueue(AELogEntry::makeQueue(queuesize)),
		m_lepLastNode(m_lepQueue + queuesize - 1), m_ullQueueSize(queuesize), m_bStopTrd(false) {


		//add the allocated queue pointed to the list
		//so we can free them later without memory leaks
		this->m_vAllocTable.reserve(16);
		this->m_vAllocTable.push_back({queuesize, m_lepQueue });
		this->startWriter();
	}

	/// <summary>
	/// Class destructor
	/// </summary>
	~AELogger() {
		this->stopWriter();

		for (std::size_t i = 0; i < this->m_vAllocTable.size(); i++) {
			delete[] this->m_vAllocTable[i].second;
		}

		this->m_fwLogger.closeFile();
		this->m_vAllocTable.clear();
	}

	// we don't need the copy-constructor nor copy-assignment operators
	// Okay, okay, fine, we don't need it *for now* 
	// I'll implement multithreading and multiple instances later
	AELogger(const AELogger&) = delete;
	AELogger& operator=(const AELogger&) = delete;
	

	inline void startWriter(void) {
		if (this->m_trdWriter.joinable()) {
			return; //we already are writing, dummy;
		}

		this->m_bStopTrd = false;
		this->m_trdWriter = std::thread(&AELogger::parseEntries, this);
		if (!this->m_trdWriter.joinable()) {
			throw std::runtime_error("Could not start AETimer thread!");
		}
	}

	inline void stopWriter(void) {
		this->m_bStopTrd = true;
		if (this->m_trdWriter.joinable()) {
			this->m_trdWriter.join();
		}
	}

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
	inline cint getLastError(void) const {
		return this->m_fwLogger.getLastError();
	}

	void writeToLog(const std::string& logmessg, const ucint logtype = AELOG_TYPE_INFO, const std::string& logmodule = AELOG_DEFAULT_MODULE_NAME) {
		if (this->m_ullFilledCount <= this->m_ullQueueSize) {
			if (logmessg.empty()) {
				return; //na'ah, no empty messages
			}
			this->m_ullFilledCount.fetch_add(1);

			//implementation: the allocation vector!
			//instead of it having only the pointers to the allocated memory chunks of the log entries
			//it would have the std::pair instead: pointer to chunk and the total amount of entries
			//ex: {0xDEAD, 1024}, {0xBEEF, 1096} -> first pointer holds 1024 total entries, second holds 1096-1024=62 entries
			//and instead of changing an atomic pointer, we'll increment a number, "current node number"
			//and then check if the number(modulo of it with the m_ullQueueSize) is less than the 1st allocation in the vector, then 2nd, etc..untill we find a match
			//then, to get the pointer to current node -> pointer of that allocation + number


			//increment node number and get the pointer
			AELogEntry* ptr = this->ptrFromIndex(m_ullNodeNumber++);
			while (ptr->m_ullOrderNum != AELOG_ENTRY_INVALID_ORDERNUM && ptr->m_bStatus != AELOG_ENTRY_STATUS_INVALID) {
				ptr = ptr->m_lepNextNode; //if current node is filled -> continue looking for unpopulated one
			}

			
			//populating it now!
			ptr->m_ullOrderNum = this->m_ullLogOrderNum.fetch_add(1);
			ptr->m_bStatus = AELOG_ENTRY_STATUS_SETTING; //alright boys, we're setting this one up

			ptr->m_tmLogTime = std::time(NULL);
			memcpy(ptr->m_sLogMessage, logmessg.c_str(), (logmessg.size() <= 511) ? logmessg.size() : 511);
			memcpy(ptr->m_sModuleName, logmodule.c_str(), (logmodule.size() <= 32) ? logmodule.size() : 31);
			ptr->m_cLogType = logtype;
			ptr->m_bStatus = AELOG_ENTRY_STATUS_READY;
		}
		else {
			throw std::runtime_error("AELog Queue is filled up!!!");
		}
	}

	void parseEntries(void) {
		
		AELogEntry* ePtr = this->m_lepQueue;
		ullint orderNum = 0;
		char str[588]{};
		//and not stop untill it's done

		while (!this->m_bStopTrd) {
			while (this->m_ullFilledCount) {
				if (ePtr->m_ullOrderNum == orderNum) {
					
					//got it!. Now wait untill it's ready
					while (ePtr->m_bStatus != AELOG_ENTRY_STATUS_READY) {
						ace::utils::sleepMS(30);
					}
					//the entry is minee!
					ePtr->m_bStatus = AELOG_ENTRY_STATUS_READING;

					//formatting and writing
					snprintf(str, 588, "[%s] [%s] [%s]: %s\n", ace::utils::formatDate(ePtr->m_tmLogTime).c_str(), AELogEntry::typeToString(ePtr->m_cLogType), ePtr->m_sModuleName, ePtr->m_sLogMessage);
					std::cout << str;

					//cleanup
					std::memset(str, NULL, 588); // clean the formatting buffer
					std::memset(ePtr->m_sLogMessage, NULL, 512); // clean log message
					std::memset(ePtr->m_sModuleName, NULL, 32); // clean module name
					ePtr->m_bStatus = AELOG_ENTRY_STATUS_INVALID;
					ePtr->m_ullOrderNum = AELOG_ENTRY_INVALID_ORDERNUM;
					this->m_ullFilledCount--;
					orderNum++;
				}
				//next node pls
				ePtr = ePtr->m_lepNextNode;
			}

			//since we're done writing the entries, sleep and check if some appear again
			ace::utils::sleepMS(100);
		}
	}

	AELogEntry* ptrFromIndex(ullint num) {
		num %= m_ullQueueSize;
		for (std::size_t i = 0; i < this->m_vAllocTable.size(); i++) {
			if (this->m_vAllocTable[i].first > num) {
				return this->m_vAllocTable[i].second + num;
			}
		}
		return m_lepQueue;
	}


private:

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
	/// The amount of entry spaces/size of the queue
	ullint m_ullQueueSize;
	/// The pointer to the first item in the queue
	AELogEntry* m_lepQueue;
	/// The pointer to the last item in the queue
	AELogEntry* m_lepLastNode;
	/// Flag to stop the writing thread
	std::atomic<bool> m_bStopTrd;

	// we don't need to keep the separate variables for the name of the file
	// and amount of log entries we did.
	// All because we: 1) already have it through AEFileWriter
	// And 2) we'll write to file only once, after formatting the strings
};

//aaand we have to register it too
REGISTER_CLASS(AELogger)


#endif // !ENGINE_AELOGGER_HPP



