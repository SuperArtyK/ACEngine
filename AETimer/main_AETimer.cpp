
#include <iostream>
#include "AETimer.hpp"
#include "include/AEUtils.hpp"

int main() {

	AETimer mytm(1);
	std::cout << sizeof(__AEModuleBase<AEFrame>) << NLC << sizeof(AEFrame) << NLC << sizeof(__AEModuleBase<AETimer>) << NLC << sizeof(AETimer) << NLC;
	while (1) {
		AETimer::waitTicks(mytm, 1);
		std::cout << "Seconds passed: " << mytm.getWorldTime() << NLC;
	}



}
