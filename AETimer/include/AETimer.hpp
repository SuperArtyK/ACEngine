
/*! @file AETimer/include/AETimer.hpp
 *
 *	@brief This file contains the engine's **tick-based timer**.
 *
 *	This timer operates using AEFrame, and ticks with the delay you can set.
 *	
 *	It allows to time or predict certain events.
 *	
 *	Or as a weird alternative to sleep functionality.
 *	
 *	@see AETimer::waitForTick()
 *	@see AETimer::waitTicks()
 *	
 *	@date 2023/09/24
 *
 *	@author	Artemii Kozhemiak (SuperArtyK)
 *
 *	@copyright MIT License
 *
 *  *Should not cause everything to break :)*
 */

#pragma once

#ifndef ENGINE_TIMER_HPP
#define ENGINE_TIMER_HPP

#include "include/AEFrame.hpp"
#include <atomic>
#include <mutex>
#include <thread>


/// <summary>
/// This module is the engine's **tick timer**.
/// 
/// It allows to time or predict events ("happen at tick X"), with the variable tick speed. <br>
/// Internally it uses the AEFrame module for the delay, so the same limitations apply.
/// 
/// Internally it makes a separate thread that increments the tick number every set time period, and sleeping after.
/// @see AEFrame
/// @note This is multithreading-safe! :D
/// </summary>
class AETimer : public __AEModuleBase<AETimer> {

public:

//constructors
	/// <summary>
	/// Class constructor -- **sets the delay** in ticks per second, and **starts the thread**.
	/// </summary>
	/// <param name="tps">The amount of ticks per second, as the double</param>
	explicit AETimer(const double tps) :
		m_rDelay(tps), m_ullTicks(0), m_bRunTrd(true) {
		this->startThread();
	}

	/// <summary>
	/// Class constructor -- assigns **default delay (ENGINE_FPS) and doesn't start the thread**
	/// </summary>
	AETimer(void) noexcept : 
		m_rDelay(ENGINE_FPS), m_ullTicks(0), m_bRunTrd(false) 
		{}

	/// <summary>
	/// Copy constructor -- **copies the data** and, if the original timer was started, **starts the timer**.
	/// </summary>
	/// <param name="tm">The original AETimer instance to make a copy of</param>
	AETimer(const AETimer& tm) :
		//COPYCONSTRUCT_MODULE(AETimer, tm), 
		m_rDelay(tm.m_rDelay), m_ullTicks(tm.m_ullTicks.load()), m_bRunTrd(tm.m_bRunTrd.load()) {

		if (this->m_bRunTrd) {
			this->startThread();
		}
	}

	/// <summary>
	/// Copy assignment operator -- **copies the data** and, if the original timer was started, **starts the timer**.
	/// </summary>
	/// <param name="tm">The original AETimer instance to make a copy of</param>
	/// <returns>Reference to the resulting AETimer copy</returns>
	AETimer& operator=(const AETimer& tm) {
		this->m_rDelay = tm.m_rDelay;
		this->m_ullTicks.store(tm.m_ullTicks.load());
		this->m_bRunTrd.store(tm.m_bRunTrd.load());
		if (this->m_bRunTrd) {
			this->startThread();
		}
		return *this;
	}

	/// <summary>
	/// **Class destructor**
	/// </summary>
	~AETimer(void) {
		this->stopThread();
	}

	/// <summary>
	/// **Starts the timer thread** and starts tick counting.
	/// @note Does nothing if thread is already started
	/// @note Throws std::runtime_error if the thread wasn't started for some reason(std::thread::joinable() returns false)
	/// @attention If the delay/fps, previously, was set to an invalid value, doesn't start thread.
	/// @see AEFrame::isValidFPS()
	/// @see AEFrame::isValidDelay()
	/// @see AEFrame::hasDelay()
	/// </summary>
	inline void startThread(void) {
		if (this->m_trdCounting.joinable()) {
			return; // we're already running, silly
		}

		if (!this->m_rDelay.hasDelay()) {
			return; // invalid fps
		}

		this->m_bRunTrd = true;
		this->m_trdCounting = std::thread(&AETimer::tickTimer, this);
		if (!this->m_trdCounting.joinable()) {
			throw std::runtime_error("Could not start AETimer thread!");
		}
	}

	/// <summary>
	/// **Stops the timer thread** and stops tick counting.
	/// @note Does nothing if the thread was already stopped.
	/// @warning Not thread-safe! *Can* (but unlikely) crash if called from 2 threads at the **same time** (closing the thread the thread twice)
	/// </summary>
	inline void stopThread(void) {
		if (!this->m_bRunTrd) {
			return; //maybe it was closed already?
		}

		this->m_bRunTrd = false;
		this->m_trdCounting.join();
	}

	/// <summary>
	/// Sets the **timer tick back to 0**.
	/// </summary>
	inline void resetTick(void) noexcept {
		this->m_ullTicks = 0;
	}

	/// <summary>
	/// Sets the timer's **new tick value to the passed value**.
	/// </summary>
	/// <param name="tick">The tick value to set instance's tick value to</param>
	inline void setTick(const ullint tick) noexcept {
		this->m_ullTicks = tick;
	}

	/// <summary>
	/// Sets the timer's **new delay as ticks-per-second**
	/// @attention If the value invalid, stops the thread.
	/// @see AEFrame::isValidFPS()
	/// @attention The thread will not be launched untill a proper tps is set.
	/// @see AETimer::startThread()
	/// @note Uses a mutex, so there *could* be some delay. Check the mutex behaviour on your system.
	/// </summary>
	/// <param name="tps">The amount of ticks per second to set the delay to</param>
	inline void setTPS(const double tps) {
		if (!AEFrame::isValidFPS(tps)) {
			this->stopThread();
		}
		std::lock_guard mtx(this->m_mtxThreadMutex);
		this->m_rDelay.setFps(tps);

	}
	
	/// <summary>
	/// Returns the **current tick** of the timer.
	/// @note If thread is not started/working, the return value will be the same (duh)
	/// </summary>
	/// <returns>
	///		Current timer tick as **ullint** type.
	///	</returns>
	inline ullint getTick(void) const noexcept {
		return this->m_ullTicks;
	}

	/// <summary>
	/// Calculates the *approximate world time* of the timer's ticks as seconds
	/// @note If thread is not started/working, the return value will be the same (duh)
	/// </summary>
	/// <returns>
	///		The approximation of the counted world time in seconds using the timer's ticks, as the **double** type.
	///	</returns>
	inline long double getWorldTime(void) const noexcept {
		return this->m_ullTicks.load() * (long double) this->m_rDelay.getDelay();
	}

	/// <summary>
	/// Returns the **ticks-per-second** of the AETimer's instance (aka AEFrame's fps goal).
	/// @see AEFrame::getFPS()
	/// </summary>
	/// <returns>
	///		The ticks-per-second rate of the instance as **double** type.
	///	</returns>
	inline double getTPS(void) const noexcept {
		return this->m_rDelay.getFPS(); 
	}

	/// <summary>
	/// Returns the **delay-per-tick** of the instance, as seconds (aka AEFrame's delay).
	/// @see AEFrame::getDelay()
	/// </summary>
	/// <returns>
	///		Delay-per-tick of the instance as seconds, as the **double** type.
	///	</returns>
	inline double getDelay(void) const noexcept {
		return this->m_rDelay.getDelay();
	}

	/// <summary>
	/// Stops and **sleeps the thread untill** the given timer **hits the certain tick number**.
	/// </summary>
	/// <param name="timer">The instance of the AETimer to wait for</param>
	/// <param name="tick">The AETimer instance tick value to wait for</param>
	static inline void waitForTick(const AETimer& timer, const ullint tick) noexcept {
		AEFrame fr(timer.getTPS()*2);
		while (tick > timer.getTick()){
			fr.sleep();
		}
	}

	/// <summary>
	/// Stops and **sleeps the thread untill** the given timer **goes through certain amount of ticks**.
	/// </summary>
	/// <param name="timer">The instance of the AETimer to wait for</param>
	/// <param name="tick">The amount of ticks to wait for in the AETimer instance</param>
	static inline void waitTicks(const AETimer& timer, const ullint tick) noexcept {
		AETimer::waitForTick(timer, tick + timer.m_ullTicks);
	}


private:

	/// <summary>
	/// The function that does the tick counting for the timer
	/// </summary>
	inline void tickTimer(void) noexcept {
		//load the atomic in the loop as fast as possible
		//we don't care about a lot of sync, since...it's 1 byte
		//and we just need approximate time to stop the timer
		while (this->m_bRunTrd.load(std::memory_order_relaxed)) {
			this->m_rDelay.sleep();
			this->m_ullTicks++;
			
		}
	}

	/// The **frame-rater of the timer** for proper delay in the tick-counting function loop
	/// @see AETimer::tickTimer()
	AEFrame m_rDelay;
	/// The thread object to launch the thread for the tick counting
	std::thread m_trdCounting;
	/// The mutex to **syncronise changes with the AETimer::m_rDelay**
	std::mutex m_mtxThreadMutex;
	/// The **tick counter** variable
	std::atomic<ullint> m_ullTicks;
	/// The **flag to continue running the tick counting loop
	std::atomic<bool> m_bRunTrd;

	//aaaand yeah, register the class
	REGISTER_MODULE(AETimer);
};


#if ENGINE_ENABLE_GLOBAL_MODULES

namespace ace {
	/// The engine's **global timer**, using the default delay (explicitly).
	/// @note It starts counting as soon as the program starts.
	/// @see #ENGINE_FPS
	inline AETimer globalTimer(ENGINE_FPS);
}

#endif // ENGINE_ENABLE_GLOBAL_MODULES


#endif // !ENGINE_TIMER_HPP
