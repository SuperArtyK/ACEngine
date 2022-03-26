
#pragma once

#ifndef AESLEEP_HPP
#define AESLEEP_HPP

#include <thread>
#include <chrono>
#include "typedefs.hpp"
#include "AEModuleBase.hpp"

/// \brief This module is used to create custom but accurate delays, unlike AEFrame that has only fixed delay(unless called setfps() to change it).
/// This module is not inherited from the base class, as it needs to be very fast, faster than the base class even,
/// and without much of dependencies.
class AESleep : __AEModuleBase<AESleep> {
public:
	/// <summary>
	/// Class constructor
	/// </summary>
	AESleep() : tp(getsystime())
	{}

	/// sleeps for given number of seconds(default is 1.0/GAME_FPS)
	void sleep(const double timems = 1.0 / GAME_FPS) {
		// add to time point
		tp = getsystime();
		tp+= tduration<double>(millisec(biguint(timems * 1000)));
		// and sleep until that time point
		sleepuntil(tp);
	}
private:
	///the timepoint, that sets time when to wake up the thread
	timepoint<systime, tduration<double>> tp;

};

ADD_MODULE_TO_ENGINE(AESleep)

#endif // !AESLEEP_HPP