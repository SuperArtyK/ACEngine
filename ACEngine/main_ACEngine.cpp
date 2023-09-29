#include <iostream>
//aefw
#include "include/AEFileWriter.hpp"
//aemb
#include "include/AEModuleBase.hpp"
//aemisc
#include "include/AETypedefs.hpp"
#include "include/AEFlags.hpp"
#include "include/AEMath.hpp"
#include "include/AEUtils.hpp"
#include "include/AETrigLookup.hpp"
//aelg
#include "include/AELogger.hpp"
//aefr
#include "include/AEFrame.hpp"
//aetm
#include "include/AETimer.hpp"
//aevec
#include "include/AEVector.hpp"

#include <cstdlib>
#include <ctime>


int main() {
	
	std::srand(std::time(NULL));
	AEFrame myfr;
	AEFileWriter myfw("acenginetest.txt", AEFW_FLAG_TRUNCATE, 1);
	AETimer mytm(10000);
	AEVector<std::size_t, 32> veca;
	for (int i = 0; i < arrsize(veca.dims); i++) {
		veca[i] = std::rand();
	}

	AELogger mylog("engine_log.txt", true);
	int a = 0;


	std::string tempstr;
	timePoint<HighResTime> tp1;
	while (1) {
		tp1 = getHighResTime();
		
		a = std::rand();
		tempstr.reserve(400);
		tempstr += "Hello ACEngine! ";
		myfw.writeString("Hello AEFileWriter!\n");
		myfw.flushFile();
		tempstr += std::to_string(ace::math::cosDeg<float>(a)) + std::string(" ") + std::to_string(ace::math::cosDeg_table(a))+NLC;
		tempstr += std::string("The timer tick is: ") + std::to_string(mytm.getTick()) + NLC;
		tempstr += std::string("The timer's world time is: ");
		tempstr += std::to_string(mytm.getWorldTime()) + std::string(" seconds") + NLC;
		tempstr += std::string("The vector is: ") + veca.toString() + NLC;
		tempstr += std::string("The magnitude of that vector is: ") + std::to_string(veca.magnitude()) + NLC;
		std::cout << tempstr;
		myfw.writeString(tempstr);
		tempstr.clear();
		myfw.flushFile();
		mylog.writeStatus();
		mylog.writeToLog("Successfully have gone through a loop!");

		std::cout << "The time it took to run the loop is: ";
		std::cout << calculateTime(tp1, getHighResTime(), float)<<NLC;

		
		
		myfr.sleep();
	}
	myfw.closeFile();
	
}