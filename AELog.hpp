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
	///atomic flag to show that AELogEntry is ready to be written
	std::atomic<bool> m_ready;
};



class AELog {
public:

	AELog() {


	}

private:
	

	AELogEntry m_logQueue;


};
 






#endif //!_AELOG_HPP