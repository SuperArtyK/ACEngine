#pragma once

#ifndef TYPEDEFS_HPP
#define TYPEDEFS_HPP

#include <limits>
#include <chrono>

 //a few typedefs so I'll type less code
 //
 //numbers
 ///short for small unsigned int
typedef unsigned char smalluint;
///small signed int
typedef char smallint;
///big unsigned int
typedef unsigned long long biguint;
///big signed int
typedef long long bigint;
///just unsigned int
//typedef unsigned long uint;

//max/min values of these numbers
///small unsigned int max
#define SUINT_MAX UCHAR_MAX
///small unsigned int min
#define SUINT_MIN 0

///small signed int max
#define SINT_MAX SCHAR_MAX
///small signed int min
#define SINT_MIN SCHAR_MIN

///big unsigned int max
#define BUINT_MAX ULLONG_MAX
///big unsigned int min
#define BUINT_MIN 0

///big signed int max
#define BINT_MAX LLONG_MAX
///big signed int min
#define BINT_MIN LLONG_MIN

/// no UINT_MAX, as it's already defined
///unsigned int min
#define UINT_MIN 0

//time
///short for system time, so we dont need to use autos and init everytime
typedef std::chrono::time_point<std::chrono::system_clock> systime;
///good ol' high resolution clock, for precision checking
typedef std::chrono::time_point<std::chrono::high_resolution_clock> highrestime;
///steady clock
typedef std::chrono::time_point<std::chrono::steady_clock> steadytime;
///returns system time
#define getsystime std::chrono::system_clock::now()
///returns highres time
#define gethighrestime std::chrono::high_resolution_clock::now()
///returns steady time
#define getsteadytime std::chrono::steady_clock::now()
///returns time between 2 timepoints(float)
#define calculatetime(tp1,tp2) std::chrono::duration<float>(tp2 - tp1).count()
///time point. using preprocessor macro, as it is a template, and we only need to shorten the name
#define timepoint std::chrono::time_point
///time duration, same reason as timepoint
#define tduration std::chrono::duration
///chrono's milliseconds
#define millisec std::chrono::milliseconds
///chrono's microseconds
#define microsec std::chrono::microseconds

//thread
///thread's sleep_until
#define sleepuntil std::this_thread::sleep_until

#endif // !TYPEDEFS_HPP