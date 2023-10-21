
/** @file AELogger/src/AELogger.cpp
 *  This file contains the definitions of the engine's logger's functions.
 *  @see AELogger
 *  @see AELogger.hpp
 *
 *  Should not cause everything to break.
 */

#include "../include/AELogger.hpp"
#include "include/AEUtils.hpp"
#include "include/AEFlags.hpp"
#include <utility>
#include <ctime>


AELogEntry* AELogger::makeQueue(const std::size_t amt, AELogEntry* oldqueue) {
	//damn, amt is really 0; get null!
	if (amt == 0) {
		throw std::runtime_error("queue size to allocate is 0!");
	}
	//allocate new log entry list
	AELogEntry* leptr = new AELogEntry[amt]{};
	for (std::size_t i = 0; i < amt; i++) {
		leptr[i].m_pNextNode = leptr + i + 1; //set the next node pointers for our linked list
	}
	//loop the last log entry to the beginning
	if (oldqueue) {
		leptr[amt - 1].m_pNextNode = oldqueue; //beginning of old queue
	}
	else {
		leptr[amt - 1].m_pNextNode = leptr; //beginning of itself
	}

	return leptr;
}


//constructor
AELogger::AELogger(const std::string_view fname, const bool clearLog, const ullint queuesize) :
	m_fwLogger(fname, !clearLog * AEFW_FLAG_APPEND /* Funny magic with bool-int conversion */), 
	m_ullLogOrderNum(0), m_ullFilledCount(0), m_ullNodeNumber(0),
	m_ullQueueSize(queuesize), m_lepQueue(AELogger::makeQueue(queuesize, nullptr)), 
	m_lepLastNode(m_lepQueue + queuesize - 1), m_bRunTrd(false), m_bQueueFilled(false){


	//add the allocated queue pointed to the list
	//so we can free them later without memory leaks
	this->m_vAllocTable.reserve(AELOG_DEFAULT_ALLOC_VECTOR_RESERVE);
	this->m_vAllocTable.emplace_back(queuesize, this->m_lepQueue);

	if (this->m_fwLogger.isOpen()) {
		this->writeToLog("Created the AELogger instance and opened the log session in the file: \"" + std::filesystem::absolute(fname).generic_string() + '\"', AELOG_TYPE_OK, this->m_sModulename);
		this->startWriter();
	}
	
}

//destructor
AELogger::~AELogger() {
	this->writeToLog("Destroying the AELogger instance and closing", AELOG_TYPE_INFO, this->m_sModulename);
	this->closeLog();

	for (std::size_t i = 0; i < this->m_vAllocTable.size(); i++) {
		delete[] this->m_vAllocTable[i].second;
	}
	this->m_vAllocTable.clear();
}


cint AELogger::startWriter(void) {
	this->writeToLog("Attempting to start the log-writing thread", AELOG_TYPE_INFO, this->m_sModulename);
	if (this->m_trdWriter.joinable()) {
		this->writeToLog("The log-writing thread is already running!", AELOG_TYPE_WARN, this->m_sModulename);
		return AELOG_ERR_THREAD_ALREADY_RUNNING; //we already are writing, dummy;
	}

	this->m_bRunTrd = true;
	this->m_trdWriter = std::thread(&AELogger::logWriterThread, this);
	if (!this->m_trdWriter.joinable()) {
		this->writeToLog("Could not start AELogger thread!! (exception)", AELOG_TYPE_FATAL_ERROR, this->m_sModulename);
		this->closeLog();
		throw std::runtime_error("Could not start AELogger thread!");
		return AELOG_ERR_UNABLE_START_THREAD;
	}
	return AELOG_ERR_NOERROR;
}


cint AELogger::stopWriter(void) {

	this->writeToLog("Attempting to stop the log-writing thread...", AELOG_TYPE_INFO, this->m_sModulename);
	this->m_bRunTrd = false;
	if (this->m_trdWriter.joinable()) {
		this->m_trdWriter.join();
		return AELOG_ERR_NOERROR;
	}
	return AELOG_ERR_THREAD_ALREADY_STOPPED;
}

// request a log entry and write to it
void AELogger::writeToLog(const std::string_view logmessg, const cint logtype, const std::string_view logmodule) {
	/// @todo REWRITE THE WAYS OF LOGGING!
	if (this->isClosed()) {
		return; // file's closed/closing!
	}

	
	/// @todo Implement decrease in log queue size...somehow

	// check for invalid arguments
	if (logmessg.empty() || logmodule.empty() || !ace::utils::isInRange<cint>(AELOG_TYPE_DEBUG, AELOG_TYPE_FATAL_ERROR, logtype) || //empty/invalid stuff
		!ace::utils::isAlNumUs(logmodule)) { //log module stuff
		return;
	}

	this->m_ullFilledCount++;
	

	if (this->m_ullFilledCount > this->m_ullQueueSize) {
		const std::lock_guard<std::mutex> lock(this->m_mtxAllocLock);
		const ullint qsize = this->m_ullQueueSize / 2;

		//make new queue
		AELogEntry* newQueue = AELogger::makeQueue(qsize, this->m_lepQueue);
		//add the new queue to the vector
		this->m_ullQueueSize += qsize;
		this->m_lepLastNode->m_pNextNode = newQueue;
		this->m_lepLastNode = newQueue + qsize - 1;
		this->m_vAllocTable.emplace_back(this->m_ullQueueSize, newQueue);
		this->writeToLogDebug("The queue was too small, resized it to " + std::to_string(this->m_ullQueueSize) + " entries", this->m_sModulename);		
	}

	
	//implementation: the allocation vector!
	//instead of it having only the pointers to the allocated memory chunks of the log entries
	//it would have the std::pair instead: pointer to chunk and the total amount of entries
	//ex: {0xDEAD, 1024}, {0xBEEF, 1096} -> first pointer holds 1024 total entries, second holds 1096-1024=62 entries
	//and instead of changing an atomic pointer, we'll increment a number, "current node number"
	//and then check if the number(modulo of it with the m_ullQueueSize) is less than the 1st allocation in the vector, then 2nd, etc..untill we find a match
	//then, to get the pointer to current node -> pointer of that allocation + number


	//increment node number and get the pointer
	AELogEntry* ptr = this->ptrFromIndex(m_ullNodeNumber++);
	while (ptr->m_ullOrderNum != AELOG_ENTRY_INVALID_ORDERNUM && ptr->m_cStatus != AELOG_ENTRY_STATUS_INVALID) {
		ptr = this->ptrFromIndex(m_ullNodeNumber++); //if current node is filled -> continue looking for unpopulated one
	}

	
	//populating it now!
	ptr->m_ullOrderNum = this->m_ullLogOrderNum++;
	ptr->m_cStatus = AELOG_ENTRY_STATUS_SETTING; //alright boys, we're setting this one up

	ptr->m_tmLogTime = std::time(NULL);
	memcpy(ptr->m_sLogMessage, logmessg.data(), (logmessg.size() > AELOG_ENTRY_MESSAGE_SIZE) ? AELOG_ENTRY_MESSAGE_SIZE : logmessg.size());
	memcpy(ptr->m_sModuleName, logmodule.data(), (logmodule.size() > AELOG_ENTRY_MODULENAME_SIZE) ? AELOG_ENTRY_MODULENAME_SIZE : logmodule.size());
	ptr->m_cLogType = logtype;
	ptr->m_cStatus = AELOG_ENTRY_STATUS_READY;

}

void AELogger::logWriterThread(void) {
	this->writeToLog("Successfully launched the log-writing thread!", AELOG_TYPE_SUCCESS, this->m_sModulename);

	AELogEntry* ePtr = this->m_lepQueue;
	//string that stores the date and time formatted string
	char timestr[20]{};
	//the final message to output
	char str[AELOG_ENTRY_MAX_SIZE]{};
	// The node order number for the writing thread.
	ullint m_ullWriterOrderNum = 0;
	constexpr const char* const strformatDebug = "[%s] [%-14s] [%s]: DEBUG->%s\n";

	if (this->m_ullFilledCount.load(std::memory_order::memory_order_relaxed)) {
		goto queueFilledLabel;
	}

	//and not stop untill it's done
	//untill we written everything *and* we stopped the thread
	while (this->m_bRunTrd.load(std::memory_order::relaxed)) {
		queueFilledLabel:
		while (this->m_ullFilledCount.load(std::memory_order::memory_order_relaxed)) {
			if (ePtr->m_ullOrderNum <= m_ullWriterOrderNum) {

				//got it!. Now wait untill it's ready
				while (ePtr->m_cStatus != AELOG_ENTRY_STATUS_READY) {
					ace::utils::sleepUS(100);
				}
				//the entry is minee!
				ePtr->m_cStatus = AELOG_ENTRY_STATUS_READING;

				//formatting and writing
				
				
// 				if (ePtr->m_cLogType == AELOG_TYPE_DEBUG) {
// 					snprintf(str, sizeof(str), strformatDebug, timestr, AELogger::typeToString(ePtr->m_cLogType), ePtr->m_sModuleName, ePtr->m_sLogMessage);
// 				}
// 				else {
// 					snprintf(str, sizeof(str), strformat, timestr, AELogger::typeToString(ePtr->m_cLogType), ePtr->m_sModuleName, ePtr->m_sLogMessage);
// 				}
				ace::utils::formatDate(ePtr->m_tmLogTime, timestr);
				snprintf(str, sizeof(str), "[%s] [%-14s] [%s]: %s\n", timestr, AELogger::typeToString(ePtr->m_cLogType), ePtr->m_sModuleName, ePtr->m_sLogMessage);
				//std::cout << str;

				this->m_fwLogger.writeData_ptr(str, std::strlen(str), 1, false);
				this->m_fwLogger.flushFile();


				//cleanup
				AELogEntry::clearEntry(ePtr);
				this->m_ullFilledCount--;
				m_ullWriterOrderNum++;
				ePtr = ePtr->m_pNextNode;
			}
			else
			{
				//next node pls
				if (ePtr->m_ullOrderNum != AELOG_ENTRY_INVALID_ORDERNUM) {
					ePtr = ePtr->m_pNextNode;
				}
			}
			
		}

		//since we're done writing the entries, sleep and check if some appear again
		ace::utils::sleepUS(1000);
	}



	snprintf(str, sizeof(str), "[%s] [%-14s] [%s]: %s\n", ace::utils::getCurrentDate().c_str(), AELogger::typeToString(AELOG_TYPE_SUCCESS), this->m_sModulename.data(), "Successfully exited the writer thread.");
	this->m_fwLogger.writeData_ptr(str, 1, std::strlen(str), true);
}


AELogEntry* AELogger::ptrFromIndex(ullint num) noexcept {
	num %= m_ullQueueSize;
	ullint prevSize = 0;
	for (std::size_t i = 0; i < this->m_vAllocTable.size(); i++) {
		if (this->m_vAllocTable[i].first > num) {
			return this->m_vAllocTable[i].second + (num - prevSize);
		}
		prevSize = this->m_vAllocTable[i].first;
	}
	return m_lepQueue;
}


void AELogger::writeToLog2(const std::string_view logmessg, const cint logtype, const std::string_view logmodule) {





}


