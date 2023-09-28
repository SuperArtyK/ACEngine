
#include "AELogger.hpp"
#include <iostream>
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
	m_fwLogger(fname, !clearLog * AEFW_FLAG_APPEND /* Funny magic with bool-int conversion */, 1), m_ullLogOrderNum(0), m_ullFilledCount(0), m_ullNodeNumber(0), m_lepQueue(AELogEntry::makeQueue(queuesize)),
	m_lepLastNode(m_lepQueue + queuesize - 1), m_ullQueueSize(queuesize), m_bStopTrd(false) {


	//add the allocated queue pointed to the list
	//so we can free them later without memory leaks
	this->m_vAllocTable.reserve(32);
	this->m_vAllocTable.push_back({ queuesize, m_lepQueue });
	this->startWriter();
}

//destructor
AELogger::~AELogger() {
	this->stopWriter();

	for (std::size_t i = 0; i < this->m_vAllocTable.size(); i++) {
		delete[] this->m_vAllocTable[i].second;
	}

	this->m_fwLogger.closeFile();
	this->m_vAllocTable.clear();
}

void AELogger::stopWriter(void) {
	this->m_bStopTrd = true;
	if (this->m_trdWriter.joinable()) {
		this->m_trdWriter.join();
	}
}

void AELogger::startWriter(void) {
	if (this->m_trdWriter.joinable()) {
		return; //we already are writing, dummy;
	}

	this->m_bStopTrd = false;
	this->m_trdWriter = std::thread(&AELogger::logWriterThread, this);
	if (!this->m_trdWriter.joinable()) {
		throw std::runtime_error("Could not start AETimer thread!");
	}
}


// request a log entry and write to it
void AELogger::writeToLog(const std::string& logmessg, const ucint logtype, const std::string& logmodule) {
	if (!this->isOpen()) {
		return; // file's closed/closing!
	}

	if (this->m_ullFilledCount > this->m_ullQueueSize) {
		std::lock_guard<std::mutex> lock(this->m_mtxAllocLock);
		ullint qsize = this->m_ullQueueSize / 2;

		//make new queue
		AELogEntry* newQueue = AELogEntry::makeQueue(qsize, this->m_lepQueue);
		//add the new queue to the vector
		this->m_ullQueueSize += qsize;
		this->m_lepLastNode->m_lepNextNode = newQueue;
		this->m_lepLastNode = newQueue + qsize - 1;
		this->m_vAllocTable.push_back({ this->m_ullQueueSize, newQueue });
		
	}

	//TODO: implement decrease in log queue size
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
		ptr = this->ptrFromIndex(m_ullNodeNumber++); //if current node is filled -> continue looking for unpopulated one
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

void AELogger::logWriterThread(void) {

	AELogEntry* ePtr = this->m_lepQueue;
	ullint orderNum = 0;
	char str[588]{};
	//and not stop untill it's done

	while (!this->m_bStopTrd) {
		while (this->m_ullFilledCount) {
			if (ePtr->m_ullOrderNum == orderNum) {

				//got it!. Now wait untill it's ready
				while (ePtr->m_bStatus != AELOG_ENTRY_STATUS_READY) {
					ace::utils::sleepUS(100);
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