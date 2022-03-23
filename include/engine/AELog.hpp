#ifndef _AELOG_HPP
#define _AELOG_HPP


#include <iostream>
#include <ctime>
#include <mutex>
#include "typedefs.hpp"
#include "AELog_types.hpp"
#include "AEModuleBase.hpp"
#include "AEFileWriter.hpp"
#include "AEFrame.hpp"


/// If-def Flag: Use debug (cout) output made with devcout() macro in code
/// @warning Multithreaded stuff can rip the cout output
//#define AE_DEBUG_COUT

#ifdef AE_DEBUG_COUT
#define devcout(x) std::cout << x
#else
#define devcout(x)
#endif

/// macro for entry order-number to be ignored by log writer
#define AELE_INVALID_ENTRY_ORDERNUM 0

///Log entry struct for the, well, logger.
///Hungarian notation is "le".
///Flags start with AELE_
struct AELogEntry {

	/// message of the log
	std::string m_sLogMessage;
	/// module that issued the log entry
	std::string m_sModuleName;
	/// time when log entry created
	std::time_t m_tmLogTime;

	/// next log entry (node)
	AELogEntry* m_lepNextNode = nullptr;
	
	/// order number of the entry, required for consequent writing
	/// and thread-safety of the log
	/// -1 ignores node
	/// checked first before waiting for status
	std::atomic<biguint> m_ullOrderNum;

	/// type of the log
	/// refer to aelog_types.hpp
	smalluint m_ucLogType;

	/// atomic flag to show that AELogEntry is ready to be written
	/// or not 
	std::atomic<bool> m_bStatus;

	const static AELogEntry clear(){
		return {"","",0,nullptr,AELE_INVALID_ENTRY_ORDERNUM,AELOG_TYPE_INFO,false};
	}
};


#define AELG_DEFAULT_QUEUE_SIZE 10

//TODO: order vars properly to avoid byte-padding

///Default engine's file logger. Err, logs messages to given file
///Hungarian notatios is 'lg'
///Flags start with AELOG_
class AELog : __AEModuleBase<AELog> {
public:
	AELog(const std::string &filename, const unsigned int initQueueSize = AELG_DEFAULT_QUEUE_SIZE) : 
	m_fwLogWriter(filename, AEFW_FLAG_APPEND, 10), m_lepQueue(makeQueue(initQueueSize)), m_ullFilledCount(0), m_bExitTrd(false), 
	m_ullQSize(initQueueSize), m_lepLastNode(m_lepQueue.load()+initQueueSize), m_lepCurrentNode(m_lepQueue.load()), m_ullOrderNum(AELE_INVALID_ENTRY_ORDERNUM)
	{
		//we probably WONT need any more, unless you have spare 1.25PB of ram to use
		devcout("Creating logger with filename: \"" << filename << "\"; and queue size of: " << initQueueSize << '\n');
		devcout("Estimated memory usage: " << (sizeof(AELog) + sizeof(AELogEntry) * initQueueSize * 1024) << "bytes\n");
		m_lepLastNode->m_lepNextNode = m_lepQueue;
		m_vecAllocTable.reserve(48);
		m_vecAllocTable.push_back(m_lepQueue);
	}
	~AELog(){
		m_bExitTrd = true;
		m_trdWriter.join();
		devcout("Destroying logger with filename: \"" << m_fwLogWriter.getFileName() << "\"\n");
		for(int i = 0; i< m_vecAllocTable.size();i++){
			devcout("Freeing ptr " << m_vecAllocTable[i]<<'\n');
			delete[] m_vecAllocTable[i];
			
		}
		m_fwLogWriter.closeFile();
		m_vecAllocTable.clear();
	}

	void writeToLog(const std::string &l_strMessg, const char l_iType = AELOG_TYPE_INFO, const std::string &l_sModuleName = "Engine"){
		// unfortunately all entries made when object was destructing
		// should be discarded
		//  WTL, short for writeToLog
		devcout("WTL:Trying to write to log. Checking status...\n");
		if (!m_bExitTrd){
			devcout("WTL:Writing thread isn't exiting/hasn't exited, continuing with procedure...\n");
			// check if next node is populated
			devcout("WTL:Checking size of queue: " << m_ullFilledCount << " currently filled out of " << m_ullQSize << '\n');
			if (m_ullFilledCount == m_ullQSize)
			{
				devcout("WTL:-Queue is full, starting allocation sequence\n");
				// oh boy, it is
				// le mutex to prevent access
				devcout("WTL:--Mutex lock\n");
				m_mtx.lock();
				//allocate same amount of memory
				devcout("WTL:--Allocating " << m_ullQSize << " entries\n");
				AELogEntry *temp = makeQueue(m_ullQSize);
				devcout("WTL:--Changing pointers...\n");
				m_lepLastNode->m_lepNextNode = temp;
				m_lepLastNode = temp + m_ullQSize - 1;
				devcout("WTL:--m_lepLastNode->m_lepNextNode now points at " << m_lepLastNode->m_lepNextNode << '\n'
					<< "WTL:--m_lepLastNode now points to " << m_lepLastNode<<'\n');

				devcout("WTL:--Adding ptr to m_vecAllocTable...\n");
				m_vecAllocTable.push_back(temp);
				devcout("WTL:--Increasing queue size counter...\n");
				m_ullQSize *= 2;
				devcout("WTL:--Unlocking mutex\n");
				m_mtx.unlock();
				devcout("WTL:-Allocation sequence complete\n");
			}
			devcout("WTL:-Checking string: ");
			if(!l_strMessg.empty()){
				devcout("non-empty\n--Writing to the cell(" << (m_lepCurrentNode.load())<<")...\n");
				m_ullFilledCount++;
				devcout("WTL:--Incremented m_ullFilledCount. It is: " << m_ullFilledCount << '\n');
				(m_lepCurrentNode.load())->m_ullOrderNum = ++(this->m_ullOrderNum);
				devcout("WTL:--Incremented m_ullOrderNum. It is: " << m_ullOrderNum << '\n');
				(m_lepCurrentNode.load())->m_sLogMessage = l_strMessg;
				(m_lepCurrentNode.load())->m_sModuleName = l_sModuleName;
				(m_lepCurrentNode.load())->m_tmLogTime = time(NULL);
				devcout("WTL:--Written data. Ready to deploy\n");
				(m_lepCurrentNode.load())->m_bStatus = true;
				devcout("WTL:--Incrementing m_lepCurrentNode...\n");
				m_lepCurrentNode = (m_lepCurrentNode.load())->m_lepNextNode;
				devcout("WTL:-Entry is fully written and ready\n");
			}
		}
		else{
			devcout("WTL:Writing thread is exiting/has exited, aborting the procedure...\n");
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
		//WriterThreaD
		devcout("WTD:Entered writer thread\n");
		// node-runner
		AELogEntry *l_lepNode = m_lepQueue;
		AEFrame myfr(100);
		biguint l_ullOrderNum = AELE_INVALID_ENTRY_ORDERNUM+1;
		devcout("WTD:-Allocated variables\n");
		devcout("WTD:Starting Loop");
		while (!m_bExitTrd)
		{
			devcout("WTD:-Loop repeat. m_bExitTrd is not true");
			devcout("WTD:-Checking queue filling...");
			if (m_ullFilledCount > 0)
			{
				devcout("filled.");
				devcout("WTD:-Writing...\n");
				// traverse to next available node

				devcout("WTD:--Trying to find next populated node\n");
				while(l_lepNode->m_ullOrderNum != l_ullOrderNum){
					l_lepNode = l_lepNode->m_lepNextNode;
				}
				//waiting for node to be "ready"
				devcout("WTD:--Waiting for entry to be ready");
				while (!l_lepNode->m_bStatus)
				{
					myfr.sleep(); // sleep 1+ms; usually 1 should be enough
				}

				devcout("WTD:--Preparing...\n");
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
				strftime(buff, sizeof(buff), "[ %Y-%m-%d.%X ] [", &tstruct);
				m_fwLogWriter.writeString(buff);
				m_fwLogWriter.writeString(checkLogType(l_lepNode->m_ucLogType));
				m_fwLogWriter.writeString("] [");
				m_fwLogWriter.writeString(l_lepNode->m_sModuleName);
				m_fwLogWriter.writeString("]: ");
				m_fwLogWriter.writeString(l_lepNode->m_sLogMessage);
				devcout("WTD:-Written Entry. Cleaning up\n");
				
				l_lepNode->m_bStatus = false;
				l_lepNode->m_ullOrderNum = AELE_INVALID_ENTRY_ORDERNUM;
			}
			else
			{
				/* code */
			}
			
		}
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
	/// list of all heap-allocated pointers
	std::vector<AELogEntry *> m_vecAllocTable;
	/// pointer to the first allocation, and the queue
	std::atomic<AELogEntry *> m_lepQueue;
	/// amount of filled entries in the queue
	std::atomic<biguint> m_ullFilledCount;
	/// flag if to stop the write thread
	std::atomic<bool> m_bExitTrd;
	/// size of the whole queue
	int m_ullQSize;
	/// mutex to lock when allocating new chunks
	std::mutex m_mtx;
	/// order number for next queue entry
	std::atomic<biguint> m_ullOrderNum;
	/// last node of the whole queue
	AELogEntry* m_lepLastNode;
	/// current node for queue entry
	std::atomic<AELogEntry *> m_lepCurrentNode;
	/// writer thread of the logger
	std::thread m_trdWriter;
};

ADD_MODULE_TO_ENGINE(AELog)


#endif //!_AELOG_HPP