
/*! @file AEMisc/include/AEUtilsMacros.hpp
 *
 *  @brief This file contains the engine's **macros for trivial tasks**.
 *
 *  Stuff that I found that I needed to implement to perform some task, like debug-mode printing
 *  
 *  And then thought that the engine would benefit from by making it an engine-wide utility.
 *
 *  Less copy-pasted code, yayyy.
 *
 *  @date 2024/03/14
 *
 *	@author	Artemii Kozhemiak (SuperArtyK)
 *
 *	@copyright MIT License
 *
 *  *Should not cause everything to break :)*
 */

#pragma once

#ifndef ENGINE_UTILS_MACROS_HPP
#define ENGINE_UTILS_MACROS_HPP

#include "AETypedefs.hpp"
#include "AEFlags.hpp"
#include <iostream>
#include <cstdio>
#include <type_traits>
#include <cstddef>

 // Thanks, Arjun! https://stackoverflow.com/questions/11350878/how-can-i-determine-if-the-operating-system-is-posix-in-c
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__)) 
#include <unistd.h> // unix stuff, wooooo!
#endif

//function-like macros
//since it's more convenient to have them as a macro
//because...syntax

/// Macro for the calculation of the **size of the static array** (that is determined compile-time).
/// <param name="arr">The array to calculate the size of</param>
/// @warning If used on dynamic array, it yields the [size of the pointer on the platform] divided by [size of the array's type]
#define arrsize(arr) (sizeof(arr) / sizeof(arr[0]) )

/// Macro for the check **if modulo of x and y results in 0**.
/// <param name="dividend">The dividend of the operation</param>
/// <param name="divisor">The divisor of the operation</param>
#define mod0(dividend, divisor) (!(dividend % divisor))

/// Macro to get the **certain bit of the given value** (bitshift operation).
/// <param name="value">The value to get the bit of</param>
/// <param name="bit">The bit number to get</param>
#define getBit(value, bit) (value& (1 << bit))

/// Macro for the better-fomatted **assertion for runtime checks**.
/// <param name="expression">The logical expression to assert</param>
/// <param name="message">The message to display if the assertion fails</param>
/// @note Only works in engine's debug mode. Does nothing if #ENGINE_DEBUG is not set to 1
/// @note Only works with engine's assertions enabled. Does nothing if #ENGINE_ENABLE_ASSERT is not set to 1
/// @see #ENGINE_DEBUG
/// @see #ENGINE_ENABLE_ASSERT
#if ENGINE_ENABLE_ASSERT && ENGINE_DEBUG
#define aceAssert(expression, message) if ( !(expression) ) { \
	std::cerr << "\n\nASSERTION FAILED! \nExpression: " << #expression << \
		"\nFile: " << __FILE__ << \
		"\nFunction: " << __FUNCTION__ << "()" << \
		"\nLine: " << __LINE__ << \
		"\nMessage: " << message << std::endl; \
	std::terminate(); }
#else
#define aceAssert(expression, message) {}
#endif

/// Macro for the **debug printf()** printing, use the same way as you have used printf before.
/// Prints "[DEBUG] [function name that called it] [invoked line] -> [stuff that you wanted the printf to print]"
/// @note Prints additional newline with each print
/// @note Only works in engine's debug mode. Does nothing if #ENGINE_DEBUG is not set to 1
/// @see #ENGINE_DEBUG
#if ENGINE_DEBUG
#define dprintf(...) if constexpr (ENGINE_DEBUG) { \
	std::printf("[DEBUG] [%s()] [line:%llu] -> ", \
		__FUNCTION__, \
		(ullint)__LINE__); \
	std::printf(__VA_ARGS__); \
	std::printf("\n"); }
#else
#define dprintf(...)
#endif

/// Macro to check **if the given type T is the same as Y**.
/// <param name="T">The first type to compare</param>
/// <param name="Y">The second type to compare</param>
/// @note This discards the cv-qualifiers from the types in the comparison
#define IS_SAME_NOCV(T, Y) (std::is_same<std::remove_cv<T>::type, std::remove_cv<Y>::type>::value)

/// Macro to check **if the given type T is the same as Y**.
/// The decay happens before the removal of cv-qualifiers
/// <param name="T">The first type to compare</param>
/// <param name="Y">The second type to compare</param>
/// @note This decays the types used in the comparison
/// @note This discards the cv-qualifiers from the types in the comparison
#define IS_SAME_NOCV_DECAY(T, Y) (std::is_same<std::remove_cv<std::decay<T>::type>::type, std::remove_cv<std::decay<Y>::type>::type>::value)


/// Macro to convert the normal **string literal to utf-8 string literal**.
/// Example: "Hello World!" to u8"Hello World!"
/// <param name="x">The string literal to convert to utf8</param>
#define UTF8(x) u8 ## x //converts the "string" to the u8"string"

/// Macro to convert the normal **string literal to a wide string literal**.
/// Example: "Hello World!" to L"Hello Worldl!"
/// <param name="x">The string literal to convert to wstring</param>
#define WSTR(x) L ## x //converts the "string" to the L"string"

/// Macro to convert the passed **non-string literals to a string literal**.
/// That can be another number macro, or random garbage (abcdef will work too, turns into "abcdef")
/// <param name="x">The stuff to convert to a string literal</param>
/// @note If a string literal was passed, then it will have the quotation marks as escape characters
/// @note Example STR("abc") will result into "\"abc\"" literal
/// @warning Doesn't work on composed macros (macros that consist of other macros within themselves)
#define STR(x) _STR_HELPER(x)

/// The macro helper to convert stuff into a string literal.
/// @see STR()
#define _STR_HELPER(x) #x


/// Macro for the engine's **offsetoff macro**.
/// It's usefull if the compiler doesn't support the offsetof natively, and make the code more cross-compileable.
/// <param name="type">The type/struct name</param>
/// <param name="member">The member of struct/type to calculate the offset between</param>
#ifdef offsetof
#define aeoffsetof(type, member) offsetof(type, member)
#else
#define aeoffsetof(type, member) (intptr_t)&(((type*)NULL)->member)

// MSVC Version of offsetof, in case my version fails to work
// #define aeoffsetof(type,member) ((::size_t)&reinterpret_cast<char const volatile&>((((type*)0)->member)))
// or
// #define aeoffsetof(type,member) ((size_t)&(((type*)0)->member))
#endif //offsetof


//other stuff
/// Macro for **newline char literal**.
#define NLC '\n'

/// Macro for **newline string literal**.
#define NLS "\n"

/// Macro for the size of the **formatted date-time string**, produced by by the ace::utils::formatDate().
#define DATETIME_STRING_SIZE sizeof("YYYY-MM-DD.HH:mm:SS")


// stuff for crosscompilation and 64bit access
// dont touch pls
#ifdef _WIN32
#define ftell _ftelli64
#define fseek _fseeki64
#elif _POSIX_VERSION 
#define ftell ftello64
#define fseek fseeko64
#endif


#endif // !ENGINE_UTILS_MACROS_HPP
