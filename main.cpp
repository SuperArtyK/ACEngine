#include "AEFileWriter.hpp"
#include "AELog.hpp"
#include <iostream>

using std::cout;

AELogEntry* makeQueue(int length, const std::string& prefix) {
    AELogEntry* result = new AELogEntry[length];
    for (int i = 0; i < length-1; i++) {
        result[i].m_logmessage.reserve(1024);
        result[i].m_logmessage = prefix + std::to_string(i);
        result[i].m_nextnode = (&result[i]) + 1;
        result[i].m_ready = false;
    }
    result[length - 1].m_logmessage.reserve(1024);
    result[length-1].m_logmessage = prefix + std::to_string(length-1);
    result[length-1].m_nextnode = nullptr;
    result[length-1].m_ready = false;
    return result;
}




void writeQueue(AELogEntry* queue, std::atomic<int>& filledCount, std::atomic<AELogEntry*>& lastptr, std::atomic<bool>& stop) {
    
    while (!stop) {




    }
    
}



int main() {
    int queuesize = 1000;
    cout << "make vector of pointers of allocated memory\n";
    std::vector<AELogEntry*> entrypVector;
    entrypVector.reserve(1024);
    cout << "make a "<< queuesize<<" item queue\n";
    AELogEntry* queue = makeQueue(1000, "1_");
    queue[queuesize - 1].m_nextnode = queue;
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
