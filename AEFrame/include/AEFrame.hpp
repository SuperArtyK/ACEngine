#pragma once

#ifndef ENGINE_FRAME_HPP
#define ENGINE_FRAME_HPP

#include "include/AETypedefs.hpp"
#include "include/AEModuleBase.hpp"

/// <summary>
/// This is the Engine's frame delay module. It allows to create precise delays for game loops.
/// So instead of guessing the delay for the main game loop, and having unrealiable call to normal "sleep for x",
/// you can always rely on this to delay/sleep the thread to make the game loop run at same time to achieve the fps that you set earlier.
/// @note To get this to work properly, make an object outside of the loop, and put the call to AEFrame::sleep() at the end of the loop.
/// @note This works by looking at the time spent by the game loop and sleeping up to the time to meet the fps goal.
/// @warn If the game loop takes more time than the set fps goal amount, the frame times can be unpredictable (and...delay might also not work)
/// </summary>
class AEFrame : public __AEModuleBase<AEFrame> {
public:

	/// <summary>
	/// Class constructor
	/// @note if you pass it 0 or negative number it will disable the delay untill a proper value is given
	/// </summary>
	/// <param name="fps">The fps goal that the frame delay module will try to achieve</param>
	explicit AEFrame(const double fps) : time_between_frames((fps <= 0) ? microSec(0) : microSec(ullint(1000000.0 / fps))),
		tp(getSteadyTime()), nodelay((fps <= 0))
	{
	}

	/// <summary>
	/// Resets the delay value to what you pass to it
	/// @note if you pass it 0 or negative number it will disable the delay untill a proper value is given
	/// </summary>
	/// <param name="fps">The fps goal that the frame delay module will try to achieve</param>
	void setFps(const double fps) {
		if (fps <= 0) { nodelay = true; return; }
		time_between_frames = microSec(ullint(1000000.0 / fps));
		tp = getSteadyTime();
	}

	///makes the current thread sleep for a set delay

	/// <summary>
	/// Puts the current thread to sleep up to the set fps goal
	/// @example If the fps goal was set to 30fps (~~33ms) and a game loop takes 13ms, then the thread will sleep the rest of 20ms to make the game loop run at 30fps.
	/// </summary>
	void sleep(void) {
		// add to time point
		if (nodelay) {
			return;
		}

		tp += time_between_frames;
		// and sleep until that time point
		sleepUntil(tp);
	}

	/// <summary>
	/// Resets the time point in the AEFrame to current system time. 
	/// Helps if the gameloop delay was much bigger than AEFrames for a long time (it causes delay to stop working untill it catches up)
	/// </summary>
	void resetTime(void) {
		tp = getSteadyTime();
	}

	/// <summary>
	/// Returns the fps goal of AEFrame instance
	/// </summary>
	/// <returns>Rounded int of the approximated fps goal</returns>
	inline int getFrameRate(void) const { return 1.0 / time_between_frames.count(); }

	///returns the delay of AEFrame in seconds

	/// <summary>
	/// Returns the absolute delay of AEFrame instance (assuming 0 gameloop delay)
	/// </summary>
	/// <returns>double of the absolute AEFrame instance's delay</returns>
	inline double getDelay(void) const { return time_between_frames.count(); }

private:

	///delay between seconds	
	timeDur<double> time_between_frames;
	///the timepoint, that sets time when to wake up the thread
	timePoint<SteadyTime, decltype(time_between_frames)> tp;

	///flag if we don't need the delay
	bool nodelay;
};

REGISTER_CLASS(AEFrame)

#endif // !ENGINE_FRAME_HPP
