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

	AELog(const std::string& filename, const unsigned int initQueueSize = AELG_DEFAULT_QUEUE_SIZE) : m_logWriter( filename, AEFW_FLAG_APPEND, 10), queue(new AELogEntry [initQueueSize]), filledCount(0), m_exitTrd(false){
		//we WONT need any more, unless you have 480PB of free ram and ready to process 6*10^15 entries
		m_allocTable.reserve(32);
		m_allocTable.push_back(queue);
	}
	~AELog(){
		for(int i = 0; i< m_allocTable.size();i++){
			delete[] m_allocTable[i];
		}
	}
	void writeLog(const std::string& text){
		
	}
	std::string getFileName() const {
		return	m_logWriter.getFileName();
	}
private:
	AEFileWriter m_logWriter;
	std::vector<AELogEntry*> m_allocTable;
	AELogEntry* queue;
	std::atomic<uint> filledCount;
	std::atomic<bool> m_exitTrd;
	
	
	
	


};
 






#endif //!_AELOG_HPP