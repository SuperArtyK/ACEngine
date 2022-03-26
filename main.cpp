#include <thread>
#include <iostream>
#include <atomic>
#include <mutex>
#include "AELog.hpp"
#include "AESleep.hpp"
using std::cout;

std::atomic<bool> exitthread;
AELog g_mylg("log.txt",100);
void worker(const std::string name){
	cout<<"-----entered worker thread with name "<<name<<'\n';
	biguint counter = 0;

	for(int i = 0; i < 255; i++){
		g_mylg.writeToLog(name+" log entry "+std::to_string(counter++));
	}
	cout<<"-----exited worker thread with name "<<name<<'\n';
}




int main(int argc, char** argv)
{
	exitthread = false;
	cout<<argv[0]<<'\n';
	std::thread t1(worker,"Thread_1");
	//std::thread t2(worker,"Thread_2"); 
	//std::thread t3(worker,"Thread_3");
	//AESleep mysl;
	//mysl.sleep(3);
	exitthread = true;
	
	
	
	cout<<"Press enter to continue . . .";

	
	std::cin.get();
	t1.join();
	//t2.join();
	//t3.join();
	return 0;
}
