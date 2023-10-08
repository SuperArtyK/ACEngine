
/** @file AEFrame/include/AEFrame.hpp
 *  This file contains the frame delay module code.
 *  Useful if you need to limit the execution time of the program. It makes
 *  sure that the loop executes with same delay, if contents of loop are
 *  processed faster than you need.
 *
 *  Should not cause everything to break :)
 */

#pragma once

#ifndef ENGINE_FRAME_HPP
#define ENGINE_FRAME_HPP

#include "include/AETypedefs.hpp"
#include "include/AEModuleBase.hpp"
#include "include/AEFlags.hpp"

/// <summary>
/// @brief This is the ArtyK's Engine's frame delay module -- it allows to create precise delays for game loops.
/// Hungarian notation is r
/// So instead of guessing the delay for the main game loop, and having unreliable call to normal "sleep for x",
/// you can always rely on this to delay/sleep the thread to make the game loop run at same time to achieve the fps that you set earlier.
/// @note To get this to work properly, make an object outside of the loop, and put the call to AEFrame::sleep() at the end of the loop.
/// @note This works by looking at the time spent by the game loop and sleeping up to the time to meet the fps goal.
/// @warning If the game loop takes more time than the set fps goal amount, the frame times can be unpredictable (and...delay might also not work)
/// </summary>
class AEFrame : public __AEModuleBase<AEFrame> {

public:
//constructors
	/// <summary>
	/// Class constructor
	/// @note if you pass it 0 or negative number it will disable the delay untill a proper value is given
	/// </summary>
	/// <param name="fps">The fps goal that the frame delay module will try to achieve</param>
	explicit AEFrame(const double fps = ENGINE_FPS) noexcept : m_tdChronoDelay((fps <= 0) ? microSec(0) : microSec(ullint(1000000.0 / fps))),
		m_tpTimepoint(getSteadyTime()), m_fFPS(fps), m_bNoDelay((fps <= 0))
	{}

//utils
	/// <summary>
	/// Resets the delay value to what you pass to it
	/// @note if you pass it 0 or negative number it will disable the delay untill a proper value is given
	/// </summary>
	/// <param name="fps">The fps goal that the frame delay module will try to achieve</param>
	inline void setFps(const double fps) noexcept {
		if (fps <= 0) { this->m_bNoDelay = true; return; }
		this->m_tdChronoDelay = microSec(ullint(1000000.0 / fps));
		this->m_tpTimepoint = getSteadyTime();
		this->m_fFPS = fps;
	}
	
	/// <summary>
	/// Puts the current thread to sleep up to the set fps goal
	/// Example: if the fps goal was set to 30fps (~~33ms) and a game loop takes 13ms, 
	/// then the thread will sleep the rest of 20ms to make the game loop run at 30fps.
	/// </summary>
	inline void sleep(void) {
		// add to time point
		if (this->m_bNoDelay) {
			return;
		}

		this->m_tpTimepoint += this->m_tdChronoDelay;
		// and sleep until that time point
		sleepUntil(this->m_tpTimepoint);
	}

	/// <summary>
	/// Resets the time point in the AEFrame to current system time. 
	/// Helps if the gameloop delay was much bigger than AEFrames for a long time (it causes delay to stop working untill it catches up)
	/// </summary>
	inline void resetTimePoint(void) noexcept {
		this->m_tpTimepoint = getSteadyTime();
	}

	/// <summary>
	/// Returns the fps goal of AEFrame instance
	/// </summary>
	/// <returns>double of the given fps goal</returns>
	inline double getFrameRate(void) const noexcept { return this->m_fFPS; }

	/// <summary>
	/// Returns the maximum AEFrame's delay from the set fps, in seconds
	/// </summary>
	/// <returns>double of the maximum AEFrame's delay in real-world seconds</returns>
	inline double getDelay(void) const noexcept { return this->m_tdChronoDelay.count(); }

private:

	/// The time duration for the delay in sleeps	
	timeDur<double> m_tdChronoDelay;
	/// The timepoint, that sets time when to wake up the thread
	timePoint<SteadyTime, decltype(m_tdChronoDelay)> m_tpTimepoint; 
	double m_fFPS;
	/// The flag if we don't need the delay, err, for some reason
	bool m_bNoDelay;
};

REGISTER_CLASS(AEFrame)

#endif // !ENGINE_FRAME_HPP
