#include "AEFileWriter.hpp"
#include "AELog.hpp"
#include "AEFrame.hpp"
#include <iostream>
#include <mutex>

using std::cout;

std::atomic<bool> exitTrd = false;
std::atomic<bool> writerFinish = false;
AELogEntry* queue = nullptr;
std::atomic<uint> filledCount = 0;


AELogEntry* makeQueue(int length) {
	AELogEntry* result = new AELogEntry[length];
	for (int i = 0; i < length; i++) {
		result[i].m_logmessage.reserve(1024);
		result[i].m_nextnode = (&result[i]) + 1;
		result[i].m_ordernum = AELE_INVALID_ENTRY_ORDERNUM;
		result[i].m_status = false;
	}
	result[length-1].m_nextnode = nullptr;
	return result;
}



///arguments:
/// ptr to queue to iterate
///atomic reference to queue-filling counter
///atomic reference to stop function loop(false to stop)
///function iterates through the queue untill last node is nullptr
///
void writeQueue() {
	writerFinish = false;
	//var to check if entry is valid
	//1 bigger than invalid
	unsigned long long int ordernum = AELE_INVALID_ENTRY_ORDERNUM+1;
	//pointer to item in the queue
	//iteration pointer
	AELogEntry* itemPtr = queue;
	AEFrame myfr(100);
	while (!exitTrd) {
		if(filledCount > 0){
			//if the queue entry is correct to be written
			if(itemPtr->m_ordernum == ordernum){
				//sleep and wait while status of write entry is not true(done)
				//so currenly being written to
				while(!itemPtr->m_status){
					myfr.sleep();
				}
				//output text
				cout<<itemPtr->m_logmessage<<'\n';
				ordernum++;
				itemPtr->m_ordernum = -1;
				itemPtr->m_logmessage.clear();	
				itemPtr->m_status = false;
				filledCount--;
			}
			itemPtr = itemPtr->m_nextnode;
			
		}
		else{
			while(filledCount != 1){
				myfr.sleep();
			}
			
		}
		
	}
	writerFinish = true;
}



void addEntry(const std::string& text){
	//array(table) with all allocation pointers to free
	static std::vector<AELogEntry*> allocTable;
	//last item in the queue
	static AELogEntry* l_last = queue;
	//item to write to
	static AELogEntry* current = queue;
	//mutex to lock when allocating new
	static std::mutex l_mtx;
	//size of the queue
	static uint l_qSize = AELG_DEFAULT_QUEUE_SIZE;
	//flag if we init'ed or not
	static bool l_init = false;
	static unsigned long long int ordernum = AELE_INVALID_ENTRY_ORDERNUM+1;
	if(!exitTrd){
		
		//do only once
		//find size of the queue
		if(!l_init){
			while(l_last->m_nextnode){
				l_last = l_last->m_nextnode;
				l_qSize++;
			}
			//we won't need much more
			//but still leave as vector just in case
			allocTable.reserve(32);
			allocTable.push_back(queue);
		}

		//check if queue is almost full
		if((filledCount+1) == l_qSize){
			//set mutex IMMEDEATELY
			//to prevent over
			l_mtx.lock();
			//allocate new chunk, twice as big as previous
			//and initialise
			AELogEntry* temp = new AELogEntry[l_qSize*2];
			for(unsigned i = 0; i < l_qSize*2; i++){
				temp[i].m_logmessage.reserve(1024);
				temp[i].m_nextnode = &temp[i+1];
				temp[i].m_ordernum = -1;
				temp[i].m_status = false;
			}
			//update the last node
			l_last->m_nextnode = &temp[0];
			//set the last-node-ptr to the last place of allocated chunk
			l_last = &temp[(l_qSize*2)-1];
			//make the last node loop
			temp[(l_qSize*2)-1].m_nextnode = queue;
			
			l_qSize += l_qSize*2;
			l_mtx.unlock();
		}
		
		if(!text.empty()){
			//set the order number, so writer locks on it
			current->m_ordernum = ordernum++;
			current->m_logmessage = text;
			//the node is ready to be written
			current->m_status = true;
		}
	
	}
	else{
		if(allocTable.size()>0){
		//lock the mutex so we can deallocate the table properly
			l_mtx.lock();
			for(int i = 0; i<allocTable.size(); i++){
				delete[] allocTable[i];
			}
			//clear the table so others won't access it
			allocTable.clear();
			l_mtx.unlock();
			
		}
	}
	
	
}


int main() {
	cout<<"Press enter to continue . . .";
	std::cin.get();
	return 0;
}
