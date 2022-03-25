
#pragma once

#ifndef AELOG_HPP
#define AELOG_HPP


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



#define AELG_DEFAULT_QUEUE_SIZE 10

//TODO: order vars properly to avoid byte-padding

///Default engine's file logger. Err, logs messages to given file
///Hungarian notatios is 'lg'
///Flags start with AELOG_
class AELog : __AEModuleBase<AELog> {
public:
	AELog(const std::string &filename, const unsigned int initQueueSize = AELG_DEFAULT_QUEUE_SIZE) : 
	m_fwLogWriter(filename, AEFW_FLAG_APPEND, 10), m_trdWriter(&AELog::writerThread, this),m_ullOrderNum(AELE_INVALID_ENTRY_ORDERNUM), 
	m_lepQueue(makeQueue(initQueueSize)), m_lepCurrentNode(m_lepQueue.load()), m_lepLastNode(m_lepQueue.load()+initQueueSize), 
	m_iFilledCount(0), m_iQSize(initQueueSize), m_bExitTrd(false)
	{
		//we probably WONT need any more, unless you have spare 1.25PB of ram to use
		devcout("Creating logger with filename: \"" << filename << "\"; and queue size of: " << initQueueSize);
		devcout("Estimated memory usage: " << (sizeof(AELog) + sizeof(AELogEntry) * initQueueSize * 1024) << "bytes");
		m_lepLastNode->m_lepNextNode = m_lepQueue;
		m_vecAllocTable.reserve(48);
		m_vecAllocTable.push_back(m_lepQueue);
	}
	~AELog(){
		m_bExitTrd = true;
		if (m_trdWriter.joinable())
			m_trdWriter.join();
		devcout("Destroying logger with filename: \"" << m_fwLogWriter.getFileName() << "\"");
		for(int i = 0; i< m_vecAllocTable.size();i++){
			devcout("Freeing allocated queue ptr at " << m_vecAllocTable[i]);
			delete[] m_vecAllocTable[i];
			
		}
		m_fwLogWriter.closeFile();
		m_vecAllocTable.clear();
	}

	void writeToLog(const std::string &l_strMessg, const smalluint l_iType = AELOG_TYPE_INFO, const std::string &l_sModuleName = "Engine"){
		// unfortunately all entries made when object was destructing
		// should be discarded
		devcout("Trying to write to log. Checking status...");
		if (!m_bExitTrd){
			devcout("Writing thread isn't exiting/hasn't exited, continuing with procedure...");
			// check if next node is populated
			devcout("Checking queue: " << m_iFilledCount << " currently filled out of " << m_iQSize);
			if (m_iFilledCount == m_iQSize)
			{
				devcout("-Queue is full, starting allocation sequence");
				// oh boy, it is
				// le mutex to prevent access
				devcout("--Mutex lock");
				m_mtx.lock();
				//allocate same amount of memory
				devcout("--Allocating " << m_iQSize << " entries");
				AELogEntry *temp = makeQueue(m_iQSize);
				devcout("--Changing pointers...");
				m_lepLastNode->m_lepNextNode = temp;
				m_lepLastNode = temp + m_iQSize - 1;
				devcout("--m_lepLastNode->m_lepNextNode now points at " << m_lepLastNode->m_lepNextNode);
				devcout("--m_lepLastNode now points to " << m_lepLastNode);

				devcout("--Adding ptr to m_vecAllocTable...");
				m_vecAllocTable.push_back(temp);
				devcout("--Increasing queue size counter...");
				m_iQSize *= 2;
				devcout("--Unlocking mutex");
				m_mtx.unlock();
				devcout("-Allocation sequence complete");
			}
			devcout("-Checking string...");
			if(!l_strMessg.empty()){
				AELogEntry * const temp = m_lepCurrentNode.load();
				devcout("-non-empty");
				devcout("--Writing to the cell(" << temp << ")...");
				m_iFilledCount++;

				devcout("--Incremented m_iFilledCount. It is: " << m_iFilledCount);
				temp->m_ullOrderNum = ++(this->m_ullOrderNum);
				devcout("--Incremented m_ullOrderNum. It is: " << m_ullOrderNum);
				temp->m_sLogMessage = l_strMessg;
				temp->m_sModuleName = l_sModuleName;
				temp->m_tmLogTime = time(NULL);
				temp->m_ucLogType = l_iType;
				devcout("--Written data. Ready to deploy");
				temp->m_bStatus = true;
				devcout("--Incrementing m_lepCurrentNode...");
				m_lepCurrentNode = temp->m_lepNextNode;
				devcout("-Entry is fully written and ready");
			}
		}
		else{
			devcout("Writing thread is exiting/has exited, aborting the procedure...");
		}
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
			result[i].m_sLogMessage.reserve(1024);
			result[i].m_lepNextNode = result+i+1;//point to next node
			result[i].m_ullOrderNum = AELE_INVALID_ENTRY_ORDERNUM;
			result[i].m_bStatus = false;
		}
		result[size - 1].m_lepNextNode = result;
		return result;
	}

	void writerThread(){
		devcout("Entered writer thread");
		// node-runner
		AELogEntry *l_lepNode = m_lepQueue;
		AEFrame myfr(100);
		biguint l_ullOrderNum = AELE_INVALID_ENTRY_ORDERNUM+1;
		devcout("-Allocated variables");
		devcout("Starting Loop");
		while (!m_bExitTrd || m_iFilledCount > 0)
		{
			devcout("-Loop repeat. m_bExitTrd is not true, or m_iFilledCount > 0");
			devcout("-Checking queue filling...");
			if (m_iFilledCount > 0)
			{
				devcout("queue is filled: "<<m_iFilledCount);
				// traverse to next available node
				devcout("l_ullOrderNum = " << l_ullOrderNum);
				devcout("-Trying to find next populated node");
				while(l_lepNode->m_ullOrderNum != l_ullOrderNum){
					l_lepNode = l_lepNode->m_lepNextNode;
				}
				//waiting for node to be "ready"
				devcout("-Waiting for entry to be ready");
				while (!l_lepNode->m_bStatus)
				{
					myfr.sleep(); // sleep 1+ms; usually 1 should be enough
				}

				devcout("-Preparing...");
				//time of log
				tm tstruct;
				char buff[80];
#if _MSC_VER && !__INTEL_COMPILER
//compiler is msvc for sure
//we need that to avoid 'unsecure function' errors
//without disabling them
				localtime_s(&tstruct, &l_lepNode->m_tmLogTime);
#else
				tstruct = *localtime(&l_lepNode->m_tmLogTime);
#endif
				strftime(buff, sizeof(buff), "\n[ %Y-%m-%d.%X ] [", &tstruct);
				devcout("--Got time of the entry");
				devcout("--Writing...");
				m_fwLogWriter.writeString(buff);
				m_fwLogWriter.writeString(checkLogType(l_lepNode->m_ucLogType));
				m_fwLogWriter.writeString("] [");
				m_fwLogWriter.writeString(l_lepNode->m_sModuleName);
				m_fwLogWriter.writeString("]: ");
				m_fwLogWriter.writeString(l_lepNode->m_sLogMessage);
				devcout("-Written Entry. Cleaning up");
				
				l_lepNode->m_bStatus = false;
				l_lepNode->m_ullOrderNum = AELE_INVALID_ENTRY_ORDERNUM;
				m_iFilledCount--;
				l_ullOrderNum++;
			}
			else
			{
				// waiting for node to be "ready"
				devcout("--Queue seems to be empty. Waiting...");
				while (!l_lepNode->m_bStatus)
				{
					myfr.sleep(); // sleep 1+ms; usually 1 should be enough
				}
			}
			
		}
		devcout("-Loop exited. m_bExitTrd is false, and m_iFilledCount == 0");
		devcout("Probably closing. Exiting thread...");
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
	std::atomic<AELogEntry *> m_lepQueue;
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


#endif //!AELOG_HPP