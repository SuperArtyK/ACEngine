#include "AEFileWriter.hpp"
#include "AELog.hpp"
#include "AEFrame.hpp"
#include <iostream>

using std::cout;

AELogEntry* makeQueue(int length, const std::string& prefix) {
    AELogEntry* result = new AELogEntry[length];
    for (int i = 0; i < length-1; i++) {
        result[i].m_logmessage.reserve(1024);
        result[i].m_logmessage = prefix + std::to_string(i);
        result[i].m_nextnode = (&result[i]) + 1;
        result[i].m_status = 0;
    }
    result[length - 1].m_logmessage.reserve(1024);
    result[length-1].m_logmessage = prefix + std::to_string(length-1);
    result[length-1].m_nextnode = nullptr;
    result[length-1].m_status = 0;
    return result;
}



///arguments:
/// ptr to queue to iterate
///atomic reference to queue-filling counter
///atomic reference to stop function loop(false to stop)
///function iterates through the queue untill last node is nullptr
///
void writeQueue(AELogEntry* queue, std::atomic<int>& filledCount, std::atomic<bool>& stop, std::atomic<AELogEntry*>& fillAddr) {
    int ordernum = 0;
	//pointer to item in the queue
	//iteration pointer
	AELogEntry* itemPtr = queue;
	AEFrame myfr(100);
    while (stop) {
		if(filledCount){
			if(itemPtr->m_ordernum == ordernum){
			//if the queue entry is correct to be written
				
			}
			else{
				itemPtr = itemPtr->m_nextnode;
			}
			
		}
		else{

			
		}
		
    }
    
}



int main() {
	
	cout<<sizeof(AELogEntry)<<'\n';
    int queuesize = 1000;
    cout << "make vector of pointers of allocated memory\n";
    std::vector<AELogEntry*> entrypVector;
	
	
    entrypVector.reserve(1024);
    cout << "make a "<< queuesize<<" item queue\n";
    AELogEntry* queue = makeQueue(1000, "1_");
	AELogEntry* last = queue;
	while(last->m_nextnode){
		last = last->m_nextnode;
	}
	last->m_nextnode = queue;
    cout << "add queue to vector\n";
    entrypVector.push_back(queue);
    cout<<"spawn thread to write from queue\n";
    


    cout << "Deallocation queues\n";
    for (int i = 0; i < entrypVector.size(); i++) {
        delete[] entrypVector[i];
    }
	
	cout<<"Press enter to continue . . .";


    std::cin.get();
    return 0;
}
