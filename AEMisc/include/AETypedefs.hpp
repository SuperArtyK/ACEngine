
#pragma once

#ifndef ENGINE_TYPEDEFS_HPP
#define ENGINE_TYPEDEFS_HPP

#include <climits>

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

#endif // !ENGINE_TYPEDEFS_HPP
