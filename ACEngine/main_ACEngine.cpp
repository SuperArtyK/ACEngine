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

#include <cstdlib>
#include <ctime>


int main() {
	
	std::srand(std::time(NULL));
	AEFrame myfr(60);
	AEFileWriter myfw("acenginetest.txt", AEFW_FLAG_TRUNCATE, 1);
	AETimer mytm(10000);
	int a = 0;

	timePoint<HighResTime> tp1;
	while (1) {
		tp1 = getHighResTime();
		a = std::rand();
		std::cout << "Hello ACEngine! ";
		myfw.writeString("Hello AEFileWriter!\n");
		myfw.flushFile();
		std::cout << ace::math::cosDeg<float>(a) << " " << ace::math::cosDeg_table(a)<<NLC;
		std::cout << "The timer tick is: " << mytm.getTick() << NLC;
		std::cout << "The timer's world time is: " << mytm.getWorldTime() << " seconds" << NLC;
		std::cout << "The time it took to run the loop is: ";
		std::cout << calculateTime(tp1, getHighResTime(), float)<<NLC;
		myfr.sleep();
	}
	myfw.closeFile();
	
}