
#pragma once

#ifndef ENGINE_TYPEDEFS_HPP
#define ENGINE_TYPEDEFS_HPP

#include <chrono>
#include <climits>

// a few typedefs so I'll type less code

// numbers
/// short for small unsigned int
typedef unsigned char smalluint;
/// small signed int
typedef char smallint;
/// big unsigned int
typedef unsigned long long biguint;
/// big signed int
typedef long long bigint;
/// just unsigned int
// typedef unsigned long uint;

// max/min values of these numbers
/// small unsigned int max
#define SUINT_MAX UCHAR_MAX
/// small unsigned int min
#define SUINT_MIN 0

/// small signed int max
#define SINT_MAX SCHAR_MAX
/// small signed int min
#define SINT_MIN SCHAR_MIN

/// big unsigned int max
#define BUINT_MAX ULLONG_MAX
/// big unsigned int min
#define BUINT_MIN 0

/// big signed int max
#define BINT_MAX LLONG_MAX
/// big signed int min
#define BINT_MIN LLONG_MIN

/// no UINT_MAX, as it's already defined
/// unsigned int min
#define UINT_MIN 0

// time
//so we don't need to write *long* names of chrono-library stuff
//or rain dance with auto's

//timepoints
/// short for system-time timepoint
typedef std::chrono::time_point<std::chrono::system_clock> systime_tp;
/// short for good ol' high-resolution-clock timepoint
typedef std::chrono::time_point<std::chrono::high_resolution_clock> highrestime_tp;
/// short for steady-clock timepoint
typedef std::chrono::time_point<std::chrono::steady_clock> steadytime_tp;

//just clock-type identifiers
/// short for system-time
typedef std::chrono::system_clock systime;
/// short for good ol' high-resolution-clock
typedef std::chrono::high_resolution_clock highrestime;
/// short for steady-clock
typedef std::chrono::steady_clock steadytime;

/// chrono's microseconds
typedef std::chrono::microseconds microsec;
/// chrono's milliseconds
typedef std::chrono::milliseconds millisec;

/// returns system time
#define getsystime() std::chrono::system_clock::now()
/// returns highres time
#define gethighrestime() std::chrono::high_resolution_clock::now()
/// returns steady time
#define getsteadytime() std::chrono::steady_clock::now()
/// returns time between 2 timepoints(float)
#define calculatetime(tp1, tp2) std::chrono::duration<float>(tp2 - tp1).count()
/// time point. using preprocessor macro, as it is a template, and we only need
/// to shorten the name
#define timepoint std::chrono::time_point
/// time duration, same reason as timepoint
#define tduration std::chrono::duration
/// chrono's milliseconds
//#define millisec 
/// chrono's microseconds
//#define microsec std::chrono::microseconds

//compiler-related macros

#ifdef _MSC_VER 
///if our compiler is vc++, make sprintf sprintf_s, so compiler shuts up
///about it's warning
#define sprintf sprintf_s
#define fscanf fscanf_s

#endif // _MSC_VER 





// thread
/// thread's sleep_until
#define sleepuntil std::this_thread::sleep_until

#endif // !ENGINE_TYPEDEFS_HPP
