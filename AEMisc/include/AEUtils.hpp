
/** @file AEMisc/include/AEUtils.hpp
 *  This file contains engine function (hopefully useful)utilities.
 *
 *  Should not cause everything to break :)
 */

#pragma once

#ifndef ENGINE_UTILS_HPP
#define ENGINE_UTILS_HPP

#include "AETypedefs.hpp"
#include "AEFlags.hpp"
#include <string>
#include <algorithm>
#include <ctime>
#include <sstream>

//function-like macros
//since it's more convenient to have them as a macro
//because...syntax

/// Macro for the calculation of static, compile-time array
/// <param name="arr">The array to calculate the size of</param>
#define arrsize(arr) (sizeof(arr) / sizeof(arr[0]) )

/// Macro for the check if modulo of x and y results in 0
/// <param name="dvd">The dividend of the operation</param>
/// <param name="dvs">The divisor of the operation</param>
#define mod0(dvd,dvs) !(dvd % dvs)

/// Macro to get the bit of the given value (bitwise operations)
/// <param name="val">The value to get the bit of</param>
/// <param name="bit">The bit number to get</param>
#define getBit(val, bit) (val& (1 << bit))

/// Better-fomatted assertion for runtime checks
/// <param name="expr">The logical expression to assert</param>
/// <param name="msg">The message to display if the assertion fails</param>
#define aceAssert(expr, msg) if ( !(expr) ) { std::cerr << "Assertion failed!\nExpression: " << #expr << "\nFile: " << __FILE__ << "\nLine: " << __LINE__ << "\nMessage: " << msg << std::endl; abort(); }

/// Macro for the debug printf() printing, use the same way as you have used printf before.
/// @note Prints additional newline with each print
/// @note Does nothing if ENGINE_DEBUG is not defined and set to 1
#if ENGINE_DEBUG
#define dprintf(...) if constexpr (ENGINE_DEBUG) { printf("DEBUG::%s()-> ", __FUNCTION__); printf(__VA_ARGS__); printf("\n"); }
#else
#define dprintf(...) 
#endif

/// Macro to check if the given type T is about the same as Y
/// Evaluates to true if both types (not caring about const-ness) are same
/// @note It decays both types and omits const-ness
/// <param name="T">The first type to compare</param>
/// <param name="Y">The second type to compare</param>
#define IS_SAME_NOC(T,Y) (std::is_same<typename std::decay<const T>::type, typename std::decay<const Y>::type>::value)

/// Macro to convert the normal string literal to utf-8/unicode string literal; u8"Hello World!"
/// <param name="x">The string literal to convert to utf8</param>
#define UTF8(x) u8 ## x //converts the "string" to the u8"string"
/// Macro to convert the "string literal" to L"wide string literal"
/// <param name="x">The string literal to convert to wstring</param>
#define WSTR(x) L ## x //converts the "string" to the L"string"

/// Offsetof alternative if the compiler doesn't support the offsetof natively
#ifdef offsetof
#define aeoffsetof(type, memb) offsetof(type, memb)
#else
#define aeoffsetof(type, memb) (intptr_t)&(((type*)NULL)->memb)
#endif //offsetof


//other stuff
/// Macro for newline char
#define NLC '\n'

/// Macro for newline string
#define NLS "\n"

/// Size of the string of the formatted date-time by the ace::utils::formatDate()
#define DATETIME_STRING_SIZE 20


// stuff for crosscompilation and 64bit access
// dont touch
#ifdef _WIN32
#define ftell _ftelli64
#define fseek _fseeki64
#else
#define ftell ftello
#define fseek fseeko
#endif


///This namespace contains all of things that any of the part of program can use, and is useful.
///Such things as flags, utilities, handles to windows, status(we're starting or closing), etc.
///Basically everything for everyone and public access
namespace ace {

	///Global usefull utilities
	namespace utils {

		/// <summary>
		/// Converts bool to the word version
		/// </summary>
		/// <param name="b">The boolean to convert</param>
		/// <returns>const char* of "true" or "false"</returns>
		constexpr const char* boolToString(const bool b) noexcept {
			return (b) ? "true" : "false";
		}

		/// <summary>
		/// Checks if the value that you passed is within range
		/// @note The type of the values you pass to this function shall be the same
		/// </summary>
		/// <typeparam name="T">The type of values to compare</typeparam>
		/// <param name="minval">Minimum value of the range of values</param>
		/// <param name="maxval">Maximum value of the range of values</param>
		/// <param name="i">The value to check</param>
		/// <returns>True if the value is in the range, false otherwise</returns>
		template<typename T>
		constexpr bool isInRange(const T minval, const T maxval, const T i) noexcept {
			return (minval <= i) && (i <= maxval);
		}

		/// <summary>
		/// Lowers the passed std::string by reference
		/// </summary>
		/// <param name="str1">The string to be lowered</param>
		inline void toLowerRef(std::string& str1) {
			std::transform(str1.begin(), str1.end(), str1.begin(), tolower);
		}

		/// <summary>
		/// Returns the lowered version of passed std::string
		/// </summary>
		/// <param name="str1">The string to be lowered</param>
		/// <returns>The lowered version of the string</returns>
		inline std::string toLowerVal(std::string str1) { // we aren't passing by const reference....because we'll make a copy anyway
			std::transform(str1.begin(), str1.end(), str1.begin(), tolower);
			return str1;
		}

		/// <summary>
		/// Uppers the passed std::string by reference
		/// </summary>
		/// <param name="str1">The string to be uppered</param>
		inline void toUpperRef(std::string& str1) {
			std::transform(str1.begin(), str1.end(), str1.begin(), toupper);
		}

		/// <summary>
		/// Returns the uppered version of passed std::string
		/// </summary>
		/// <param name="str1">The string to be uppered</param>
		/// <returns>The uppered version of the string</returns>
		inline std::string toUpperVal(std::string str1) { // we aren't passing by const reference....because we'll make a copy anyway
			std::transform(str1.begin(), str1.end(), str1.begin(), toupper);
			return str1;
		}
		
		/// <summary>
		/// Checks if the given string is a numerical value
		/// </summary>
		/// <param name="strnum">C-string of the "number" to check</param>
		/// <returns>True if that string is indeed a number, false otherwise</returns>
		constexpr bool isNum(const char* const strnum) noexcept {
			//null pointer check
			if (!strnum) {
				return false;
			}

			const std::size_t len = std::strlen(strnum);

			//is it 0?
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
			//quick check, if string has 0 as the first number...hey that's invalid!
			if (strnum[i] == '0' && (i == 1 || len > 1)) {
				return false;
			}
			for (; i < len; i++) {
				if (strnum[i] < '0' || strnum[i] > '9') {
					return false;
				}
			}
			//passed
			return true;
		}

		/// <summary>
		/// Formats the current (c)time point as YYYY-MM-DD.HH:mm:SS
		/// @warning If -1 was passed, fails and returns empty string;
		/// </summary>
		/// <param name="timept">C time point</param>
		/// <returns>std::string of the formatted string</returns>
		inline std::string formatDate(const time_t timept) {
			if (timept == -1) {
				return "";
			}

			struct tm tstruct { 0 };
#ifdef _WIN32
			localtime_s(&tstruct, &timept);
#else
			tstruct = *localtime(&timept);
#endif
			char buff[DATETIME_STRING_SIZE]{};
			snprintf(buff, sizeof(buff), "%04d-%02d-%02d.%02d:%02d:%02d",
				tstruct.tm_year + 1900, tstruct.tm_mon + 1,
				tstruct.tm_mday, tstruct.tm_hour, tstruct.tm_min,
				tstruct.tm_sec);
			return buff;
		}

		/// <summary>
		/// Formats the current (c)time point as YYYY-MM-DD.HH:mm:SS
		/// @warning If -1 was passed, fails and doesn't modify the string contents
		/// @warning On fail, returns the passed string.
		/// </summary>
		/// <param name="timept">C time point</param>
		/// <param name="str">The c-string to write the value to. Must be at least 20 bytes long (19 characters with 1 null terminator)</param>
		/// <returns></returns>
		inline char* formatDate(const time_t timept, char* const str) noexcept {
			if (timept == -1 || !str) {
				return str;
			}

			struct tm tstruct { 0 };
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
		/// Retrieves the current date and time and formats it in the YYYY-MM-DD.HH:mm:SS
		/// </summary>
		/// <returns>std::stringof the current date in YYYY-MM-DD.HH:mm:SS format</returns>
		inline std::string getCurrentDate(void) {
			return ace::utils::formatDate(time(nullptr));
		}

		inline const time_t stringToDate(const char* const timestr, const char* const timeformat = "%Y-%m-%d.%X") {
			struct tm tstruct { 0 };
			std::istringstream iss(timestr);
			iss >> std::get_time(&tstruct, timeformat);

			// use system timezone databases for...proper time shift;
			// thanks Michael Paxton and Lightness-Races-in-Orbit: https://stackoverflow.com/a/55475752
			tstruct.tm_isdst = -1; 
			return mktime(&tstruct);
		}


		/// <summary>
		/// Converts the hex address value to string
		/// </summary>
		/// <param name="myptr">Pointer to whatever memory adress</param>
		/// <returns>std::string with the hexadecimal version of the adress</returns>
		inline std::string addrToStr(const void* const myptr) {//adds address value to string
			char buf[int(sizeof(void*) * 2.5)]{}; 
			snprintf(buf, sizeof(buf), "0x%p", myptr);
			return buf;
		}
		
		/// <summary>
		/// Convenient sleep function to sleep the thread for ms milliseconds
		/// @note May be inaccurate in delay (as...all sleep functions are)
		/// </summary>
		/// <param name="ms">The amount of milliseconds to sleep</param>
		inline void sleepMS(const int ms) {
			sleepFor(milliSec(ms));
		}

		/// <summary>
		/// Convenient sleep function to sleep the thread for ms microseconds
		/// @note May be inaccurate in delay (as...all sleep functions are)
		/// </summary>
		/// <param name="us">The amount of microseconds to sleep</param>
		inline void sleepUS(const int us) {
			sleepFor(microSec(us));
		}

		/// <summary>
		/// fopen-cross-compiler --just a cross-compiler wrapper of FILE's fopen to do it properly.
		/// Opens the file with given name and flags.
		/// Needed so i dont need to retype ifdef for each of file openings
		/// Makes code cleaner...relatively
		/// </summary>
		/// <param name="fname">File name, as you'd pass to fopen</param>
		/// <param name="flags">File flags, as you'd pass to fopen</param>
		inline FILE* fopenCC(const char* const fname, const char* const flags) noexcept {
			//safety, so our compiler shuts up about the unsafe and deprecated function
			//and trigger only on vc++
			FILE* filestr = nullptr;
//if our compiler is vc++
#ifdef _WIN32 
			fopen_s(&filestr, fname, flags);
#else
//or some other stuff
			filestr = fopen(fname, flags);
#endif // _WIN32 
			return filestr;
		}

		/// <summary>
		/// Converts the given character value to int ('0' to 0), if it is within the '0'->'9' range
		/// </summary>
		/// <param name="c">The character value to convert</param>
		/// <returns>int's 0-9 values if the given char value is within range, -1 otherwise</returns>
		constexpr int charToInt(const char c) noexcept {
			return (ace::utils::isInRange<char>('0', '9', c)) ? (c - '0') : -1;
		}
		
		/// <summary>
		/// Converts the given int value to char (0 to '0'), if it is within the 0->9 range
		/// </summary>
		/// <param name="c">The integer value to convert</param>
		/// <returns>char's '0'->'9' values if the given char value is within range, -1 otherwise</returns>
		constexpr char intToChar(const int i) noexcept {
			return (ace::utils::isInRange<int>(0, 9, i)) ? (i + '0') : -1;
		}

		//alphanumeric string
		constexpr inline bool isAlNum(const std::string_view str) {
			return std::find_if_not(str.begin(), str.end(), [](char c) noexcept { return std::isalnum(c); }) == str.end();
		}

		//alphanumeric + underscore string
		constexpr inline bool isAlNumUs(const std::string_view str) {
			return std::find_if_not(str.begin(), str.end(), [](char c) noexcept { return std::isalnum(c) || c == '_'; }) == str.end();
		}

	}	
}

#endif // !ENGINE_UTILS_HPP
