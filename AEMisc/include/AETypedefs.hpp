
/*! @file AEMisc/include/AETypedefs.hpp
 *  
 *  @brief This file contains the engine's **typedefs and variable type aliases**.
 *  
 *  The reason for this is that they are long or inconvenient to type.
 *  
 *  So -- shorter names for easier coding.
 *  
 *  @date 2023/09/21
 *
 *	@author	Artemii Kozhemiak (SuperArtyK)
 *
 *	@copyright MIT License
 *	
 *  *Should not cause everything to break :)*
 */

#pragma once

#ifndef ENGINE_TYPEDEFS_HPP
#define ENGINE_TYPEDEFS_HPP

#include <climits>
#include <chrono>
#include <thread>

// a few typedefs so I'll type less [hard to read] code 

// numbers
/// A shortcut for an integer of the **unsigned char** type (8 bits).
typedef uint8_t ucint;

/// A shortcut for an integer of the **signed char** type (8 bits).
typedef int8_t cint;

/// A shortcut for an integer of the **unsigned long long int** type (64 bits).
typedef unsigned long long ullint;

/// A shortcut for an integer of the **long long int** type (64 bits).
typedef long long llint;

/// A shortcut for a just **signed int** (32 bits)
/// @remark I don't know if you'd ever use this, but it's there, for completion.
typedef signed int sint;

/// A shortcut for an integer of the **unsigned int** (32 bits)
typedef unsigned int uint;

/// A shortcut for an integer of the just **unsigned short** type (16 bits)
typedef unsigned short ushort;

/// A shortcut for a just **signed short** (16 bits)
/// @note I don't know if you'd ever use this, but it's there, for completion.
typedef signed short sshort;



// max/min values of these numbers
/// Macro for the **maximum value of the ::ucint** type
#define UCINT_MAX UCHAR_MAX

/// Macro for the **minimum value of the ::ucint** type
#define UCINT_MIN 0

/// Macro for the **maximum value of the ::cint** type
#define CINT_MAX SCHAR_MAX

/// Macro for the **minimum value of the ::cint** type
#define CINT_MIN SCHAR_MIN

/// Macro for the **maximum value of the ::ullint** type
#define ULLINT_MAX ULLONG_MAX

/// Macro for the **minimum value of the ::ullint** type
#define ULLINT_MIN 0

/// Macro for the **maximum value of the ::llint** type
#define LLINT_MAX LLONG_MAX

/// Macro for the **minimum value of the ::llint** type
#define LLINT_MIN LLONG_MIN

/// Macro for the **maximum value of the ::sint** type
#define SINT_MAX INT_MAX

/// Macro for the **minimum value of the ::sint** type
#define SINT_MIN INT_MIN

// no UINT_MAX, as it's already defined
/// Macro for the **minimum value of the ::uint** type
#define UINT_MIN 0

// no USHRT_MAX, as it's already defined
/// Macro for the **minimum value of the ::ushort** type
#define USHRT_MIN 0

/// Macro for the **maximum value of the ::sshort** type
#define SSHRT_MAX SHRT_MAX

/// Macro for the **minimum value of the ::sshort** type
#define SSHRT_MIN SHRT_MIN


/// Macro for the **NULL value as a 0**, not a pointer
#define AENULL 0


//time so we don't need to use autos (ew)
/// A shortcut for **chrono's system clock**
typedef std::chrono::system_clock SysClock;

/// A shortcut for **chrono's high resolution clock**
typedef std::chrono::high_resolution_clock HighResClock;

/// A shortcut for **chrono's steady clock**
typedef std::chrono::steady_clock SteadyClock;


/// A macro to get the **system clock's current time** (now())
#define getSysTime() SysClock::now()

/// A macro to get the **high resolution clock's current time** (now())
#define getHighResTime() HighResClock::now()

/// A macro to get the **steady clock's current time** (now())
#define getSteadyTime() SteadyClock::now()


/// <summary>
/// An alias for the **std::chrono::time_point** to not type...as much
/// </summary>
/// <typeparam name="Clock">The clock (system, hi-res, steady) on which this time point is measured</typeparam>
/// <typeparam name="Duration">a std::chrono::duration type used to measure the time since epoch</typeparam>
template<class Clock, class Duration = typename Clock::duration>
using timePoint = std::chrono::time_point<Clock, Duration>;

/// <summary>
/// An alias for the **std::chrono::duration** to not type as much
/// </summary>
/// <typeparam name="ticksType">An arithmetic type representing the number of ticks</typeparam>
/// <typeparam name="Period">A std::ratio representing the tick period (i.e. the number of second's fractions per tick)</typeparam>
template<class ticksType, class Period = std::ratio<1>>
using timeDur = std::chrono::duration<ticksType, Period>;


/// An an alias as the shortcut of **chrono's milliseconds**.
using milliSec = std::chrono::milliseconds;

/// An alias as the shortcut of **chrono's microseconds**.
using microSec = std::chrono::microseconds;

/// An alias as the shortcut of **chrono's nanoseconds**.
using nanoSec = std::chrono::nanoseconds;

/// A macro to calculate **time between 2 timepoints** as some type.
/// <param name="tp1">The first, starting point of time (time point)</param>
/// <param name="tp2">The second, starting point of time (time point)</param>
/// <param name="type">The type of the resulting operation</param>
#define timeBetween(tp1, tp2, type) std::chrono::duration<type>(tp2 - tp1).count()

//thread
/// A macro as the shortcut of **thread's sleep_until**
/// <param name="sec">The chronos's duration object (milliseconds, seconds, etc)</param>
#define sleepUntil(sec) std::this_thread::sleep_until(sec)

/// A macro as the shortcut of **thread's sleep_for**
/// <param name="sec">The chronos's duration object (milliseconds, seconds, etc)</param>
#define sleepFor(sec) std::this_thread::sleep_for(sec)

#endif // !ENGINE_TYPEDEFS_HPP
