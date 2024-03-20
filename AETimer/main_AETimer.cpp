
#include <iostream>
#include "AETimer.hpp"
#include "include/AEUtils.hpp"

int main() {

	AETimer a(5);

	AETimer mytm(a);

	std::cout << mytm.getModuleAmount() << " " << mytm.getModuleName() << NLC;

	//AETimer::waitForTick(globalTimer)


	while (1) {
		AETimer::waitTicks(ace::globalTimer, 1);
		std::cout << "Seconds passed: " << ace::globalTimer.getWorldTime() << "; Ticks passed: "<< ace::globalTimer.getTick()<<NLC;
	}



}
