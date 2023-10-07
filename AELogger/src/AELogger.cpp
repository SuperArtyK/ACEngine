
/** @file AELogger/src/AELogger.cpp
 *  This file contains the definitions of the engine's logger's functions
 *  @see AELogger
 *  @see AELogger.hpp
 *
 *  Should not cause everything to break.
 */

#include "../include/AELogger.hpp"
#include "include/AEUtils.hpp"
#include <utility>
#include <ctime>


AELogEntry* AELogEntry::makeQueue(const std::size_t amt, AELogEntry* oldqueue) {
	//damn, amt is really 0; get null!
	if (amt == 0) {
		throw std::runtime_error("queue size to allocate is 0!");
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


//constructor
AELogger::AELogger(const std::string& fname, const bool clearLog, const ullint queuesize) :
	m_fwLogger(fname, !clearLog * AEFW_FLAG_APPEND /* Funny magic with bool-int conversion */, 1), m_ullLogOrderNum(0), m_ullFilledCount(0), m_ullNodeNumber(0),
	m_ullWriterOrderNum(0), m_ullQueueSize(queuesize), m_lepQueue(AELogEntry::makeQueue(queuesize, nullptr)), m_lepLastNode(m_lepQueue + queuesize - 1), m_bStopTrd(false) {


	//add the allocated queue pointed to the list
	//so we can free them later without memory leaks
	this->m_vAllocTable.reserve(AELOG_DEFAULT_ALLOC_VECTOR_RESERVE);
	this->m_vAllocTable.emplace_back( queuesize, this->m_lepQueue );
	this->writeToLog("Created the AELogger instance and opened the log session in the file: \"" + fname + '\"', AELOG_TYPE_OK, this->m_sModulename);
	this->startWriter();
}

//destructor
AELogger::~AELogger() {
	this->writeToLog("Destroying the AELogger instance and closing", AELOG_TYPE_INFO, this->m_sModulename);
	this->closeLog();

	for (std::size_t i = 0; i < this->m_vAllocTable.size(); i++) {
		delete[] this->m_vAllocTable[i].second;
	}
}


void AELogger::startWriter(void) {
	this->writeToLog("Attempting to start the log-writing thread", AELOG_TYPE_INFO, this->m_sModulename);
	if (this->m_trdWriter.joinable()) {
		this->writeToLog("The log-writing thread is already running!", AELOG_TYPE_WARN, this->m_sModulename);
		return; //we already are writing, dummy;
	}

	this->m_bStopTrd = false;
	this->m_trdWriter = std::thread(&AELogger::logWriterThread, this);
	if (!this->m_trdWriter.joinable()) {
		this->writeToLog("Could not start AELogger thread!! (exception)", AELOG_TYPE_FATAL_ERROR, this->m_sModulename);
		this->closeLog();
		throw std::runtime_error("Could not start AELogger thread!");
	}
}


void AELogger::stopWriter(void) {

	this->writeToLog("Attempting to stop the log-writing thread...", AELOG_TYPE_INFO, this->m_sModulename);
	this->m_bStopTrd = true;
	if (this->m_trdWriter.joinable()) {
		this->m_trdWriter.join();
	}
}

// request a log entry and write to it
void AELogger::writeToLog(const std::string& logmessg, const cint logtype, const std::string& logmodule) {
	if (!this->isOpen()) {
		return; // file's closed/closing!
	}

	/// @todo Implement decrease in log queue size...somehow
	if (logmessg.empty()) {
		return; //na'ah, no empty messages
	}

	this->m_ullFilledCount++;

	if (this->m_ullFilledCount > this->m_ullQueueSize) {
		const std::lock_guard<std::mutex> lock(this->m_mtxAllocLock);
		const ullint qsize = this->m_ullQueueSize / 2;

		//make new queue
		AELogEntry* newQueue = AELogEntry::makeQueue(qsize, this->m_lepQueue);
		//add the new queue to the vector
		this->m_ullQueueSize += qsize;
		this->m_lepLastNode->m_lepNextNode = newQueue;
		this->m_lepLastNode = newQueue + qsize - 1;
		this->m_vAllocTable.emplace_back(this->m_ullQueueSize, newQueue);
		this->writeToLogDebug("The queue was too small, resized it to " + std::to_string(this->m_ullQueueSize) + " entries", AELOG_TYPE_WARN, this->m_sModulename);		
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
	memcpy(ptr->m_sLogMessage, logmessg.c_str(), (logmessg.size() <= 511) ? logmessg.size() : 511);
	memcpy(ptr->m_sModuleName, logmodule.c_str(), (logmodule.size() <= 31) ? logmodule.size() : 31);
	ptr->m_cLogType = logtype;
	ptr->m_cStatus = AELOG_ENTRY_STATUS_READY;
}

void AELogger::logWriterThread(void) {
	this->writeToLog("Successfully launched the log-writing thread!", AELOG_TYPE_SUCCESS, this->m_sModulename);

	AELogEntry* ePtr = this->m_lepQueue;
	char str[588]{};
	char timestr[20]{};

	//and not stop untill it's done
	//untill we written everything *and* we stopped the thread
	while (!this->m_bStopTrd || this->m_ullFilledCount) {
		while (this->m_ullFilledCount) {
			if (ePtr->m_ullOrderNum == m_ullWriterOrderNum) {

				//got it!. Now wait untill it's ready
				while (ePtr->m_cStatus != AELOG_ENTRY_STATUS_READY) {
					ace::utils::sleepUS(100);
				}
				//the entry is minee!
				ePtr->m_cStatus = AELOG_ENTRY_STATUS_READING;

				//formatting and writing
				ace::utils::formatDate(ePtr->m_tmLogTime, timestr);
				snprintf(str, 587, "[%s] [%s] [%s]: %s\n", timestr, AELogEntry::typeToString(ePtr->m_cLogType), ePtr->m_sModuleName, ePtr->m_sLogMessage);
				//std::cout << str;

				this->m_fwLogger.writeData_ptr(str, 1, std::strlen(str), true);

				//cleanup
				std::memset(str, NULL, 587); // clean the formatting buffer
				std::memset(ePtr->m_sLogMessage, NULL, 511); // clean log message
				std::memset(ePtr->m_sModuleName, NULL, 31); // clean module name
				ePtr->m_cStatus = AELOG_ENTRY_STATUS_INVALID;
				ePtr->m_ullOrderNum = AELOG_ENTRY_INVALID_ORDERNUM;
				this->m_ullFilledCount--;
				m_ullWriterOrderNum++;
			}
			//next node pls
			ePtr = ePtr->m_lepNextNode;
		}

		//since we're done writing the entries, sleep and check if some appear again
		ace::utils::sleepMS(30);
	}



	snprintf(str, 587, "[%s] [%s] [%s]: Successfully exited the writer thread.\n", ace::utils::getCurrentDate().c_str(), AELogEntry::typeToString(AELOG_TYPE_SUCCESS), this->m_sModulename);
	this->m_fwLogger.writeData_ptr(str, 1, std::strlen(str), true);
}


AELogEntry* AELogger::ptrFromIndex(ullint num) {
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