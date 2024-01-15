
#include "AELogger.hpp"
#include "include/AEFrame.hpp"
#include "include/AEUtils.hpp"
#include <iostream>
using std::cout;


AELogger mylog("log.txt", true);

constexpr int fpsval = 10000;

std::atomic<bool> stoptrd = 0;

void logfiller(const std::string name) {


	AEFrame a(fpsval);
	std::cout << "Entered logfiller with a thread" << NLC;
	for (int i = 0; !stoptrd; i++) {
		mylog.writeToLog("Hello " + std::string(__FUNCTION__) + "! (log entry " + std::to_string(i + 1) + ")", AELOG_TYPE_INFO, __FUNCTION__);
		mylog.writeToLogDebug("Working in the " + std::string(__FUNCTION__), __FUNCTION__);
		mylog.writeToLog("Something isn't right in " + std::string(__FUNCTION__), AELOG_TYPE_WARN, __FUNCTION__);
		mylog.writeToLog("Something is very not good in " + std::string(__FUNCTION__), AELOG_TYPE_SEVERE_WARN, __FUNCTION__);
		mylog.writeToLog("Started writing to log in" + std::string(__FUNCTION__), AELOG_TYPE_OK, __FUNCTION__);
		mylog.writeToLog("Success, we wrote to log in" + std::string(__FUNCTION__) + "!", AELOG_TYPE_SUCCESS, __FUNCTION__);
		mylog.writeToLog("Something is wrong in " + std::string(__FUNCTION__), AELOG_TYPE_ERROR, __FUNCTION__);
		mylog.writeToLog("Something is very very wrong in " + std::string(__FUNCTION__), AELOG_TYPE_FATAL_ERROR, __FUNCTION__);
		a.sleep();
	}


}


void oldmain() {
	std::cout << sizeof(AELogEntry) << "; " << sizeof(AELogEntry) * 65535 << NLC;

	//globalLogger.writeStatus();

	std::thread t1(&logfiller, "logfil1");


	std::thread t2(&logfiller, "logfil2");
	AEFrame a(fpsval);
	for (int i = 0; ; i++) {
		mylog.writeToLog("Hello " + std::string(__FUNCTION__) + "! (log entry " + std::to_string(i + 1) + ")", AELOG_TYPE_INFO, __FUNCTION__);
		mylog.writeToLogDebug("Working in the " + std::string(__FUNCTION__), __FUNCTION__);
		mylog.writeToLog("Something isn't right in " + std::string(__FUNCTION__), AELOG_TYPE_WARN, __FUNCTION__);
		mylog.writeToLog("Something is very not good in " + std::string(__FUNCTION__), AELOG_TYPE_SEVERE_WARN, __FUNCTION__);
		mylog.writeToLog("Started writing to log in " + std::string(__FUNCTION__), AELOG_TYPE_OK, __FUNCTION__);
		mylog.writeToLog("Success, we wrote to log in " + std::string(__FUNCTION__) + "!", AELOG_TYPE_SUCCESS, __FUNCTION__);
		mylog.writeToLog("Something is wrong in " + std::string(__FUNCTION__), AELOG_TYPE_ERROR, __FUNCTION__);
		mylog.writeToLog("Something is very very wrong in " + std::string(__FUNCTION__), AELOG_TYPE_FATAL_ERROR, __FUNCTION__);

		a.sleep();
	}


	//mylog.parseEntries();
	t1.join();
	t2.join();
	std::cin.get();
}

struct entrydata {
	char str[64]{};
	entrydata(const std::string_view cstr) {
		std::memcpy(str, cstr.data(), 31);
	}
};

struct node {
	entrydata nodedata;
	std::atomic<node*> next;

	node(const entrydata& data) : nodedata(data), next(nullptr) {}
};

//MPSCQueue 
class Queue {

public:

	Queue() {

	}

	void push(const entrydata& newData) {
		node* const newNode = new node(newData);

		
		node* nptr = nullptr;
		
		
		node* oldTail = this->tail.load();
		
		if (!qsize) {
			if (!tail.compare_exchange_strong(nptr, newNode)) { goto normalpush; }
			head.compare_exchange_strong(nptr = nullptr, newNode);
		}
		else {

			normalpush:
			//oldTail = this->tail.load();
			while (!oldTail->next.compare_exchange_weak(nptr = nullptr, newNode)) {
				oldTail = this->tail.load();
				//nptr = nullptr;
			}

			if (!tail.compare_exchange_strong(oldTail, newNode)) {
				abort();
			}


			if (!qsize) {
				head.compare_exchange_strong(nptr, tail);
			}
		}

		
		
		fexit:
		qsize++;
	}

	node* pop() {
		if (qsize == 0) {
			return nullptr;
		}
		qsize--;
		node* oldHead = this->head.load();
		
		while (oldHead &&
			!head.compare_exchange_weak(oldHead, oldHead->next)) {
			oldHead = head.load();
			
		}

		return oldHead;
	}

	std::size_t getSize() const {
		return this->qsize.load();
	}

private:

	std::atomic<node*> head = nullptr;
	std::atomic<node*> tail = nullptr;
	std::atomic<std::size_t> qsize = 0;


};



std::atomic<ullint> amtwritten = 0;


void readqueue(Queue& myq) {
	node* nptr;
	AEFrame myfr(10);

	AEFileWriter myfw("test.txt", AEFW_FLAG_TRUNCATE, 1);

	while (!stoptrd || myq.getSize() > 0) {
		nptr = myq.pop();
		if (nptr) {
			//cout << nptr->nodedata.str << NLC;
			myfw.writeString(nptr->nodedata.str);
			myfw.flushFile();
			if (mod0(amtwritten++, 10000)) {
				cout << amtwritten << NLC;
			}
			delete nptr;
		}
		else {
			if (myq.getSize() != 0) {
				cout << "ERROR!\n\n\n\n";
			}
			else {
				cout << "Queue is empty, waiting 10ms...\n";
				myfr.sleep();
			}
		}
	}
}


void writequeue(Queue& myq) {
	AEFrame myfr(10000);
	ullint i = 0;
	while (!stoptrd) {
		myq.push({ "\ntest line" + std::to_string(++i) });
		myfr.sleep();
	}


}


int main()
{

	
	
	Queue myq;
// 	std::thread t1(&readqueue, std::ref(myq));
// 	std::thread t2(&writequeue, std::ref(myq));
	
	std::thread t1(&logfiller, "log1");
	std::thread t2(&logfiller, "log2");


	ace::utils::sleepMS(5000);

	stoptrd = true;

	t1.join();
	t2.join();
	return 0;
}


