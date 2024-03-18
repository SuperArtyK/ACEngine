
/*! @file AEFrame/include/AEFrame.hpp
 *
 *	@brief This file contains the engine's **module for frame-precise loop delays**.
 *
 *	This is usefull when you need to limit the execution of the program in the loop.
 *
 *	Limit the loop cycle execution to precisely set amount (let's say 60 times per second),
 *	if the contents of the loop are processed faster than you need.
 *
 *	Example of usage (and reason for the name): game loop, that you need to limit to some fps.
 *	
 *	@date 2023/09/21
 *
 *	@author	Artemii Kozhemiak (SuperArtyK)
 *
 *	@copyright MIT License
 *
 *  *Should not cause everything to break :)*
 */


#pragma once

#ifndef ENGINE_FRAME_HPP
#define ENGINE_FRAME_HPP

#include "include/AEFlags.hpp"
#include "include/AEModuleBase.hpp"

// in case you, the programmer undefined it
#ifndef ENGINE_FPS
#error Hey you! You undefined the ENGINE_FPS and want to use the AEFrame module(or something that depends on it). If you wanted to disable the default delay functionality, set the ENGINE_FPS to 0 or negative instead!
#endif // !ENGINE_FPS


/// <summary>
/// This module is the engine's **precise frame/loop-cycle limiter**.
///
/// It allows to limit the execution of a loop to a set amount of iterations per second. <br>
/// Or, instead have the set delay per each loop cycle.
/// 
/// It works by sleeping the difference between the time spent by code in the loop and the time per each frame.
/// 
/// For example: <br>
///		If you want the loop to have 100 cycles/second or fps (10 milliseconds per cycle), 
///		and the code takes **4ms** to complete, then the AEFrame will sleep the difference. <br>
///		In this case it's **6ms** to make it up to the 10ms per cycle.
///	
///	@note By default it uses the value of #ENGINE_FPS as the fps target for the loops
/// </summary>
class AEFrame : public __AEModuleBase<AEFrame> {

public:

	/// This is an an alias for the internal clock of the AEFrame module.
	/// @warning The resulution of the AEFrameClock that your compiler gives
	///		must be at least 1 microsecond, otherwise it won't compile (breaks assertion).
	using AEFrameClock = AEF_CLOCK;

//constructors
	/// <summary>
	/// Class constructor.
	/// 
	/// @attention If you pass it negative, 0, or more than 1 million, it will disable the
	///		delay (sets AEFrame::m_bNoDelay to **true**)
	/// @note Defaults the fps delay to #ENGINE_FPS
	/// </summary>
	/// <param name="fps">The fps goal that the frame delay module will try to achieve</param>
	explicit AEFrame(const double fps = ENGINE_FPS) noexcept :
		m_tdChronoDelay((fps <= 0 || fps > 1000000.0) ? microSec(0) : microSec(llint(1000000.0 / fps))),
		m_tpTimepoint(AEFrameClock::now()), m_fFPS(fps), m_bNoDelay((fps <= 0 || fps > 1000000.0))
		{}

	/// <summary>
	/// Copy constructor -- copies data from a passed AEFrame instance.
	/// </summary>
	/// <param name="two">The second AEFrame instance</param>
	AEFrame(const AEFrame& two) noexcept :
		//COPYCONSTRUCT_MODULE(AEFrame, two),
		m_tdChronoDelay(two.m_tdChronoDelay),
		m_tpTimepoint(AEFrameClock::now()), m_fFPS(two.m_fFPS), m_bNoDelay(two.m_bNoDelay)
		{}

	/// <summary>
	/// Copy-assignment operator -- copies data from a passed AEFrame instance.
	/// 
	/// @note This resets the internal time point (calls AEFrame::resetTimePoint())
	/// </summary>
	/// <param name="two">The second AEFrame instance</param>
	/// <returns>
	///		The reference to the resulting AEFrame instance.
	///	</returns>
	AEFrame& operator=(const AEFrame& two) noexcept {
		this->m_tdChronoDelay = two.m_tdChronoDelay;
		this->m_tpTimepoint = AEFrameClock::now();
		this->m_fFPS = two.m_fFPS;
		this->m_bNoDelay = two.m_bNoDelay;
		this->resetTimePoint();

		return *this;
	}


//utils
	/// <summary>
	/// Sets the delay of the instance as the frames (loop cycles) per second to reach in execution (fps goal).
	/// 
	/// @attention If you pass it negative, 0, or more than 1 million, it will disable the
	///		delay (sets AEFrame::m_bNoDelay to **true**)
	/// @note This resets the internal time point (calls AEFrame::resetTimePoint())
	/// </summary>
	/// <param name="fps">The frames(cycles) per second to set the delay as</param>
	inline void setFps(const double fps) noexcept {
		if (fps <= 0.0 || fps > 1000000.0) { this->m_bNoDelay = true; return; }
		this->m_tdChronoDelay = microSec(llint(1000000.0 / fps));
		this->m_tpTimepoint = AEFrameClock::now();
		this->m_fFPS = fps;
		this->m_bNoDelay = false;
	}

	/// <summary>
	/// Sets the delay of the instance as maximum time for each loop cycle.
	/// 
	/// @attention If you pass negative or 0, it will disable the delay (sets AEFrame::m_bNoDelay to **true**)
	/// </summary>
	/// <param name="sec">The delay of instance/time for each loop cycle</param>
	inline void setDelay(const double sec) {
		if (sec <= 0.0) { this->m_bNoDelay = true; return; }
		this->m_tdChronoDelay = microSec(llint(1000000.0 * sec));
		this->m_tpTimepoint = AEFrameClock::now();
		this->m_fFPS = 1.0 / sec;
		this->m_bNoDelay = false;
	}

	/// <summary>
	/// Puts the current thread for the delay that was set earlier.
	/// 
	/// Example: if the fps delay was set to 30fps (~~33ms delay in real time) and a game
	///		loop takes just 13ms, then the thread will sleep the rest of 20ms to make the
	///		game loop run at around ~~33ms of delay or around 30fps.
	///	
	/// @attention If the delay is disabled (AEFrame::m_bNoDelay is **true**), just returns and doesn't do anything
	/// @note If the AEFrame::m_tpTimepoint gets too out of touch with real time, resets the time point (calls AEFrame::resetTimePoint())
	///		The definition of "out of touch" is controlled by #AEF_RESET_TIME_WHEN_BEHIND_MS.
	///	@note Automatic timepoint resetting works **only** if #AEF_RESET_TIME_WHEN_BEHIND_MS is **enabled**
	/// </summary>
	inline void sleep(void) noexcept {
		// add to time point
		if (this->m_bNoDelay) {
			return;
		}
#if AEF_RESET_TIME_WHEN_BEHIND_MS > 0
		if ((AEFrameClock::now() - this->m_tpTimepoint) > m_tdResetTime) {
			this->resetTimePoint();
		}
#endif 

		this->m_tpTimepoint += this->m_tdChronoDelay;
		// and sleep until that time point
		sleepUntil(this->m_tpTimepoint);
	}

	/// <summary>
	/// Resets the time point in the AEFrame to the real (current) time.
	/// Helps if the AEFrame::m_tpTimepoint time point drifted far from the reality.
	/// </summary>
	inline void resetTimePoint(void) noexcept {
		this->m_tpTimepoint = AEFrameClock::now();
	}

	/// <summary>
	/// Returns the fps goal of AEFrame instance.
	/// </summary>
	/// <returns>
	///		The fps goal of the instance as type **double**.
	/// </returns>
	inline double getFrameRate(void) const noexcept {
		return this->m_fFPS; 
	}

	/// <summary>
	/// Returns the maximum AEFrame's delay from the set fps, in seconds.
	/// 
	/// Or better: the time, in seconds, for each loop cycle that AEFrame::sleep() is called
	/// </summary>
	/// <returns>
	///		The delay of the instance as seconds as type **double**.
	/// </returns>
	inline double getDelay(void) const noexcept {
		return this->m_tdChronoDelay.count(); 
	}

private:

	static_assert(
		(std::chrono::duration_cast<std::chrono::microseconds>(typename AEFrameClock::duration(1)).count()) <= 1,
		"Your C++ compiler doesn't provide a precise enough steady_clock."
	);


	/// The time duration for resetting the AEFrame::m_tpTimepoint if it gets too different with real time.
	/// Only exists if #AEF_RESET_TIME_WHEN_BEHIND_MS is defined
	/// @see #AEF_RESET_TIME_WHEN_BEHIND_MS
#if AEF_RESET_TIME_WHEN_BEHIND_MS > 0
	static constexpr timeDur<double> m_tdResetTime = microSec(milliSec(llint(AEF_RESET_TIME_WHEN_BEHIND_MS)));
#endif

	/// The time duration for the sleeping delay
	timeDur<double> m_tdChronoDelay;
	/// The timepoint, that sets time when to wake up the thread.
	timePoint<AEFrameClock, decltype(m_tdChronoDelay)> m_tpTimepoint;
	double m_fFPS;
	/// The flag if we don't need the delay, err, for some reason.
	bool m_bNoDelay;

	//aaand we have to register it too
	REGISTER_MODULE(AEFrame);
};

#endif // !ENGINE_FRAME_HPP
