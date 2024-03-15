
/*! @file AEMisc/include/AEUtils.hpp
 *  
 *  @brief This file contains the engine's **usefull utilities for trivial tasks**.
 *  
 *  Stuff that I found that I needed to implement to perform some task (like bool to "true"/"false" string), <br>
 *  thought the engine would benefit from by making it a global utility.
 *  
 *  Less copy-pasted code, yayyy.
 *  
 *  *Should not cause everything to break :)*
 */

#pragma once

#ifndef ENGINE_UTILS_HPP
#define ENGINE_UTILS_HPP

#include "AEFlags.hpp"
#include "AETypedefs.hpp"
#include <algorithm>
#include <iostream>
#include <string>
#include <cstring>
#include <ctime>
#include <sstream>
#include <iomanip>

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
#define mod0(dividend, divisor) ()

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
/// Prints "[current time] [DEBUG] [function name that called it] [invoked line] -> [stuff that you wanted the printf to print]"
/// @note Prints additional newline with each print
/// @note Only works in engine's debug mode. Does nothing if #ENGINE_DEBUG is not set to 1
/// @see #ENGINE_DEBUG
#if ENGINE_DEBUG
#define dprintf(...) if constexpr (ENGINE_DEBUG) { \
	printf("[%s] [DEBUG] [%s()] [line:%llu] -> ", \
		ace::utils::getCurrentDate().c_str(), \
		__FUNCTION__, \
		(ullint)__LINE__); \
		printf(__VA_ARGS__); printf("\n"); }
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


/// This namespace contains **everything that belongs to the engine**.
/// Everything but the typedefs and macros. <br>
/// That includes modules, custom types (structs), version and current status values, handles to windows, etc.
/// 
/// Think of it as ACEngine's version of std namespace
namespace ace {

	/// This namespace contains the engine's **utility functions and code**.
	/// The functions that do the code-cluttering stuff for you.
	namespace utils {

		/// <summary>
		/// Converts **bool to string** (word versions).
		/// </summary>
		/// <param name="b">The boolean to convert</param>
		/// <returns>
		///		If **b** is true:
		///		* The **const std::string_view** of "true"
		///		
		///		If **b** is false:
		///		* The **const std::string_view** of "false"
		/// </returns>
		constexpr const std::string_view boolToString(const bool b) noexcept {
			return (b) ? "true" : "false";
		}

		/// <summary>
		/// Checks if the **value is within some range**.
		/// @note The type of the values you pass to this function shall be the same
		/// </summary>
		/// <typeparam name="T">The 1st type of values to compare</typeparam>
		/// <typeparam name="Y">The 2nd type of values to compare (defaults to **T**)</typeparam>
		/// <typeparam name="U">The 3rd type of values to compare (defaults to **T**)</typeparam>
		/// <param name="minval">Minimum value of the range of values</param>
		/// <param name="maxval">Maximum value of the range of values</param>
		/// <param name="i">The value to check</param>
		/// <returns>
		///		If the value **i** is within the range:
		///		* **True**
		///		
		///		Otherwise:
		///		* **False**
		/// </returns>
		template<typename T, typename Y = T , typename U = T>
		constexpr bool isInRange(const T minval, const Y maxval, const U i) noexcept {
			return (minval <= i) && (i <= maxval);
		}

		/// <summary>
		/// **Lowers the passed std::string** by reference -- transformation is applied directly to the passed string.
		/// </summary>
		/// <param name="str1">The (reference to) string to be lowered</param>
		inline void toLowerRef(std::string& str1) {
			std::transform(str1.begin(), str1.end(), str1.begin(), ::tolower); // adding "::" because c++ decided to add the CharT version in <locale>, that conflicts
		}

		/// <summary>
		/// Returns the **lowered version of passed std::string**.
		/// </summary>
		/// <param name="str1">The string to be lowered</param>
		/// <returns>
		///		The lowered version of the **str1**
		///	</returns>
		inline std::string toLowerVal(std::string str1) { // we aren't passing by const reference....because we'll make a copy anyway
			std::transform(str1.begin(), str1.end(), str1.begin(), ::tolower); // because c++ decided to add the CharT version in <locale>, that conflicts
			return str1;
		}

		/// <summary>
		/// **Uppers the passed std::string** by reference.
		/// </summary>
		/// <param name="str1">The (reference to) string to be uppered</param>
		inline void toUpperRef(std::string& str1) {
			std::transform(str1.begin(), str1.end(), str1.begin(), ::toupper); // because c++ decided to add the CharT version in <locale>, that conflicts
		}

		/// <summary>
		/// Returns the **uppered version of passed std::string**.
		/// </summary>
		/// <param name="str1">The string to be uppered</param>
		/// <returns>
		///		The uppered version of the **str1**
		/// </returns>
		inline std::string toUpperVal(std::string str1) { // we aren't passing by const reference....because we'll make a copy anyway
			std::transform(str1.begin(), str1.end(), str1.begin(), ::toupper); // because c++ decided to add the CharT version in <locale>, that conflicts
			return str1;
		}
		
		
		/// <summary>
		/// Checks if the given **string is a numerical value**.
		/// </summary>
		/// <typeparam name="checkFloat">Template flag whether to check for integer values (false) or any/float values (true)</typeparam>
		/// <param name="strnum">String object (any, std::string, c-string, etc), "number" to check</param>
		/// <returns>
		///		If **strnum** is a numeric string:
		///		* **True**
		///		 
		///		Otherwise, or if the size of **strnum** is 0:
		///		* **False**
		/// </returns>
		template<const bool checkFloat = true>
		constexpr bool isNum(const std::string_view strnum) noexcept {
			const std::size_t len = strnum.size();
			//is it empty?
			if (len == 0) {
				return false;
			}
			size_t i = 0;
			if (strnum[0] == '-') {
				//check if string has only the minus in it
				if (len < 2) {
					return false;
				}
				i = 1;
			}
			//quick check, if string has 0 as the first number, minus before it
			//and no dot after
			if (strnum[i] == '0' && i == 1 &&
				(len == 2 || 
					(len > 2 && strnum[++i] != '.'))) {
				return false; //hey that's invalid!
			}

			bool metDecimal = false;
			for (; i < len; i++) {
				if (strnum[i] < '0' || strnum[i] > '9') {
					if constexpr (checkFloat) {
						if (strnum[i] == '.' && !metDecimal) {
							metDecimal = true;
						}
						else {
							return false;
						}
					}
					else {
						return false;
					}
					
				}
			}
			//passed
			return true;
		}

		/// <summary>
		/// Formats the current **(c)time point as YYYY-MM-DD.HH:mm:SS string**.
		/// </summary>
		/// <param name="timept">C time point</param>
		/// <returns>
		///		If the **timept** is positive:
		///		* **std::string** containing the formatted **timept** with the set format, of size #DATETIME_STRING_SIZE.
		///		
		///		Otherwise (**timept** is negative):
		///		* Emtpy **std::string**
		/// </returns>
		inline std::string formatDate(const std::time_t timept) {
			if (timept < 0) {
				return "";
			}

			struct tm tstruct {};
#ifdef _WIN32
			localtime_s(&tstruct, &timept);
#else
			tstruct = *localtime(&timept);
#endif
			char buff[DATETIME_STRING_SIZE]{};
			snprintf(buff, DATETIME_STRING_SIZE, "%04d-%02d-%02d.%02d:%02d:%02d",
				tstruct.tm_year + 1900, tstruct.tm_mon + 1,
				tstruct.tm_mday, tstruct.tm_hour, tstruct.tm_min,
				tstruct.tm_sec);
			return buff;
		}

		/// <summary>
		/// Formats the current **(c)time point as YYYY-MM-DD.HH:mm:SS c-string**.
		/// @warning **str** must be at least #DATETIME_STRING_SIZE bytes long. Otherwise you'll get a sigsev in your face.
		/// @see #DATETIME_STRING_SIZE
		/// </summary>
		/// <param name="timept">C time point</param>
		/// <param name="str">The c-string to write the value to</param>
		/// <returns>
		///		If the **timept** is positive and **str** is not nullptr:
		///		* The pointer to the formatted string (same as passed **str**)
		/// 
		///		Otherwise:
		///		* Unmodified **str**
		///	</returns>
		inline char* formatDate(const std::time_t timept, char* const str) noexcept {
			if (timept < 0 || !str) {
				return str;
			}

			struct tm tstruct {};
#ifdef _WIN32
			localtime_s(&tstruct, &timept);
#else
			tstruct = *localtime(&timept);
#endif

			snprintf(str, DATETIME_STRING_SIZE, "%04d-%02d-%02d.%02d:%02d:%02d",
				tstruct.tm_year + 1900, tstruct.tm_mon + 1,
				tstruct.tm_mday, tstruct.tm_hour, tstruct.tm_min,
				tstruct.tm_sec);
			return str;
		}

		/// <summary>
		/// Formats the **current date and time as YYYY-MM-DD.HH:mm:SS**.
		/// @see ace::utils::formatDate(const std::time_t)
		/// </summary>
		/// <returns>
		///		**std::string** of the current date in **YYYY-MM-DD.HH:mm:SS** format
		///	</returns>
		inline std::string getCurrentDate(void) {
			return ace::utils::formatDate(std::time(nullptr));
		}

		/// <summary>
		/// Converts the given c-string to a (c)time point (std::time_t), using a set format
		/// @note Format specifiers https://en.cppreference.com/w/cpp/io/manip/get_time
		/// </summary>
		/// <param name="timestr">The c-string to a formatted time</param>
		/// <param name="timeformat">A c-string of the format that it uses</param>
		/// <returns>
		///		If the **timestr** and **timeformat** isn't nullptr, and **timestr** follows the set format:
		///		* Time since epoch as a **std::time_t** object
		///		
		///		Otherwise:
		///		* **-1** as the **std::time_t** object
		///	</returns>
		inline std::time_t stringToDate(const char* const timestr, const char* const timeformat = "%Y-%m-%d.%X") {
			if (!timestr || !timeformat) {
				return std::time_t(-1);
			}
			struct tm tstruct {};
			std::istringstream iss(timestr);
			iss >> std::get_time(&tstruct, timeformat);

			// use system timezone databases for...proper time shift;
			// thanks Michael Paxton and Lightness-Races-in-Orbit: https://stackoverflow.com/a/55475752
			tstruct.tm_isdst = -1; 
			return std::mktime(&tstruct);
		}


		/// <summary>
		/// Converts the **hex address value to std::string**.
		/// This keeps the leading zeros of the address. So passing the nullptr, which is 0, will result in something like 0x00000000
		/// @note The pointer value is not evaluated (passing nullptr is fine :) )
		/// </summary>
		/// <param name="myptr">Pointer to whatever memory adress</param>
		/// <param name="add0x">Flag to whether include the "0x" before the address value</param>
		/// <returns>
		///		**std::string** with the hexadecimal version of the passed address
		///	</returns>
		inline std::string addrToStr(const void* const myptr, const bool add0x = true) noexcept {
			if (add0x) {
				char myarr[sizeof(void*) * 2 + 3];
				std::snprintf(myarr, sizeof(myarr), "0x%0*llx", int(sizeof(void*)) * 2, ullint(std::uintptr_t(myptr)));
				return myarr;
			}
			// add0x is false
			char myarr[sizeof(void*) * 2 + 1];
			std::snprintf(myarr, sizeof(myarr), "%0*llx", int(sizeof(void*)) * 2, ullint(std::uintptr_t(myptr)));
			return myarr;
		}
		
		/// <summary>
		/// Convenient sleep function to **sleep the thread for milliseconds**.
		/// @note May be inaccurate in delay (as...all sleep functions are)
		/// Uses #sleepFor() inside
		/// @see #sleepFor()
		/// @see #milliSec()
		/// </summary>
		/// <param name="ms">The amount of milliseconds to sleep</param>
		inline void sleepMS(const int ms) noexcept {
			sleepFor(milliSec(ms));
		}

		/// <summary>
		/// Convenient sleep function to **sleep the thread for microseconds**.
		/// @note May be inaccurate in delay (as...all sleep functions are)
		/// Uses #sleepFor() inside
		/// @see #sleepFor()
		/// @see #microSec()
		/// </summary>
		/// <param name="us">The amount of microseconds to sleep</param>
		inline void sleepUS(const int us) noexcept {
			sleepFor(microSec(us));
		}

		/// <summary>
		/// A wrapper to make the proper **cross-compiler version of fopen()**
		/// Opens the file with given name and flags.
		/// Needed so i don't need to retype ifdef for each of file openings
		/// Or pragma-disable warnings
		/// Makes code cleaner...relatively
		/// </summary>
		/// <param name="fname">File name, as you'd pass to fopen</param>
		/// <param name="flags">File flags, as you'd pass to fopen</param>
		/// <returns>
		///		Pointer to the **std::FILE** as the result of the fopen call
		///	</returns>
		inline std::FILE* fopenCC(const char* const fname, const char* const flags) noexcept {
			//safety, so our compiler shuts up about the unsafe and deprecated function
			//and trigger only on vc++
			std::FILE* filestr = nullptr;
//if our compiler is vc++
#ifdef _WIN32 
			if (fopen_s(&filestr, fname, flags)) {
				return nullptr;
			}
#else
//or some other stuff
			filestr = fopen(fname, flags);
#endif // _WIN32 
			return filestr;
		}

		/// <summary>
		/// Converts the given **numeric char to int of type T**
		/// </summary>
		/// <typeparam name="T">The type of the int to convert the char to</typeparam>
		/// <param name="c">The numeric char to convert</param>
		/// <returns>
		///		If the given char is within the '0'->'9' range:
		///		* The value of type **T** that corresponds to the given value of char
		///		
		///		Otherwise:
		///		* **-1** as the type **T**
		///	</returns>
		template<typename T = int>
		constexpr T numCharToInt(const char c) noexcept requires(std::is_integral<T>::value) {
			return (ace::utils::isInRange<char, char, char>('0', '9', c)) ? (c - '0') : -1;
		}
		
		/// <summary>
		/// Converts the given **int of type T to numeric char**
		/// </summary>
		/// <typeparam name="T">The type of the int to check</typeparam>
		/// <param name="i">The integer value to convert</param>
		/// <returns>
		///		If the given int is within the 0 -> 9 range:
		///		* The value of type char that corresponds to the given int value of type **T**
		///		
		///		Otherwise:
		///		* **-1** as the char type
		///	</returns>
		template<typename T = int>
		constexpr char intToNumChar(const T i) noexcept {
			return (ace::utils::isInRange<T, T, T>(0, 9, i)) ? (i + '0') : -1;
		}

		/// <summary>
		/// Checks **if the passed string is alpha-numeric** (contains only numbers and english letters)
		/// </summary>
		/// <param name="str">The passed string to check</param>
		/// <returns>
		///		If the string contains *only* numbers(0-9) or alphabetic characters(A-Z, a-z):
		///		* **True**
		///		
		///		Otherwise:
		///		* **False**
		///	</returns>
		constexpr inline bool isAlNum(const std::string_view str) {
			return std::find_if_not(str.begin(), str.end(), [](char c) noexcept { return std::isalnum(c); }) == str.end();
		}

		/// <summary>
		/// Checks **if the passed string is alpha-numeric with underscores** (contains only numbers, english letters, and underscores)
		/// </summary>
		/// <param name="str">The passed string to check</param>
		/// <returns>
		///		If the string contains *only* numbers(0-9), alphabetic characters(A-Z, a-z), or an underscore (_):
		///		* **True**
		///		
		///		Otherwise:
		///		* **False**
		///	</returns>
		constexpr inline bool isAlNumUs(const std::string_view str) {
			return std::find_if_not(str.begin(), str.end(), [](char c) noexcept { return std::isalnum(c) || c == '_'; }) == str.end();
		}

	}	
}

#endif // !ENGINE_UTILS_HPP
