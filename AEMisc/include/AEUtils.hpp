#pragma once

#ifndef ENGINE_UTILS_HPP
#define ENGINE_UTILS_HPP

#include "AETypedefs.hpp"
#include <string>
#include <algorithm>
#include <ctime>
#include <sstream>


/// Macro for the calculation of static, compile-time array
#define arrsize(x) (sizeof(x)/sizeof(x[0]))

/// Macro for the check if modulo of x and y results in 0
#define mod0(x,y) !(x%y)

/// Macro to check if the given type T is about the same as Y
/// @note It decays both types and omits const-ness
#define IS_SAME_NOC(T,Y) (std::is_same<typename std::decay<const T>::type, typename std::decay<const Y>::type>::value)

/// Macro to convert the normal string literal to utf-8/unicode string literal; u8"Hello World!"
#define UTF8(x) u8 ## x //converts the "string" to the u8"string"
/// Macro to convert the "string literal" to L"wide string literal"
#define WSTR(x) L ## x //converts the "string" to the L"string"

//other stuff
/// Macro for newline char
#define NLC '\n'

/// Macro for newline string
#define NLS "\n"


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
		constexpr const char* boolToString(const bool b) {
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
		constexpr bool isInRange(const T minval, const T maxval, const T i) {
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
		constexpr bool isNum(const char* strnum) { 
			//null pointer check
			if (!strnum) {
				return false;
			}

			std::size_t len = std::strlen(strnum);

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
		/// </summary>
		/// <param name="timept">C time point</param>
		/// <returns>std::string of the formatted string</returns>
		inline std::string formatDate(const time_t timept) {
			struct tm tstruct;
#ifdef _WIN32
			localtime_s(&tstruct, &timept);
#else
			tstruct = *localtime(&timept);
#endif
			char buff[20]{};
			strftime(buff, sizeof(buff), "%Y-%m-%d.%X", &tstruct);
			return buff;
		}

		/// <summary>
		/// Retrieves the current date and time and formats it in the YYYY-MM-DD.HH:mm:SS
		/// </summary>
		/// <returns>std::stringof the current date in YYYY-MM-DD.HH:mm:SS format</returns>
		inline std::string getCurrentDate(void) {//
			return ace::utils::formatDate(time(NULL));
		}

		

		/// <summary>
		/// Converts the hex address value to string
		/// </summary>
		/// <param name="myptr">Pointer to whatever memory adress</param>
		/// <returns>std::string with the hexadecimal version of the adress</returns>
		inline std::string addrToStr(const void* myptr) {//adds address value to string
			std::ostringstream oss;
			oss << "0x"<< myptr;
			return oss.str();
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
		/// <param name="ms">The amount of microseconds to sleep</param>
		inline void sleepUS(const int us) {
			sleepFor(microSec(us));
		}
	}	
}

#endif // !ENGINE_UTILS_HPP
