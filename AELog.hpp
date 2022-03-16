#ifndef _AELOG_HPP
#define _AELOG_HPP

#include "AEFileWriter.hpp"
#include <thread>
#include <atomic>
#include <thread>
///macro for ordernum to be ignored by log writer
#define AELE_INVALID_ENTRY_ORDERNUM 0

struct AELogEntry {
	/// message of the log
	std::string m_logmessage;
	/// next node of the log
	AELogEntry* m_nextnode = nullptr;
	/// order number of the entry, required for consequent writing
	/// and thread-safety of the log
	/// -1 ignores node
	/// checked first before waiting for status
	std::atomic<uint> m_ordernum;
	/// atomic flag to show that AELogEntry is ready to be written
	/// or not 
	std::atomic<bool> m_status;
};


#define AELG_DEFAULT_QUEUE_SIZE 10


class AELog {
public:

	AELog() {


	}

private:
	

	AELogEntry m_logQueue;


};
 






#endif //!_AELOG_HPP