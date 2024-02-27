
/*! @file AELogger/src/AELogger.cpp
 *  This file contains the definitions of the engine's logger's functions.
 *  @see AELogger
 *  @see AELogger.hpp
 *
 *  Should not cause everything to break :)
 */

#include "../include/AELogger.hpp"

//constructor
AELogger::AELogger(const std::string logpath, const std::string fname, const bool clearLog, const ullint queuesize) :
	m_fwLogger(logpath + fname, !clearLog * AEFW_FLAG_APPEND /* Funny magic with bool-int conversion */),
	m_ullFilledCount(0), m_ullNodeNumber(0), m_ullQueueSize(queuesize),
	m_lepQueue(AELogEntry::makeQueue(queuesize, true, nullptr)), m_lepLastNode(m_lepQueue + queuesize - 1),
	m_bRunTrd(false), m_bQueueFilled(false) {


	//add the allocated queue pointed to the list
	//so we can free them later without memory leaks
	this->m_vAllocTable.reserve(AELOG_DEFAULT_ALLOC_VECTOR_RESERVE);
	this->m_vAllocTable.emplace_back(queuesize, this->m_lepQueue);

	if (this->m_fwLogger.isOpen()) {
		this->writeToLog("Created the AELogger instance and opened the log session in the file: \"" + std::filesystem::absolute(fname).generic_string() + '\"', AELOG_TYPE_OK, this->getModuleName());
		this->startWriter();
	}

}

//destructor
AELogger::~AELogger(void) {
	this->writeToLog("Destroying the AELogger instance and closing", AELOG_TYPE_INFO, this->getModuleName());
	this->closeLog();

	for (std::size_t i = 0; i < this->m_vAllocTable.size(); i++) {
		delete[] this->m_vAllocTable[i].second;
	}
	this->m_vAllocTable.clear();
}


cint AELogger::startWriter(void) {
	this->writeToLog("Attempting to start the log-writing thread", AELOG_TYPE_INFO, this->getModuleName());
	if (this->m_trdWriter.joinable()) {
		this->writeToLog("The log-writing thread is already running!", AELOG_TYPE_WARN, this->getModuleName());
		return AELOG_ERR_THREAD_ALREADY_RUNNING; //we already are writing, dummy;
	}

	this->m_bRunTrd = true;
	this->m_trdWriter = std::thread(&AELogger::logWriterThread, this);
	if (!this->m_trdWriter.joinable()) {
		this->writeToLog("Could not start AELogger thread!! (exception)", AELOG_TYPE_FATAL_ERROR, this->getModuleName());
		this->closeLog();
		throw std::runtime_error("Could not start AELogger thread!");
		return AELOG_ERR_UNABLE_START_THREAD;
	}
	return AELOG_ERR_NOERROR;
}


cint AELogger::stopWriter(void) {

	this->writeToLog("Attempting to stop the log-writing thread...", AELOG_TYPE_INFO, this->getModuleName());
	this->m_bRunTrd = false;
	if (this->m_trdWriter.joinable()) {
		this->m_trdWriter.join();
		
		this->writeToLogDirectly("Successfully exited the writer thread.", AELOG_TYPE_OK, this->getModuleName()); // the log is closed, so is the queue
		return AELOG_ERR_NOERROR;
	}
	this->writeToLogDirectly("Cannot close the already-closed thread!", AELOG_TYPE_ERROR, this->getModuleName());
	return AELOG_ERR_THREAD_ALREADY_STOPPED;
}

// request a log entry and write to it
cint AELogger::writeToLog(const std::string_view logmessg, const cint logtype, const std::string_view logmodule, const std::time_t logTime) {
	if (this->isClosed()) {
		return AEFW_ERR_FILE_NOT_OPEN; // file's closed/closing!
	}

	// The only reason this function isn't shortcutted to a writeToLog with an entry
	// is simply because it is doing the same thing anyway
	// copying string data to a log entry
	// and we'll need to do the same thing if we want to use the 2nd version of writeToLog
	
	/// @todo Implement decrease in log queue size...somehow

	// check for invalid arguments
	if (logmessg.empty() || logmodule.empty() || !ace::utils::isInRange(AELOG_TYPE_DEBUG, AELOG_TYPE_FATAL_ERROR, logtype) || //empty/invalid stuff
		!ace::utils::isAlNumUs(logmodule)) { //log module stuff
		return AELOG_ERR_INVALID_ENTRY_DATA;
	}

	this->m_ullFilledCount++;
	

	if (this->m_ullFilledCount > this->m_ullQueueSize) {
		const std::lock_guard<std::mutex> lock(this->m_mtxAllocLock);
		const ullint qsize = this->m_ullQueueSize / 2;

		//make new queue
		AELogEntry* newQueue = AELogEntry::makeQueue(qsize, true, this->m_lepQueue);
		//add the new queue to the vector
		this->m_ullQueueSize += qsize;
		this->m_lepLastNode->m_pNextNode = newQueue;
		this->m_lepLastNode = newQueue + qsize - 1;
		this->m_vAllocTable.emplace_back(this->m_ullQueueSize, newQueue);
		this->writeToLogDebug("The queue was too small, resized it to " + std::to_string(this->m_ullQueueSize) + " entries", this->getModuleName());
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
	while (ptr->m_cStatus != AELE_STATUS_INVALID) {
		ptr = this->ptrFromIndex(m_ullNodeNumber++); //if current node is filled -> continue looking for unpopulated one
	}

	
	//populating it now!
	ptr->m_cStatus = AELE_STATUS_SETTING; //alright boys, we're setting this one up

	ptr->m_tmLogTime = (logTime == 0) ? std::time(nullptr) : logTime;
	memcpy(ptr->m_sLogMessage, logmessg.data(), (logmessg.size() > AELE_MESSAGE_SIZE) ? AELE_MESSAGE_SIZE : logmessg.size());
	memcpy(ptr->m_sModuleName, logmodule.data(), (logmodule.size() > AELE_MODULENAME_SIZE) ? AELE_MODULENAME_SIZE : logmodule.size());
	ptr->m_cLogType = logtype;
	ptr->m_cStatus = AELE_STATUS_READY;
	return AELOG_ERR_NOERROR;
}



void AELogger::logWriterThread(void) {
	//bit of announcements
	this->writeToLog("Successfully launched the log-writing thread!", AELOG_TYPE_SUCCESS, this->getModuleName());


	//pointers to traverse the queue
	AELogEntry* ePtr = this->m_lepQueue;
	AELogEntry* oldptr = ePtr;

	

	//the final message to output
	char str[AELE_FORMAT_MAX_SIZE]{};

	// The node order number for the writing thread.
	//ullint m_ullWriterOrderNum = 0;


	//and not stop untill it's done
	//untill we written everything *and* we stopped the thread
	

	while (this->m_bRunTrd.load(std::memory_order::relaxed) || this->m_ullFilledCount.load(std::memory_order::relaxed)) {
		while (this->m_ullFilledCount.load(std::memory_order::relaxed)) {
			
			//got it!. Now wait untill it's ready
			while (ePtr->m_cStatus != AELE_STATUS_READY) {
				ace::utils::sleepUS(100);
			}
			//the entry is minee!
			ePtr->m_cStatus = AELE_STATUS_READING;
			
			//formatting
			
			AELogEntry::formatEntry(str, *ePtr);
			
			//std::cout << str;
			
			//writing
			this->m_fwLogger.writeData_ptr(str, std::strlen(str), 1, false);
			this->m_fwLogger.flushFile();
			
			//gotta go to the next ptr
			ePtr = ePtr->m_pNextNode;

			//cleanup
			AELogEntry::clearEntry(*oldptr);
			oldptr = ePtr;
			this->m_ullFilledCount--;
			//m_ullWriterOrderNum++;
			
		}

		//since we're done writing the entries, sleep and check if some appear again
		ace::utils::sleepUS(1000);
	}
	this->writeToLogDirectly("Reached the end of the log-writing thread function", AELOG_TYPE_OK, this->getModuleName());
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



