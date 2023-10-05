
#include <iostream>
#include "AETimer.hpp"
#include "include/AEUtils.hpp"

int main() {

	AETimer a(5);

	AETimer mytm(a);

	std::cout << mytm.getModuleAmount() << " " << mytm.getModuleName() << NLC;
	while (1) {
		AETimer::waitTicks(globalTimer, 1);
		std::cout << "Seconds passed: " << globalTimer.getWorldTime() << NLC<<"Ticks passed: "<< globalTimer.getTick();
	}



}
