
#include <iostream>
#include "AEFrame.hpp"
#include "include/AETypedefs.hpp"
#include <thread>
using std::cout;
using std::endl;
using std::cin;

std::atomic<ullint> i = 0;

std::atomic<bool> stopTrd = true;

int worker() noexcept {
	AEFrame myfr;
	myfr.setFps(10);
	cout << "id: " << myfr.getInstanceCounter() << NLC;
	while (stopTrd) {
		//tp1 = getSteadyTime();
		//a = time(nullptr);
		i++;
		myfr.sleep();
	}

	//return timeBetween(tp1, getSteadyTime(), float);
	return 1;


}



int main()
{
	cout << "sizeof timeDur<double> " << sizeof(timeDur<double>)<<NLC;
	cout << "sizeof timePoint<AEFrameClock, decltype(m_tdChronoDelay)>" << sizeof(timePoint<AEFrame::AEFrameClock, timeDur<double>>) << NLC;

	AEFrame myfr;
	myfr.setFps(1);

	std::thread mytrd(worker);

	

	ullint previ = i;
	ullint curi = i;
	while (1) {
		curi = i;
		std::printf("FPS: %llu;\n", curi - previ);
		previ = curi;
		myfr.sleep();
	}
	stopTrd = false;

	mytrd.join();

}
