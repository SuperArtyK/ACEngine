
/** @file AEMisc/include/AETypedefs.hpp
 *  This file contains typedefs/aliases of the variable types that are long and
 *  are used often/inconvenient to use in their long original naming.
 *
 *  Should not cause everything to break :)
 */

#pragma once

#ifndef ENGINE_TYPEDEFS_HPP
#define ENGINE_TYPEDEFS_HPP

#include <climits>
#include <chrono>
#include <thread>

// a few typedefs so I'll type less [hard to read] code 

// numbers
/// A shortcut for small unsigned (char) integer (8bit)
typedef uint8_t ucint;

/// A shortcut for small signed (char) integer (8bit)
typedef int8_t cint;

/// A shortcut for big unsigned (long long) integer (64bit)
typedef uint64_t ullint;

/// A shortcut for big signed (long long) integer (64bit)
typedef int64_t llint;

/// A shortcut for a just signed int (32bit)
typedef int32_t sint;

/// A shortcut for a just unsigned int (32bit)
typedef uint32_t uint;

/// A shortcut for a just unsigned short (at least 16 bit)
typedef unsigned short ushort;


// max/min values of these numbers
/// Macro for the maximum value of the ucint type (unsigned char/8bit int)
#define UCINT_MAX UCHAR_MAX

/// Macro for the minimum value of the ucint type (unsigned char/8bit int)
#define UCINT_MIN 0

/// Macro for the maximum value of the cint type (signed char/8bit int)
#define CINT_MAX SCHAR_MAX

/// Macro for the minimum value of the cint type (signed char/8bit int)
#define CINT_MIN SCHAR_MIN

/// Macro for the maximum value of the ullint type (unsigned long long/64bit int)
#define ULLINT_MAX ULLONG_MAX

/// Macro for the minimum value of the ullint type (unsigned long long/64bit int)
#define ULLINT_MIN 0

/// Macro for the maximum value of the llint type (signed long long/64bit int)
#define LLINT_MAX LLONG_MAX

/// Macro for the minimum value of the llint type (signed long long/64bit int)
#define LLINT_MIN LLONG_MIN

/// Macro for the maximum value of the sint type (signed 32bit int)
#define SINT_MAX INT32_MAX

/// Macro for the minimum value of the sint type (signed 32bit int)
#define SINT_MIN INT32_MIN

// no UINT_MAX, as it's already defined
/// Macro for the minimum value of the uint type (unsigned long long/64bit int)
#define UINT_MIN 0

/// Macro for the minimum value of the ushrt type (unsigned short/16bit int)
#define USHRT_MIN 0



//time so we don't need to use autos and init every time
/// A shortcut for chrono's system time
typedef std::chrono::system_clock SysTime;

/// A shortcut for chrono's high resolution time
typedef std::chrono::high_resolution_clock HighResTime;

/// A shortcut for chrono's steady time
typedef std::chrono::steady_clock SteadyTime;


/// A macro to get the system time's now()
#define getSysTime() std::chrono::system_clock::now()

/// A macro to get the high resolution time's now()
#define getHighResTime() std::chrono::high_resolution_clock::now()

/// A macro to get the steady time's now()
#define getSteadyTime() std::chrono::steady_clock::now()


/// A macro to calculate time between 2 timepoints as some type.
/// <param name="tp1">The first, starting point of time (time point)</param>
/// <param name="tp2">The second, starting point of time (time point)</param>
/// <param name="type">The type of the resulting operation</param>
#define calculateTime(tp1, tp2, type) std::chrono::duration<type>(tp2 - tp1).count()

/// A macro as the shortcut of chrono's time point. I couldn't shove it into a typedef so...a macro
#define timePoint std::chrono::time_point

/// A macro as the shortcut of chrono's time duration. Same reason as timePoint
#define timeDur std::chrono::duration

/// A macro as the shortcut of chrono's milliseconds.
/// <param name="ms">The number of milliseconds to convert to</param>
#define milliSec(ms) std::chrono::milliseconds(ms)

/// A macro as the shortcut of chrono's microseconds.
/// <param name="us">The number of microseconds to convert to</param>
#define microSec(us) std::chrono::microseconds(us)


//thread
/// A macro as the shortcut of thread's sleep_until.
/// <param name="sec">The chronos's duration object (milliseconds, seconds, etc)</param>
#define sleepUntil(sec) std::this_thread::sleep_until(sec)

/// A macro as the shortcut of thread's sleep_for.
/// <param name="sec">The chronos's duration object (milliseconds, seconds, etc)</param>
#define sleepFor(sec) std::this_thread::sleep_for(sec)

#endif // !ENGINE_TYPEDEFS_HPP
