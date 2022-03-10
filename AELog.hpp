#ifndef _AELOG_HPP
#define _AELOG_HPP

#include "AEFileWriter.hpp"
#include <thread>
#include <atomic>
#include <thread>

struct AELogEntry {
	///message of the log
	std::string m_logmessage;
	///next node of the log
	AELogEntry* m_nextnode = nullptr;
	///order number of the entry, required for consequent writing
	///and thread-safety of the log
	///-1 ignores node
	///checked first before waiting for 
	std::atomic<int> m_ordernum = -1;
	///atomic flag to show that AELogEntry is ready to be written -- 1
	///or it is being written currently to -- 2
	///or a it's a free node -- 0
	std::atomic<char> m_status;
};



class AELog {
public:

	AELog() {


	}

private:
	

	AELogEntry m_logQueue;


};
 






#endif //!_AELOG_HPP