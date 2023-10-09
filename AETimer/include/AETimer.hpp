
/** @file AETimer/include/AETimer.hpp
 *  This file contains the code for engine's timer. You can use it to time events.
 *  Or like...wait for the tick of the timer for a certain delay or even to happen.
 *  
 *  Should not cause everything to break :)
 */

#pragma once

#ifndef ENGINE_TIMER_HPP
#define ENGINE_TIMER_HPP

#include "include/AEFrame.hpp"
#include "include/AETypedefs.hpp"
#include "include/AEModuleBase.hpp"
#include "include/AEFlags.hpp"
#include <thread>
#include <atomic>


/// <summary>
/// @brief ArtyK's Engine Timer -- the engine's internal timer for, um, timing events or anything, with variable tick speed.
/// Just create it and look at the ticks of the timer from anywhere in the program...that you can access it.
/// Hungarian notation is tm
/// </summary>
class AETimer : public __AEModuleBase<AETimer> {

public:

//constructors
	/// <summary>
	/// Class constructor -- starts the timer automatically
	/// </summary>
	/// <param name="tps">The amount of ticks per second, as the float</param>
	explicit AETimer(const double tps = ENGINE_FPS) :
		m_rDelay(tps), m_ullTicks(0), m_bRunTrd(true) {
		this->startThread();
	}

	/// <summary>
	/// Class constructor -- just assigns default delay (ENGINE_FPS) and doesn't start the thread
	/// </summary>
	AETimer() noexcept : m_rDelay(ENGINE_FPS), m_ullTicks(0), m_bRunTrd(false) {}

	/// <summary>
	/// Copy constructor -- Just copies the data and then starts the timer if original timer was started.
	/// </summary>
	/// <param name="tm">The original AETimer instance to make a copy of</param>
	AETimer(const AETimer& tm) :
		m_rDelay(tm.m_rDelay), m_ullTicks(tm.m_ullTicks.load()), m_bRunTrd(tm.m_bRunTrd.load()) {

		if (this->m_bRunTrd) {
			this->startThread();
		}
	}

	/// <summary>
	/// The copy assignment operator. Just copies the data and then starts the timer if original timer was started.
	/// </summary>
	/// <param name="tm">The original AETimer instance to make a copy of</param>
	/// <returns>Reference to the resulting AETimer copy</returns>
	AETimer& operator=(const AETimer& tm) {
		this->m_rDelay = tm.m_rDelay;
		this->m_ullTicks = tm.m_ullTicks.load();
		this->m_bRunTrd = tm.m_bRunTrd.load();
		if (this->m_bRunTrd) {
			this->startThread();
		}
		return *this;
	}

	/// <summary>
	/// Class destructor
	/// </summary>
	~AETimer() {
		this->stopThread();
	}

	/// <summary>
	/// Starts the timer thread and starts tick counting
	/// @note Does nothing if thread is already started
	/// </summary>
	inline void startThread(void) {
		if (this->m_trdCounting.joinable()) {
			return; // we're already running, silly
		}

		this->m_bRunTrd = true;
		this->m_trdCounting = std::thread(&AETimer::tickTimer, this);
		if (!this->m_trdCounting.joinable()) {
			throw std::runtime_error("Could not start AETimer thread!");
		}
	}

	/// <summary>
	/// Stops the timer thread and stops tick counting
	/// </summary>
	inline void stopThread(void) {
		if (!this->m_trdCounting.joinable() && !this->m_bRunTrd) {
			return; //maybe it was closed already?
		}

		this->m_bRunTrd = false;
		this->m_trdCounting.join();
	}

	/// <summary>
	/// Resets the timer's tick to 0
	/// </summary>
	inline void resetTick(void) noexcept {
		this->m_ullTicks = 0;
	}

	/// <summary>
	/// Sets the new tick value instead of the current
	/// </summary>
	/// <param name="tick">The tick value to set instance's tick value to</param>
	inline void setTick(const ullint tick) noexcept {
		this->m_ullTicks = tick;
	}

	/// <summary>
	/// Sets the new delay instead of the current
	/// @note It stops the thread and starts it again -- don't expect the counting to start immediately
	/// </summary>
	/// <param name="tps">The amount of ticks per second to set the delay to</param>
	inline void setDelay(const double tps) {
		this->stopThread();
		this->m_rDelay.setFps(tps);
		this->startThread();
	}
	
	/// <summary>
	/// Returns the current tick of the timer.
	/// @note If thread is not started/working, the return value will be the same
	/// </summary>
	/// <returns>ullint of the current timer tick</returns>
	inline ullint getTick(void) const noexcept {
		return this->m_ullTicks;
	}

	/// <summary>
	/// Calculates the approximate world time of the timer in seconds (from the ticks)
	/// @note If thread is not started/working, the return value will be the same
	/// </summary>
	/// <returns>double of the approximate world time the timer has counted (using it's ticks)</returns>
	inline double getWorldTime(void) const noexcept {
		return this->m_ullTicks.load() * this->m_rDelay.getDelay();
	}

	/// <summary>
	/// Returns the fps goal of AEFrame instance in the AETimer
	/// @see AEFrame::getFrameRate()
	/// </summary>
	/// <returns>Rounded int of the approximated fps goal</returns>
	inline double getFrameRate(void) const noexcept {
		return this->m_rDelay.getFrameRate(); 
	}

	/// <summary>
	/// Returns the maximum AEFrame's instance in the AETimer delay from the set fps, in seconds
	/// @see AEFrame::getDelay()
	/// </summary>
	/// <returns>double of the maximum AEFrame's instance in the AETimer delay in real-world seconds</returns>
	inline double getDelay(void) const noexcept {
		return this->m_rDelay.getDelay();
	}

	/// <summary>
	/// Stops and sleeps the thread untill the given timer hits the certain tick number.
	/// </summary>
	/// <param name="timer">The instance of the AETimer to wait for</param>
	/// <param name="tick">The AETimer instance tick value to wait for</param>
	static inline void waitForTick(const AETimer& timer, const ullint tick) {
		AEFrame fr(timer.getFrameRate()*2);
		while (tick > timer.getTick()){
			fr.sleep();
		}
	}

	/// <summary>
	/// Stops and sleeps the thread untill the given timer goes through certain tick amount
	/// </summary>
	/// <param name="timer">The instance of the AETimer to wait for</param>
	/// <param name="tick">The amount of ticks to wait for in the AETimer instance</param>
	static inline void waitTicks(const AETimer& timer, const ullint tick) {
		AETimer::waitForTick(timer, tick + timer.m_ullTicks);
	}


private:

	/// <summary>
	/// The function that does the tick counting for the timer
	/// </summary>
	inline void tickTimer(void) {
		//load the atomic in the loop as fast as possible
		//we don't care about a lot of sync, since...it's 1 byte
		//and we just need approximate time to stop the timer
		while (this->m_bRunTrd.load(std::memory_order_relaxed)) {
			this->m_rDelay.sleep();
			this->m_ullTicks++;
			
		}
	}

	/// The frame-rater of the timer for proper delay in the tick-counting function loop
	AEFrame m_rDelay;
	/// The thread that does the tick counting
	std::thread m_trdCounting;
	/// The tick count variable
	std::atomic<ullint> m_ullTicks;
	/// The flag to continue running the tick counting loop
	std::atomic<bool> m_bRunTrd;


};

//aaaand yeah, register the class
REGISTER_CLASS(AETimer);


#ifdef ENGINE_ENABLE_GLOBAL_MODULES

/// The global timer of the engine to time engine-wide events.
/// It starts counting as soon as the program starts.
inline AETimer globalTimer(ENGINE_FPS);

#endif // ENGINE_ENABLE_GLOBAL_MODULES


#endif // !ENGINE_TIMER_HPP
