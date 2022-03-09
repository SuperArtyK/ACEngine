#include "AEFileWriter.hpp"
#include "AELog.hpp"
#include <iostream>

using std::cout;

AELogEntry* makeQueue(int length, const std::string& prefix) {
    AELogEntry* result = new AELogEntry[length];
    for (int i = 0; i < length-1; i++) {
        result[i].m_logmessage = prefix + std::to_string(i);
        result[i].m_nextnode = (&result[i]) + 1;
        result[i].m_ready = false;
    }
    result[length-1].m_logmessage = prefix + std::to_string(length-1);
    result[length-1].m_nextnode = nullptr;
    result[length-1].m_ready = false;
    return result;
}


int main() {
    int queuesize = 10;
	int queuesize2 = 10;
    AELogEntry* nodelist = makeQueue(queuesize, "1_");
	
	
	

    AELogEntry* node = nodelist;

    //nodelist[queuesize - 1].m_nextnode = node;
    while (true) {
        cout << "Node #" << node->m_logmessage << "; Address: " << node << '\n';
        if(node->m_nextnode){
			node = node->m_nextnode;
		}
		else{
			break;
		}
		
		
    }
	cout<<"------------------------\n";
	AELogEntry* nodelist2 = makeQueue(queuesize2, "2_");
	nodelist[queuesize-1].m_nextnode = nodelist2;
	node = nodelist;
	while (true) {
        cout << "Node #" << node->m_logmessage << "; Address: " << node << '\n';
        if(node->m_nextnode){
			node = node->m_nextnode;
		}
		else{
			break;
		}
		
		
    }
	
	cout<<"Press enter to continue . . .";
    std::cin.get();
    delete[] nodelist;
	delete[] nodelist2;
    return 0;
}
