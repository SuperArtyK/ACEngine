
#pragma once

#ifndef ENGINE_AELOG_HPP
#define ENGINE_AELOG_HPP

#include <iostream>
#include <ctime>
#include <mutex>
#include "typedefs.hpp"
#include "AELog_types.hpp"
#include "custom_types.hpp"
#include "AEModuleBase.hpp"
#include "AEFileWriter.hpp"
#include "AEFrame.hpp"



/// If-def Flag: Use debug (cout) output made with devcout() macro in code
/// @warning Multithreaded stuff can rip the cout output
#define AE_DEBUG_COUT

#ifdef AE_DEBUG_COUT
#define devcout(x) std::cout <<"DEBUG::"<<__FUNCTION__<<"::" x <<'\n'
#else
#define devcout(x)
#endif


///macro for AELog's default queue size if it was not given
#define AELOG_DEFAULT_QUEUE_SIZE 10
///macro for AELog::makeQueue()'s m_sLogMessage.reserve() value
#define AELOG_DEFAULT_STRING_RESEVE 256

//TODO: order vars properly to avoid byte-padding

///Default engine's file logger. Err, logs messages to given file
///Hungarian notatios is 'lg'
///Flags start with AELOG_
class AELog : __AEBasicModule<AELog> {
public:
	AELog(const std::string &filename, const unsigned int initQueueSize = AELOG_DEFAULT_QUEUE_SIZE) : 
	m_fwLogWriter(filename, AEFW_FLAG_APPEND, 10), m_trdWriter(&AELog::writerThread, this),m_ullOrderNum(AELE_INVALID_ENTRY_ORDERNUM), 
	m_lepQueue(makeQueue(initQueueSize)), m_lepCurrentNode(m_lepQueue), m_lepLastNode(m_lepQueue+initQueueSize-1), 
	m_iFilledCount(0), m_iQSize(initQueueSize), m_bExitTrd(false)
	{
		//we probably WONT need any more, unless you have spare 1.25PB of ram to use
		devcout("Creating logger with filename: \"" << filename << "\"; and queue size of: " << initQueueSize);
		devcout("Estimated memory usage: " << (sizeof(AELog) + sizeof(AELogEntry) * initQueueSize * AELOG_DEFAULT_STRING_RESEVE) << "B");
		m_lepLastNode->m_lepNextNode = m_lepQueue;
		m_vecAllocTable.reserve(48);
		m_vecAllocTable.push_back(m_lepQueue);
	}
	~AELog(){
		devcout("Destroying logger with filename: \"" << m_fwLogWriter.getFileName() << "\"");
		m_bExitTrd = true;
		if (m_trdWriter.joinable()){
			devcout("Thread is running and joinable. Joining...");
			m_trdWriter.join();
		}
		devcout("Freeing allocated queues in m_vecAllocTable...");
		devcout("m_vecAllocTable.size() = "<<m_vecAllocTable.size());
		for(int i = 0; i< m_vecAllocTable.size();i++){
			devcout("Freeing allocated queue ptr at " << m_vecAllocTable[i]);
			delete[] m_vecAllocTable[i];
			
		}
		m_fwLogWriter.closeFile();
		devcout("Closed file writing");
		m_vecAllocTable.clear();
		devcout("Cleared allocation table m_vecAllocTable");
		
	}

	void writeToLog(const std::string &l_strMessg, const smalluint l_iType = AELOG_TYPE_INFO, const std::string &l_sModuleName = "Engine"){
		
	}

	inline std::string getFileName() const
	{
		return	m_fwLogWriter.getFileName();
	}


private:
	///allocates queue of AELogEntry with size 'size' and returns ptr to first element
	AELogEntry *makeQueue(const biguint size) const
	{
		AELogEntry *result = new AELogEntry[size];
		for (int i = 0; i < size; i++)
		{
			result[i].m_sLogMessage.reserve(AELOG_DEFAULT_STRING_RESEVE);
			result[i].m_sModuleName.reserve(12);//usually more than enough
			result[i].m_lepNextNode = &result[i+1];//point to next node
			result[i].m_ullOrderNum = AELE_INVALID_ENTRY_ORDERNUM;
			result[i].m_bStatus = false;
		}
		result[size - 1].m_lepNextNode = &result[0];
		return result;
	}

	void writerThread(){
		
	}

	inline static const char* checkLogType(const smallint logtype){
			switch (logtype)
			{

			case AELOG_TYPE_WARN:
				return "WARN";
				break;

			case AELOG_TYPE_ERROR:
				return "ERROR";
				break;

			// also AELOG_TYPE_FERROR
			case AELOG_TYPE_FATAL_ERROR:
				return "FATAL_ERROR";
				break;

			// also AELOG_TYPE_SWARN
			case AELOG_TYPE_SEVERE_WARNING:
				return "SWARN";
				break;

			case AELOG_TYPE_OK:
				return "OK";
				break;

			case AELOG_TYPE_SUCCESS:
				return "SUCCESS";
				break;

			// AELOG_TYPE_INFO by default
			default:
				return "INFO";
				break;
			}
	}


	/// file writer for logs
	AEFileWriter m_fwLogWriter;
	/// mutex to lock when allocating new chunks
	std::mutex m_mtx;
	/// list of all heap-allocated pointers
	std::vector<AELogEntry *> m_vecAllocTable;
	/// writer thread of the logger
	std::thread m_trdWriter;
	/// order number for next queue entry
	std::atomic<biguint> m_ullOrderNum;
	/// pointer to the first allocation, and the queue
	AELogEntry * m_lepQueue;
	/// current node for queue entry
	std::atomic<AELogEntry *> m_lepCurrentNode;
	/// last node of the whole queue
	AELogEntry *m_lepLastNode;
	/// amount of filled entries in the queue
	std::atomic<int> m_iFilledCount;
	/// size of the whole queue
	int m_iQSize;
	/// flag if to stop the write thread
	std::atomic<bool> m_bExitTrd;
};

ADD_MODULE_TO_ENGINE(AELog)


#endif //!ENGINE_AELOG_HPP
