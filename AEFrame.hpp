/*
	ArtyK's Console (Game) Engine. Console engine for apps and games
	Copyright (C) 2021  Artemii Kozhemiak

	https://github.com/SuperArtyK/artyks-engine

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License 3.0 as published
	by the Free Software Foundation.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License 3.0 for more details.

	You should have received a copy of the GNU General Public License 3.0
	along with this program.  If not, see <https://www.gnu.org/licenses/gpl-3.0.txt>.
*/

/** @file AEFrame.hpp
 *  Modified version of ArtyK's Console (Game) Engine's AEFrame module
 *  to fit the delay task here
 *  Original description goes below
 *  -----------------------------------------------
 *  This file contains the frame delay module code.
 *  Useful if you need to limit the execution time of the program. It makes
 *  sure that the loop(f.ex.) executes with same delay, if contents of loop
 *  processed faster than you need.
 *
 *  Should not cause everything to break.
 */

#pragma once

#ifndef AEFRAME_HPP
#define AEFRAME_HPP

#include <thread>
#include <chrono>
#include "typedefs.hpp"

#define GAME_FPS 66

 //usage:
 // create obj AEFrame with fps value
 // AEFrame fr(30); -- creates frame_rater fr with 30fps delays
 //
 // after your stuff is done, call frame_rater::sleep();
 //	while(true) {
 //		std::cout << "Hello world\n";
 //		fr.sleep();
 //	}
 // it outputs "Hello world" and waits the rest of time of 30fps delay(cout delay+rest = 0.0333...)
 //
 // also you can change framerate on the fly with AEFrame::setfps(float myfps)

 /// \brief This module is used for creating delays(in FPS format) in the engine.
 /// It is great if used in loops, as it makes them have delay of AEFrame, if body executes fasted than the delay time.
 /// This module is not inherited from the base class, as it needs to be very fast, faster than the base class even,
 /// and without much of dependencies.
class AEFrame {
public:

	/// <summary>
	/// Class constructor
	/// </summary>
	/// <param name="fps">the delay in the format of framerate, defaults to the GAME_FPS</param>
	/// @note if you pass it 0 or negative number it will disable the delay
	explicit AEFrame(const float fps = GAME_FPS) : time_between_frames((fps <= 0) ? microsec(0) : microsec(biguint(1000000 / fps))),
		tp(getsystime), nodelay((fps <= 0))
	{
	}

	///resets the delay value to what you pass to it
	/// @note if you pass it 0 or negative number it will disable the delay
	/// @see AEFrame()
	void setfps(const float fps = GAME_FPS) {
		if (fps <= 0) { nodelay = true; return; }
		time_between_frames = microsec(biguint(1000000 / fps));
		tp = getsystime;
	}

	///makes the current thread sleep for a set delay
	void sleep(void) {
		// add to time point
		if (nodelay) {
			return;
		}
		tp += time_between_frames;
		// and sleep until that time point
		sleepuntil(tp);
	}

	///resets the time point time to current system time
	void resettime() {
		tp = getsystime;
	}
	///returns framerate of AEFrame
	inline int getframerate(void) const { return int((1.0 / time_between_frames.count())+0.5); }
	///returns the delay of AEFrame in seconds
	inline double getdelay(void) const { return time_between_frames.count(); }
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
	tduration<double> time_between_frames;
	///the timepoint, that sets time when to wake up the thread
	timepoint<std::chrono::system_clock, decltype(time_between_frames)> tp;
	///flag if we don't need the delay
	bool nodelay;
};



#endif // !AEFRAME_HPP