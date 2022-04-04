
#pragma once

#ifndef AEFRAME_HPP
#define AEFRAME_HPP

//code for AEFrame module
//frame delay
#include <thread>
#include "typedefs.hpp"
#include "global_vars.hpp"
#include "AEModuleBase.hpp"

 /// \brief This module is used for creating delays(in FPS format) in the engine.
 /// It is great if used in loops, as it makes them have delay of AEFrame, if body executes fasted than the delay time.
 /// This module is not inherited from the base class, as it needs to be very fast, faster than the base class even,
 /// and without much of dependencies.
 /// Hungarian notation is fr
class AEFrame : public __AEBasicModule<AEFrame>{
public:

	/// <summary>
	/// Class constructor
	/// </summary>
	/// <param name="fps">the delay in the format of framerate, defaults to the GAME_FPS</param>
	/// @note if you pass it 0 or negative number it will disable the delay
	explicit AEFrame(const float fps = GAME_FPS) :
		m_durDelay((fps <= 0) ? microsec(0) : millisec(biguint(1000.0f/fps))),
		m_tp(getsystime()), m_bNoDelay((fps <= 0))
	{
	}

	///resets the delay value to what you pass to it
	/// @note if you pass it 0 or negative number it will disable the delay
	/// @see AEFrame()
	void setfps(const float fps = GAME_FPS) {
		if (fps <= 0) { m_bNoDelay = true; return; }
		m_durDelay = microsec(biguint(1000000 / fps));
		m_tp = getsystime();
	}

	///makes the current thread sleep for a set delay
	void sleep(void) {
		// add to time point
		if (m_bNoDelay) {
			return;
		}
		m_tp += m_durDelay;
		// and sleep until that time point
		sleepuntil(m_tp);
	}

	///resets the time point time to current system time
	void resettime() {
		m_tp = getsystime();
	}
	///returns framerate of AEFrame
	inline int getframerate(void) const { return int((1.0 / m_durDelay.count())+0.5); }
	///returns the delay of AEFrame in seconds
	inline double getdelay(void) const { return m_durDelay.count(); }
#ifdef AE_EXPERIMENTAL
	/// uses all utils for class
	///@see similar thing as __AEBaseClass::benchmark()
	void benchmark() {
		setfps(getframerate());
		getdelay();
	}
#endif

private:

	///delay between seconds
	tduration<double> m_durDelay;
	///the timepoint, that sets time when to wake up the thread
	timepoint<systime, decltype(m_durDelay)> m_tp;
	///flag if we don't need the delay
	bool m_bNoDelay;
};

ADD_MODULE_TO_ENGINE(AEFrame)

#endif // !AEFRAME_HPP
