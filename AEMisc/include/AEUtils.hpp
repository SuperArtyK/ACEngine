
/*! @file AEMisc/include/AEUtils.hpp
 *  
 *  @brief This file contains the engine's **usefull utilities for trivial tasks**.
 *  
 *  Stuff that I found that I needed to implement to perform some simple task, 
 *  like convert the bool value to "true"/"false" string.
 *  
 *  But then thought that the engine would benefit from by making it an engine-wide utility.
 *  
 *  Sooooo, lLess copy-pasted code, yayyy.
 *  
 *  @date 2023/09/22
 *
 *	@author	Artemii Kozhemiak (SuperArtyK)
 *
 *	@copyright MIT License
 *	
 *  *Should not cause everything to break :)*
 */

#pragma once

#ifndef ENGINE_UTILS_HPP
#define ENGINE_UTILS_HPP



#include "AEUtilsMacros.hpp"
#include "AEFlags.hpp"
#include "AETypedefs.hpp"
#include <algorithm>
#include <string>
#include <cstring>
#include <ctime>
#include <sstream>
#include <iomanip>


/// This namespace contains **everything that belongs to the engine**.
/// 
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
		[[nodiscard]] constexpr const std::string_view boolToString(const bool b) noexcept {
			return (b) ? "true" : "false";
		}

		/// <summary>
		/// Checks if the **value is within some range**.
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
		[[nodiscard]] constexpr bool isInRange(const T minval, const Y maxval, const U i) noexcept {
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
		[[nodiscard]] inline std::string toLowerVal(std::string str1) { // we aren't passing by const reference....because we'll make a copy anyway
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
		[[nodiscard]] inline std::string toUpperVal(std::string str1) { // we aren't passing by const reference....because we'll make a copy anyway
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
		[[nodiscard]] constexpr bool isNum(const std::string_view strnum) noexcept {
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
		[[nodiscard]] inline std::string formatDate(const std::time_t timept) {
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
		/// 
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
		/// 
		/// @see ace::utils::formatDate(const std::time_t)
		/// </summary>
		/// <returns>
		///		**std::string** of the current date in **YYYY-MM-DD.HH:mm:SS** format
		///	</returns>
		[[nodiscard]] inline std::string getCurrentDate(void) {
			return ace::utils::formatDate(std::time(nullptr));
		}

		/// <summary>
		/// Converts the given c-string to a (c)time point (std::time_t), using a set format.
		/// 
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
		[[nodiscard]] inline std::time_t stringToDate(const char* const timestr, const char* const timeformat = "%Y-%m-%d.%X") {
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
		/// 
		/// This keeps the leading zeros of the address. So passing the nullptr, which is 0, will result in something like 0x00000000.
		/// 
		/// @note The pointer value is not evaluated (passing nullptr is fine :) )
		/// </summary>
		/// <param name="myptr">Pointer to whatever memory adress</param>
		/// <param name="add0x">Flag to whether include the "0x" before the address value</param>
		/// <returns>
		///		**std::string** with the hexadecimal version of the passed address
		///	</returns>
		[[nodiscard]] inline std::string addrToStr(const void* const myptr, const bool add0x = true) noexcept {
			if (add0x) {
				char myarr[sizeof(void*) * 2 + 3];
				std::snprintf(myarr, sizeof(myarr), "0x%0*llX", int(sizeof(void*)) * 2, ullint(std::uintptr_t(myptr)));
				return myarr;
			}
			// add0x is false
			char myarr[sizeof(void*) * 2 + 1];
			std::snprintf(myarr, sizeof(myarr), "%0*llX", int(sizeof(void*)) * 2, ullint(std::uintptr_t(myptr)));
			return myarr;
		}
		
		/// <summary>
		/// Convenient sleep function to **sleep the thread for milliseconds**.
		/// 
		/// Uses #sleepFor() inside.
		/// 
		/// @note May be inaccurate in delay (as...all sleep functions are)
		/// @see #sleepFor()
		/// @see #milliSec()
		/// </summary>
		/// <param name="ms">The amount of milliseconds to sleep</param>
		inline void sleepMS(const int ms) noexcept {
			sleepFor(milliSec(ms));
		}

		/// <summary>
		/// Convenient sleep function to **sleep the thread for microseconds**.
		/// 
		/// Uses #sleepFor() inside.
		/// 
		/// @note May be inaccurate in delay (as...all sleep functions are)
		/// @see #sleepFor()
		/// @see #microSec()
		/// </summary>
		/// <param name="us">The amount of microseconds to sleep</param>
		inline void sleepUS(const int us) noexcept {
			sleepFor(microSec(us));
		}

		/// <summary>
		/// A wrapper to make the proper **cross-compiler version of fopen()**.
		/// 
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
		[[nodiscard]] inline std::FILE* fopenCC(const char* const fname, const char* const flags) noexcept {
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
		/// Converts the given **numeric char to int of type T**.
		/// 
		/// @remark Requires the type **T** to be an integral type
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
		[[nodiscard]] constexpr T numCharToInt(const char c) noexcept requires(std::is_integral<T>::value) {
			return (ace::utils::isInRange<char, char, char>('0', '9', c)) ? (c - '0') : -1;
		}
		
		/// <summary>
		/// Converts the given **int of type T to numeric char**.
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
		[[nodiscard]] constexpr char intToNumChar(const T i) noexcept {
			return (ace::utils::isInRange<T, T, T>(0, 9, i)) ? (i + '0') : -1;
		}

		/// <summary>
		/// Checks **if the passed string is alpha-numeric** (contains only numbers and english letters).
		/// </summary>
		/// <param name="str">The passed string to check</param>
		/// <returns>
		///		If the string contains *only* numbers(0-9) or alphabetic characters(A-Z, a-z):
		///		* **True**
		///		
		///		Otherwise:
		///		* **False**
		///	</returns>
		[[nodiscard]] constexpr inline bool isAlNum(const std::string_view str) {
			return std::find_if_not(str.begin(), str.end(), [](char c) noexcept { return std::isalnum(c); }) == str.end();
		}

		/// <summary>
		/// Checks **if the passed string is alpha-numeric with underscores** (contains only numbers, english letters, and underscores).
		/// </summary>
		/// <param name="str">The passed string to check</param>
		/// <returns>
		///		If the string contains *only* numbers(0-9), alphabetic characters(A-Z, a-z), or an underscore (_):
		///		* **True**
		///		
		///		Otherwise:
		///		* **False**
		///	</returns>
		[[nodiscard]] constexpr inline bool isAlNumUs(const std::string_view str) {
			return std::find_if_not(str.begin(), str.end(), [](char c) noexcept { return std::isalnum(c) || c == '_'; }) == str.end();
		}


		/// <summary>
		/// Converts the passed **byte value to hex** representation, into a passed c-string.
		/// @attention The c-string **str** needs to be at least 3 bytes in length (2 characters + null terminator)
		/// @note It sets the null-terminator as the 3rd character of the c-string
		/// </summary>
		/// <param name="ptr">The c-string to write the data into</param>
		/// <param name="bt">The byte to convert</param>
		/// <returns>
		///		If **nullptr** pas passed for the **ptr** argument:
		///		* **nullptr**
		///		
		///		Otherwise:
		///		* The passed c-string **ptr** containing the hex representation of the passed byte.
		/// </returns>
		inline constexpr const char* const byteToHex(char* const ptr, const ucint bt) noexcept {

			constexpr char hexUpper[]{
				'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
				'A', 'B', 'C', 'D', 'E', 'F',
			};

			if (ptr) {
				ptr[0] = hexUpper[bt / 16];
				ptr[1] = hexUpper[bt % 16];
				ptr[2] = '\0';
			}
			
			return ptr;
		}

		/// <summary>
		/// Converts the passed **byte value to hex** representation.
		/// </summary>
		/// <param name="bt">The byte to convert</param>
		/// <returns>
		///		The passed byte as hex as **std::string type**; it has the length 2.
		/// </returns>
		[[nodiscard]] inline std::string byteToHex(const ucint bt) {
			std::string result = "00";
			ace::utils::byteToHex(result.data(), bt);
			return result;
		}

		/// <summary>
		/// Reads the raw data at a given address and **converts the read bytes to hex**.
		/// </summary>
		/// <param name="myptr">The pointer to memory address to read from</param>
		/// <param name="szRead">The amount of bytes to read</param>
		/// <param name="useDeilm">A flag to whether use any delimeter between the bytes</param>
		/// <param name="delim">The string representing the delimeter</param>
		/// <returns>
		///		The raw bytes at the memory address as hex as the **std::string** type.
		/// </returns>
		[[nodiscard]] inline std::string readBytesAsHex(const void* const myptr, const std::size_t szRead, const bool useDeilm = true, const std::string_view delim = " ") {
			const ucint* const ptr = (const ucint*)myptr;
			
			char strhex[] = "00";

			std::string result;
			result.reserve(szRead * 2 + (useDeilm)? delim.size() * szRead : 0);
			for (std::size_t i = 0; i < szRead; i++) {
				result.append(byteToHex(strhex, ptr[i]));
				if (useDeilm) {
					result.append(delim);
				}
			}
			return result;
		}


	}	
}

#endif // !ENGINE_UTILS_HPP
