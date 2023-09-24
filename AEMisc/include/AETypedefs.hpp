
#pragma once

#ifndef ENGINE_TYPEDEFS_HPP
#define ENGINE_TYPEDEFS_HPP

#include <climits>
#include <chrono>
#include <thread>

// a few typedefs so I'll type less [hard to read] code 

// numbers
/// A shortcut for small unsigned (char) integer
typedef uint8_t ucint;

/// A shortcut for small signed (char) integer
typedef int8_t cint;

/// A shortcut for big unsigned (long long) integer
typedef unsigned long long ullint;

/// A shortcut for big signed (long long) integer
typedef long long llint;

/// A shortcut for a just unsigned int
typedef unsigned long uint;



// max/min values of these numbers
/// small unsigned int max
#define UCINT_MAX UCHAR_MAX

/// small unsigned int min
#define UCINT_MIN 0

/// small signed int max
#define CINT_MAX SCHAR_MAX

/// small signed int min
#define CINT_MIN SCHAR_MIN

/// big unsigned int max
#define ULLINT_MAX ULLONG_MAX

/// big unsigned int min
#define ULLINT_MIN 0

/// big signed int max
#define LLINT_MAX LLONG_MAX

/// big signed int min
#define LLINT_MIN LLONG_MIN

// no UINT_MAX, as it's already defined
/// unsigned int min
#define UINT_MIN 0

/// unsigned short min
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


/// A macro to calculate time between 2 timepoints(tp1, tp2) as some type (typ)
#define calculateTime(tp1,tp2,typ) std::chrono::duration<typ>(tp2 - tp1).count()

/// A macro as the shortcut of chrono's time point. I couldn't shove it into a typedef so...a macro
#define timePoint std::chrono::time_point

/// A macro as the shortcut of chrono's time duration. Same reason as timePoint
#define timeDur std::chrono::duration

/// A macro as the shortcut of chrono's milliseconds
#define milliSec(x) std::chrono::milliseconds(x)

/// A macro as the shortcut of chrono's microseconds
#define microSec(x) std::chrono::microseconds(x)


//thread
/// A macro as the shortcut of thread's sleep_until
#define sleepUntil(x) std::this_thread::sleep_until(x)

/// A macro as the shortcut of thread's sleep_for
#define sleepFor(x) std::this_thread::sleep_for(x)

#endif // !ENGINE_TYPEDEFS_HPP
