
/*! @file AEFileReader/include/AEFileReader.hpp
 *  This file contains the code for the engine's file reader.
 *  Which is just a small wrapper around the c's FILE api, for speed and convenience.
 *  One note when using it though: file is blocked from access by other programs when open.
 *  But maybe that's a good feature ;)
 *
 *  Should not cause everything to break :)
 */

#pragma once

#ifndef ENGINE_FILE_READER_HPP
#define ENGINE_FILE_READER_HPP

#include "include/AEModuleBase.hpp"
#include "include/AEUtils.hpp"
#include <atomic>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
#include <cstdio>


// Do NOT touch!

// crosscompile stuff
// do not touch
#ifdef _WIN32
#define fscanf fscanf_s
#endif


//Error flags
/// Macro for the indicator that everything is good/no error was encountered in the process
#define AEFR_ERR_NOERROR ENGINE_MODULE_ERR_NOERROR

/// same as AEFR_ERR_NOERROR - value of a successfull read.
#define AEFR_ERR_READ_SUCCESS 0

/// Macro for the error if file isn't open and file operation functions of the file reader are used.
#define AEFR_ERR_FILE_NOT_OPEN -1


//file creation/manipulation
/// Macro for the error if the file name is empty.
#define AEFR_ERR_OPEN_FILE_NAME_EMPTY -2

/// Macro for the error if the file couldn't be created for some other reason, like missing permissions to access files.
#define AEFR_ERR_OPEN_FILE_DOESNT_EXIST -3

/// Macro for the error that occurs when trying to open the file, while the AEFileReader instance already has an opened file
#define AEFR_ERR_OPEN_FILE_ALREADY_OPENED -4

/// Macro for the error occurring when end of file was reached when reading the file.
#define AEFR_ERR_READ_EOF -5

/// Macro for the reading error that occurred when reading the file (look up ferror() ).
#define AEFR_ERR_READ_ERROR -6

/// Macro for the error that occurs if the data pointer, item count and item size is null/zero in AEFileReader::readData_ptr().
#define AEFR_ERR_READ_ZERO_SIZE -7

/// Macro for the error that occurs if invalid cursor origin (not SEEK_SET, SEEK_CUR, or SEEK_END) was passed to AEFileReader::setCursorPos()
#define AEFR_ERR_WRONG_CURSOR_ORIGIN -8

//macros for code
/// Macro for the shortened "check for opened file, set error flag and return error flag if closed", DO NOT TOUCH!
#define _AEFR_EXIT_ON_CLOSED_FILE if (this->isClosed()) { return AEFR_ERR_FILE_NOT_OPEN; }
/// Macro for the shortened "check for opened file during the read operation, set error flag and return error flag if closed", DO NOT TOUCH!
#define _AEFR_EXIT_ON_READ_CLOSED_FILE if (this->isClosed()) { this->m_szLastReadAmount = 0; return AEFR_ERR_FILE_NOT_OPEN; }


/// <summary>
/// ArtyK's Engine File Reader; umm, it reads data from the given file.
/// It is a wrapper around the C's FILE api, for speed and convenience.
/// It can read strings (untill: max string size, newline, NULL character), bools, ints, and floats
/// 
/// Just create it, read stuff, and dump the terabytes of data from the file to your memory and what not.
/// Hungarian notation is fr. (m_frMyFileReader)
/// Flags start with AEFR_
/// 
/// @warning This is not thread-safe!
/// </summary>
/// @todo add generic read function
class AEFileReader : public __AEModuleBase<AEFileReader> {

public:

//constructors
	/// <summary>
	/// Class constructor -- constructs the instance and opens the file of given name.
	/// @see AEFileReader::openFile()
	/// </summary>
	/// <param name="fname">The file name to opens</param>
	explicit AEFileReader(const std::string_view fname);

	/// <summary>
	/// Class constructor -- constructs the instance with default values, and doesn't open the file.
	/// </summary>
	AEFileReader(void) noexcept : m_sFilename(""), m_ullTotalReads(0), m_szLastReadAmount(0), m_fpFilestr(nullptr) {}

	/// <summary>
	/// Class move constructor
	/// </summary>
	AEFileReader(AEFileReader&& fr) noexcept;

//we don't need those
	/// <summary>
	/// Deleted copy constructor. 
	/// There is no need to copy AEFR, since access to file is in instance's FILE pointer.
	/// If in original instance, the file gets closed, the pointer is invalidated.
	/// Which can lead to...bad consequences using it again in the copied instance.
	/// </summary>
	AEFileReader(const AEFileReader&) = delete;

	/// <summary>
	/// Deleted copy assignment operator.
	/// There is no need to copy AEFR, since access to file is in instance's FILE pointer.
	/// If in original instance, the file gets closed, the pointer is invalidated.
	/// Which can lead to...bad consequences using it again in the copied instance.
	/// </summary>
	AEFileReader& operator=(const AEFileReader&) = delete;

	/// <summary>
	/// Default Destructor. Just flushes and closes the file.
	/// </summary>
	~AEFileReader(void) {
		this->closeFile();
	}


//file opening
	/// <summary>
	/// Opens the file for reading.
	/// </summary>
	/// <param name="fname">The name of the file to open</param>
	/// <returns>AEFR_ERR_NOERROR if file was opened successfully; otherwise AEFR_ERR_OPEN_* flags (like AEFR_ERR_OPEN_FILE_NAME_EMPTY)</returns>
	cint openFile(const std::string_view fname); //defined after class

	/// <summary>
	/// Closes the currently opened file.
	/// </summary>
	/// <returns>AEFR_ERR_NOERROR if file was closed successfully; AEFR_ERR_FILE_NOT_OPEN if file isn't open</returns>
	inline cint closeFile(void) noexcept {
		_AEFR_EXIT_ON_CLOSED_FILE;

		fclose(this->m_fpFilestr);
		this->m_fpFilestr = nullptr;
		this->m_szLastReadAmount = 0;
		this->m_sFilename.clear();
		return AEFR_ERR_NOERROR;
	}


//read stuff
	/// <summary>
	/// Reads the bytes (size of T) from the file and dumps/reinterprets them as the value in the passed variable.
	/// @note If EOF/Error was encountered when reading, the rest of bytes that weren't filled are zeroed
	/// </summary>
	/// <typeparam name="T">The type of the variable passed</typeparam>
	/// <param name="var">The variable to fill with bytes</param>
	/// <returns>AEFR_ERR_READ_SUCCESS on success; otherwise AEFR_ERR_FILE_NOT_OPEN if file isn't open, or other AEFR_ERR_READ_* flags (like AEFR_ERR_READ_EOF)</returns>
	template<typename T>
	inline cint readVariable(T& var) noexcept {
		_AEFR_EXIT_ON_READ_CLOSED_FILE;
		std::memset(&var, AENULL, sizeof(T));
		return this->readData_ptr(&var, sizeof(T), 1);
	}

//read strings as normal
	/// <summary>
	/// Reads the given amount of bytes and interprets them as the string.
	/// @note Modifies the length of the std::string to the dcount+1 size (to accomodate for the trailing null-termiantion character).
	/// @note If the resulting data size is less than dcount (like from EOF), resizes it to the amount read.
	/// @note If the file is closed, it doesn't modify the string
	/// </summary>
	/// <param name="str">The refernce to the std::string object to read the bytes of string to</param>
	/// <param name="dcount">The amount of characters to read</param>
	/// <returns>AEFR_ERR_READ_SUCCESS on success; otherwise AEFR_ERR_FILE_NOT_OPEN if file isn't open, or other AEFR_ERR_READ_* flags (like AEFR_ERR_READ_EOF)</returns>
	inline cint readString(std::string& str, const std::size_t dcount) {
		_AEFR_EXIT_ON_READ_CLOSED_FILE;
		str.resize(dcount + 1);
		const cint temp = this->readString(str.data(), dcount);
		if (temp) {
			str.resize(this->m_szLastReadAmount);
		}
		return temp;
	}

	/// <summary>
	/// Reads the given amount of bytes and interprets them as the string.
	/// @note Modifies the length of the std::vector<char> to the dcount+1 size (to accomodate for the trailing null-termiantion character).
	/// @note If the resulting data size is less than dcount (like from EOF), resizes it to the amount read (+1 for the null termination).
	/// @note If the file is closed, it doesn't modify the std::vector<char>
	/// </summary>
	/// <param name="str">The refernce to the std::vector<char> object to read the bytes of string to</param>
	/// <param name="dcount">The amount of characters to read</param>
	/// <returns>AEFR_ERR_READ_SUCCESS on success; otherwise AEFR_ERR_FILE_NOT_OPEN if file isn't open, or other AEFR_ERR_READ_* flags (like AEFR_ERR_READ_EOF)</returns>
	inline cint readString(std::vector<char>& str, const std::size_t dcount) {
		_AEFR_EXIT_ON_READ_CLOSED_FILE;
		str.resize(dcount + 1); //+1 for the null terminator
		const cint temp = this->readString(str.data(), dcount);
		if (temp) {
			str.resize(this->m_szLastReadAmount + 1);
		}
		return temp;
	}
	
	/// <summary>
	/// Reads the given amount of bytes and interprets them as the string.
	/// @note The passed string must be at least dcount+1 characters long (+1 is for the trailing null termination)
	/// @note If the resulting data size is less than dcount (like from EOF), fills the rest of unfilled characters with NULL
	/// @note If the file is closed, it doesn't modify the data
	/// </summary>
	/// <param name="str">The pointer to the c-string to read the bytes of string to</param>
	/// <param name="dcount">The amount of characters to read</param>
	/// <returns>AEFR_ERR_READ_SUCCESS on success; otherwise AEFR_ERR_FILE_NOT_OPEN if file isn't open, or other AEFR_ERR_READ_* flags (like AEFR_ERR_READ_EOF)</returns>
	cint readString(char* str, const std::size_t dcount) noexcept; //defined below class


//read string untill new linecharacter

	/// <summary>
	/// Reads the characters to a std::string untill a newline (included), or untill the maximum character amount -- whichever comes first.
	/// @note Modifies the length of the std::string to the dcount+1 size (to accomodate for the trailing null-termiantion character).
	/// @note If the resulting data size is less than dcount (like from EOF or met newline), resizes it to the amount read.
	/// @note If the file is closed, it doesn't modify the string
	/// </summary>
	/// <param name="str">The refernce to the std::string object to read the bytes of string to</param>
	/// <param name="dcount">The maximum amount of characters to read</param>
	/// <returns>AEFR_ERR_READ_SUCCESS on success; otherwise AEFR_ERR_FILE_NOT_OPEN if file isn't open, or other AEFR_ERR_READ_* flags (like AEFR_ERR_READ_EOF)</returns>
	inline cint readStringNL(std::string& str, const int dcount) {
		_AEFR_EXIT_ON_READ_CLOSED_FILE;
		str.resize((std::size_t)dcount + 1); //+1 because of null termination..since we write to it directly
		const cint temp = this->readStringNL(str.data(), dcount);
		str.resize(this->m_szLastReadAmount); // resize to the string size (in case we got eof/newline)
		return temp;
	}

	/// <summary>
	/// Reads the characters to a std::vector<char> untill a newline (included), or untill the maximum character amount -- whichever comes first.
	/// @note Modifies the length of the std::vector<char> to the dcount+1 size (to accomodate for the trailing null-termiantion character).
	/// @note If the resulting data size is less than dcount (like from EOF or met newline), resizes it to the amount read (+1 for the null termination).
	/// @note If the file is closed, it doesn't modify the std::vector<char>
	/// </summary>
	/// <param name="str">The refernce to the std::vector<char> object to read the bytes of string to</param>
	/// <param name="dcount">The maximum amount of characters to read</param>
	/// <returns>AEFR_ERR_READ_SUCCESS on success; otherwise AEFR_ERR_FILE_NOT_OPEN if file isn't open, or other AEFR_ERR_READ_* flags (like AEFR_ERR_READ_EOF)</returns>
	inline cint readStringNL(std::vector<char>& str, const int dcount) {
		_AEFR_EXIT_ON_READ_CLOSED_FILE;
		str.resize((std::size_t)dcount + 1);
		const cint temp = this->readStringNL(str.data(), dcount);
		str.resize(this->m_szLastReadAmount + 1); //we're resizing; +1 because vectors dont insert null terminator
		return temp;
	}

	/// <summary>
	/// Reads the characters to a c-string untill a newline (included), or untill the maximum character amount -- whichever comes first.
	/// @note The passed string must be at least dcount+1 characters long (+1 is for the trailing null termination)
	/// @note If the resulting data size is less than dcount (like from EOF or met newline), fills the rest of unfilled characters with NULL
	/// @note If the file is closed, it doesn't modify the data
	/// </summary>
	/// <param name="str">The pointer to the c-string to read the bytes of string to</param>
	/// <param name="dcount">The maximum amount of characters to read</param>
	/// <returns>AEFR_ERR_READ_SUCCESS on success; otherwise AEFR_ERR_FILE_NOT_OPEN if file isn't open, or other AEFR_ERR_READ_* flags (like AEFR_ERR_READ_EOF)</returns>
	cint readStringNL(char* str, const int dcount) noexcept; //defined below class


//read string untill null character
	/// <summary>
	/// Reads the characters to a std::string untill a NULL character(I guess, you can say "included"?), or untill the maximum character amount -- whichever comes first.
	/// @note Modifies the length of the std::string to the dcount+1 size (to accomodate for the trailing null-termiantion character).
	/// @note If the resulting data size is less than dcount (like from EOF or met null), resizes it to the amount read.
	/// @note If the file is closed, it doesn't modify the string
	/// </summary>
	/// <param name="str">The refernce to the std::string object to read the bytes of string to</param>
	/// <param name="dcount">The maximum amount of characters to read</param>
	/// <returns>AEFR_ERR_READ_SUCCESS on success; otherwise AEFR_ERR_FILE_NOT_OPEN if file isn't open, or other AEFR_ERR_READ_* flags (like AEFR_ERR_READ_EOF)</returns>
	inline cint readStringNULL(std::string& str, const llint dcount) {
		_AEFR_EXIT_ON_READ_CLOSED_FILE;
		str.resize(std::size_t(dcount + 1));
		const cint temp = this->readStringNULL(str.data(), dcount);
		str.resize(this->m_szLastReadAmount); // resize to the string size (untill null termination)
		return temp;
	}

	/// <summary>
	/// Reads the characters to a std::string untill a NULL character(I guess, you can say "included"?), or untill the maximum character amount -- whichever comes first.
	/// @note Modifies the length of the std::vector<char> to the dcount+1 size (to accomodate for the trailing null-termiantion character).
	/// @note If the resulting data size is less than dcount (like from EOF or met null), resizes it to the amount read (+1 for the null termination).
	/// @note If the file is closed, it doesn't modify the std::vector<char>
	/// </summary>
	/// <param name="str">The refernce to the std::vector<char> object to read the bytes of string to</param>
	/// <param name="dcount">The maximum amount of characters to read</param>
	/// <returns>AEFR_ERR_READ_SUCCESS on success; otherwise AEFR_ERR_FILE_NOT_OPEN if file isn't open, or other AEFR_ERR_READ_* flags (like AEFR_ERR_READ_EOF)</returns>
	inline cint readStringNULL(std::vector<char>& str, const llint dcount) {
		_AEFR_EXIT_ON_READ_CLOSED_FILE;
		str.resize(std::size_t(dcount + 1));
		const cint temp = this->readStringNULL(str.data(), dcount);
		str.resize(this->m_szLastReadAmount + 1); // resize to the string size (untill null termination); +1 because it doesnt insert null terminator
		return temp;
	}

	/// <summary>
	/// Reads the characters to a c-string untill a NULL character(I guess, you can say "included"?), or untill the maximum character amount -- whichever comes first.
	/// @note The passed string must be at least dcount+1 characters long (+1 is for the trailing null termination)
	/// @note If the resulting data size is less than dcount (like from EOF or met null), fills the rest of unfilled characters with NULL
	/// @note If the file is closed, it doesn't modify the data
	/// </summary>
	/// <param name="str">The pointer to the c-string to read the bytes of string to</param>
	/// <param name="dcount">The amount of characters to read</param>
	/// <returns>AEFR_ERR_READ_SUCCESS on success; otherwise AEFR_ERR_FILE_NOT_OPEN if file isn't open, or other AEFR_ERR_READ_* flags (like AEFR_ERR_READ_EOF)</returns>
	cint readStringNULL(char* str, const llint dcount) noexcept; 


//read int
	/// <summary>
	/// Reads the bytes (size of T) from the file and dumps/reinterprets them as the value in the passed integer.
	/// @note If EOF/Error was encountered when reading, the rest of bytes that weren't filled are zeroed
	/// @note If the file is closed, it doesn't modify the data
	/// @note AEFileReader::readVariable()
	/// </summary>
	/// <typeparam name="T">The type of the int passed</typeparam>
	/// <param name="num">The int to fill with bytes</param>
	/// <returns>AEFR_ERR_READ_SUCCESS on success; otherwise AEFR_ERR_FILE_NOT_OPEN if file isn't open, or other AEFR_ERR_READ_* flags (like AEFR_ERR_READ_EOF)</returns>
	template<typename T>
	inline cint readInt(T& num) noexcept requires(std::is_integral<T>::value == true) {
		//static_assert(std::is_integral<T>::value, "Cannot use non-integral types in AEFileReader::readInt()");
		return this->readVariable<T>(num);
	}

	/// <summary>
	/// Reads the string of numbers from the file and convert it to int.
	/// @note If the first character isn't numeric (or minus), the read fails and integer is set to 0
	/// @note If the file is closed, it doesn't modify the data
	/// </summary>
	/// <typeparam name="T">The type of the int passed</typeparam>
	/// <param name="num">The int to read the value to</param>
	/// <returns>AEFR_ERR_READ_SUCCESS on success; otherwise AEFR_ERR_FILE_NOT_OPEN if file isn't open, or other AEFR_ERR_READ_* flags (like AEFR_ERR_READ_EOF)</returns>
	template<typename T>
	inline cint readIntString(T& num) noexcept requires(std::is_integral<T>::value == true); //defined below class


//read floats
	/// <summary>
	/// Reads the bytes (size of T) from the file and dumps/reinterprets them as the value in the passed float.
	/// @note If EOF/Error was encountered when reading, the rest of bytes that weren't filled are zeroed
	/// @note If the file is closed, it doesn't modify the data
	/// @note AEFileReader::readVariable()
	/// </summary>
	/// <typeparam name="T">The type of the float passed</typeparam>
	/// <param name="num">The float to fill with bytes</param>
	/// <returns>AEFR_ERR_READ_SUCCESS on success; otherwise AEFR_ERR_FILE_NOT_OPEN if file isn't open, or other AEFR_ERR_READ_* flags (like AEFR_ERR_READ_EOF)</returns>
	template<typename T>
	inline cint readFloat(T& num) noexcept (std::is_floating_point<T>::value == true) {
		static_assert(std::is_floating_point<T>::value, "Cannot use non-float types in AEFileReader::readFloat()");
		return this->readVariable<T>(num);
	}

	/// <summary>
	/// Reads the string of numbers from the file and convert it to float.
	/// @note If the first character isn't numeric (or belonging to float formatting, the read fails and float is (mem)set to 0
	/// @note If the file is closed, it doesn't modify the data
	/// </summary>
	/// <typeparam name="T">The type of the float passed</typeparam>
	/// <param name="num">The float to read the value to</param>
	/// <returns>AEFR_ERR_READ_SUCCESS on success; otherwise AEFR_ERR_FILE_NOT_OPEN if file isn't open, or other AEFR_ERR_READ_* flags (like AEFR_ERR_READ_EOF)</returns>
	template<typename T>
	inline cint readFloatString(T& num) noexcept requires(std::is_floating_point<T>::value == true); //defined below class


//read misc
	/// <summary>
	/// Reads the byte from the file and dumps/reinterprets it as the value in the passed bool (null is false, anything else is true).
	/// @note If EOF/Error was encountered when reading, and no bytes were read - the bool is set to false 
	/// @note If the file is closed, it doesn't modify the data
	/// @note AEFileReader::readVariable()
	/// </summary>
	/// <param name="num">The bool to read</param>
	/// <returns>AEFR_ERR_READ_SUCCESS on success; otherwise AEFR_ERR_FILE_NOT_OPEN if file isn't open, or other AEFR_ERR_READ_* flags (like AEFR_ERR_READ_EOF)</returns>
	inline cint readBool(bool& num) noexcept {
		return this->readVariable<bool>(num);
	}

	/// <summary>
	/// Reads the string in the file, looking for "true" or "false" (case insensitive).
	/// @note If the read characters don't evaluate to "true" or "false", the read fails and bool is set to false
	/// @note If the file is closed, it doesn't modify the data
	/// </summary>
	/// <param name="num">The bool to read the value to</param>
	/// <returns>AEFR_ERR_READ_SUCCESS on success; otherwise AEFR_ERR_FILE_NOT_OPEN if file isn't open, or other AEFR_ERR_READ_* flags (like AEFR_ERR_READ_EOF)</returns>
	cint readBoolString(bool& num) noexcept;

	/// <summary>
	/// Reads the character from the file and writes you to a given char.
	/// @note If EOF/Error was encountered when reading, and no bytes were read - the char is set to 0 
	/// @note If the file is closed, it doesn't modify the data
	/// @note AEFileReader::readVariable()
	/// @note Same as AEFileReader::readByte()
	/// </summary>
	/// <param name="num">The char to read</param>
	/// <returns>AEFR_ERR_READ_SUCCESS on success; otherwise AEFR_ERR_FILE_NOT_OPEN if file isn't open, or other AEFR_ERR_READ_* flags (like AEFR_ERR_READ_EOF)</returns>
	inline cint readChar(signed char& num) noexcept {
		return this->readVariable<signed char>(num);
	}

	/// <summary>
	/// Reads the byte from the file and writes you to a given unsigned.
	/// @note If EOF/Error was encountered when reading, and no bytes were read - the char is set to 0 
	/// @note If the file is closed, it doesn't modify the data
	/// @note AEFileReader::readVariable()
	/// </summary>
	/// <param name="num">The char to read</param>
	/// <returns>AEFR_ERR_READ_SUCCESS on success; otherwise AEFR_ERR_FILE_NOT_OPEN if file isn't open, or other AEFR_ERR_READ_* flags (like AEFR_ERR_READ_EOF)</returns>
	inline cint readByte(unsigned char& num) noexcept {
		return this->readVariable<unsigned char>(num);
	}


//read binary
	
	/// <summary>
	/// Reads the bytes from the file and dumps them into the std::vector<unsigned char>.
	/// @note Modifies the length of the std::vector<unsigned char> to the dcount size
	/// @note If the resulting data size is less than dcount (like from EOF), resizes it to the amount read
	/// @note If the file is closed, it doesn't modify the std::vector<unsigned char>
	/// </summary>
	/// <param name="cdata">The reference to the std::vector<char> object to fill with bytes</param>
	/// <param name="dcount">The amount of bytes to read</param>
	/// <returns>AEFR_ERR_READ_SUCCESS on success; otherwise AEFR_ERR_FILE_NOT_OPEN if file isn't open, or other AEFR_ERR_READ_* flags (like AEFR_ERR_READ_EOF)</returns>
	inline cint readBytes(std::vector<unsigned char>& cdata, const std::size_t dcount) {
		_AEFR_EXIT_ON_READ_CLOSED_FILE;
		cdata.resize(dcount);
		const cint temp = this->readData_ptr(cdata.data(), dcount, sizeof(unsigned char));
		if (!temp) cdata.resize(this->m_szLastReadAmount); //resize to what was actually read
		return temp;
	}

	/// <summary>
	/// Reads the bytes from the file and dumps them into the given pointer.
	/// @note The data pointed by the cdata must be of at least dcount size!
	/// @note If the resulting data size is less than dcount (like from EOF), fills the rest of unfilled characters with NULL
	/// @note If the file is closed, it doesn't modify the data of the pointer
	/// @note Just a shortcut for the AEFileReader::readData_ptr()
	/// </summary>
	/// <param name="cdata">The pointer to the data to fill with bytes</param>
	/// <param name="dcount">The amount of bytes to read</param>
	/// <returns>AEFR_ERR_READ_SUCCESS on success; otherwise AEFR_ERR_FILE_NOT_OPEN if file isn't open, or other AEFR_ERR_READ_* flags (like AEFR_ERR_READ_EOF)</returns>
	inline cint readBytes(void* cdata, const std::size_t dcount) noexcept {
		std::memset(cdata, AENULL, dcount);
		return this->readData_ptr(cdata, dcount, sizeof(char));
	}


	/// <summary>
	/// Reads an array of dcount elements, each one with a size of dsize bytes, to memory block pointed by cdata.
	/// Also sets the last read amount variable to the read amount of elements. If it doesn't equal to dcount, then EOF or error was encountered during reading.
	/// @note The memory block pointed by the cdata must be of at least dcount size!
	/// @note If the file is closed, it doesn't modify the data of the pointer
	/// </summary>
	/// <param name="cdata">The memory block to read the bytes to</param>
	/// <param name="dcount">The amount of elements to read from the file</param>
	/// <param name="dsize">The size of each element</param>
	/// <returns>AEFR_ERR_READ_SUCCESS on success; otherwise AEFR_ERR_FILE_NOT_OPEN if file isn't open, or other AEFR_ERR_READ_* flags (like AEFR_ERR_READ_EOF)</returns>
	cint readData_ptr(void* cdata, const std::size_t dcount, const std::size_t dsize = sizeof(char)) noexcept; //defined below class


//file info get-setters
	/// <summary>
	/// Checks if a file is open by this file-reader.
	/// </summary>
	/// <returns>True if file is open, false if otherwise</returns>
	inline bool isOpen(void) const noexcept {
		return this->m_fpFilestr;//null if closed, something other if opened
	}

	/// <summary>
	/// Checks if a file isn't open by this file-reader.
	/// </summary>
	/// <returns>True if file is closed/not open, false if otherwise</returns>
	inline bool isClosed(void) const noexcept {
		return !this->isOpen();
	}

	/// <summary>
	/// Returns size of the file in bytes.
	/// </summary>
	/// <returns>File size in bytes if file is open; otherwise AEFR_ERR_FILE_NOT_OPEN</returns>
	inline llint getFileSize(void) const noexcept {
		_AEFR_EXIT_ON_CLOSED_FILE;

		const llint curpos = ftell(this->m_fpFilestr); // get current position
		fseek(this->m_fpFilestr, 0, SEEK_END); // change pos to end of file
		const llint endpos = ftell(this->m_fpFilestr); // get pos of file at the end; file size essentially
		fseek(this->m_fpFilestr, curpos, SEEK_SET); // return to original pos
		return endpos;
	}

	/// <summary>
	/// Returns the full name of currently open file, including the relative path, if such was provided.
	/// @note If no relative path was provided when opening the file, then output of AEFileReader::getFullFileName() is equivalent to AEFileReader::getFileName()
	/// </summary>
	/// <returns>std::string of the opened file (including relative path if was given); empty string otherwise</returns>
	inline std::string getFullFileName(void) const noexcept {
		return this->m_sFilename;
	}

	/// <summary>
	/// Returns the relative path of the opened file.
	/// If the file was opened in the same directory as the executable (no relative path provided), returns "./"
	/// </summary>
	/// <returns>std::string of the relative file path of opened file; empty string otherwise</returns>
	inline std::string getRelativePath(void) const {
		if (this->isClosed()) {
			return "";
		}
		const std::size_t found = this->m_sFilename.rfind('/');
		if (found != std::string::npos) {
			return this->m_sFilename.substr(0, found);
		}
		else {
			return "./";
		}
	}

	/// <summary>
	/// Returns the name of the opened file, devoid of the path
	/// </summary>
	/// <returns>std::string of the opened file name; emtpy string otherwise</returns>
	inline std::string getFileName(void) const {
		const std::size_t found = this->m_sFilename.rfind('/');
		if (found != std::string::npos) {
			return this->m_sFilename.substr(found, this->m_sFilename.size() - found);
		}
		else {
			return this->m_sFilename;
		}
	}

	/// <summary>
	/// Returns the full, absolute path of the opened file
	/// </summary>
	/// <returns>std::string of the absolute path of the opened file; empty string otherwise</returns>
	inline std::string getFullPath(void) const {
		if (this->isClosed()) {
			return "";
		}
		return std::filesystem::absolute(this->m_sFilename).generic_string();
	}

	/// <summary>
	/// Returns the file pointer of this file-reader.
	/// </summary>
	/// <returns>Pointer to FILE used in the file reader</returns>
	inline std::FILE* getFilePtr(void) const noexcept {
		return this->m_fpFilestr;
	}


//file cursor stuff
	/// <summary>
	/// Returns current read cursor position.
	/// </summary>
	/// <returns>Current read cursor position, starting from 0, if file is open; otherwise AEFR_ERR_FILE_NOT_OPEN</returns>
	inline llint getCursorPos(void) const noexcept {
		_AEFR_EXIT_ON_CLOSED_FILE;
		return ftell(this->m_fpFilestr);
	}

	/// <summary>
	/// Sets read cursor position to pos from origin.
	/// @note If cursor is beyond end of file, next read will trigger EOF error and will not read any data (just fill the given data place with NULL)
	/// @note If origin is not SEEK_SET, SEEK_CUR or SEEK_END returns AEFR_ERR_READ_EOF
	/// </summary>
	/// <param name="pos">Position to be set to relative to origin (same as "offset" in fseek)</param>
	/// <param name="origin">Relative origin for the operation. Google SEEK_SET, SEEK_CUR and SEEK_END for more details</param>
	/// <returns>0 on success; otherwise AEFR_ERR_FILE_NOT_OPEN if file's closed, AEFR_ERR_WRONG_CURSOR_ORIGIN on invalid origin, or things that fseek can return</returns>
	inline int setCursorPos(const llint pos, const int origin = SEEK_CUR) const noexcept {
		_AEFR_EXIT_ON_CLOSED_FILE;
		if(origin != SEEK_SET && origin != SEEK_CUR && origin != SEEK_END){
			return AEFR_ERR_READ_EOF;
		}
		return fseek(this->m_fpFilestr, pos, origin);
	}


//misc stuff
	/// <summary>
	/// Returns total reader requests made to file.
	/// </summary>
	/// <returns>Amount of times the read operation has been called on this AEFileReader instance</returns>
	inline ullint getTotalReads(void) const noexcept {
		return this->m_ullTotalReads.load(std::memory_order::relaxed);
	}

	/// <summary>
	/// Gets the last read amount of bytes from the opened file.
	/// @note If last operation failed and no bytes were read (closed file, full EOF) -- returns 0;
	/// </summary>
	/// <returns>std::size_t of the amount of bytes read in the last reading operation</returns>
	inline std::size_t getLastReadAmount(void) const noexcept {
		return this->m_szLastReadAmount;
	}

	/// <summary>
	/// Checks if the file (cursor) has reached it's (file's) end (feof)
	/// </summary>
	/// <returns>True if end of file was reached, false otherwise</returns>
	inline bool isEOF(void) const noexcept {
		return std::feof(this->m_fpFilestr);
	}

	/// <summary>
	/// Checks if error was encountered while reading the file data (ferror)
	/// </summary>
	/// <returns>True if end of file was reached, false otherwise</returns>
	inline bool isError(void) const noexcept {
		return std::ferror(this->m_fpFilestr);
	}


private:

	/// Full filename and relative path
	std::string m_sFilename;
	/// Counter for total read operations for file
	std::atomic<ullint> m_ullTotalReads;
	/// The amount of read bytes during last operation
	std::size_t m_szLastReadAmount;
	/// Object for file reading
	std::FILE* m_fpFilestr;

	//aaand register it
	REGISTER_MODULE(AEFileReader)
};



//and inline definitions of functions
template<typename T>
inline cint AEFileReader::readIntString(T& num) noexcept requires(std::is_integral<T>::value == true) {
	static_assert(std::is_integral<T>::value, "Cannot use non-integral types in AEFileReader::readIntString()");
	_AEFR_EXIT_ON_READ_CLOSED_FILE;

	std::memset(&num, AENULL, sizeof(T));
	cint temp = 0;
	if constexpr (IS_SAME_NOCV(T, signed char)) {
		temp = fscanf(m_fpFilestr, "%hhd", &num);
	}
	else if constexpr (IS_SAME_NOCV(T, unsigned char)) {
		temp = fscanf(m_fpFilestr, "%hhud", &num);
	}
	else if constexpr (IS_SAME_NOCV(T, short)) {
		temp = fscanf(m_fpFilestr, "%hd", &num);
	}
	else if constexpr (IS_SAME_NOCV(T, unsigned short)) {
		temp = fscanf(m_fpFilestr, "%hud", &num);
	}
	else if constexpr (IS_SAME_NOCV(T, int)) {
		temp = fscanf(m_fpFilestr, "%d", &num);
	}
	else if constexpr (IS_SAME_NOCV(T, unsigned int)) {
		temp = fscanf(m_fpFilestr, "%ud", &num);
	}
	else if constexpr (IS_SAME_NOCV(T, long)) {
		temp = fscanf(m_fpFilestr, "%ld", &num);
	}
	else if constexpr (IS_SAME_NOCV(T, unsigned long)) {
		temp = fscanf(m_fpFilestr, "%lud", &num);
	}
	else if constexpr (IS_SAME_NOCV(T, long long)) {
		temp = fscanf(m_fpFilestr, "%lld", &num);
	}
	else if constexpr (IS_SAME_NOCV(T, std::size_t)) {
		temp = fscanf(m_fpFilestr, "%zd", &num);
	}
	else {
		temp = fscanf(m_fpFilestr, "%llud", &num);
	}

	this->m_ullTotalReads.fetch_add(1, std::memory_order::relaxed);
	if (temp != 1) {
		return AEFR_ERR_READ_EOF;
	}
	return AEFR_ERR_READ_SUCCESS;
}

template<typename T>
inline cint AEFileReader::readFloatString(T& num) noexcept requires(std::is_floating_point<T>::value == true) {
	static_assert(std::is_floating_point<T>::value, "Cannot use non-float types in AEFileReader::readFloatString()");
	_AEFR_EXIT_ON_READ_CLOSED_FILE;

	std::memset(&num, AENULL, sizeof(T));
	cint temp = 0;
	if constexpr (IS_SAME_NOCV(T, float)) {
		temp = fscanf(m_fpFilestr, "%f", &num);
	}
	else if constexpr (IS_SAME_NOCV(T, double)) {
		temp = fscanf(m_fpFilestr, "%lf", &num);
	}
	else { //long double
		temp = fscanf(m_fpFilestr, "%Lf", &num);
	}

	this->m_ullTotalReads.fetch_add(1, std::memory_order::relaxed);
	if (temp != 1) {
		return AEFR_ERR_READ_EOF;
	}
	return AEFR_ERR_READ_SUCCESS;
}

#undef fscanf

#endif // !ENGINE_FILE_READER_HPP
