
/** @file AEFileWriter/include/AEFileWriter.hpp
 *  This file contains the code for the engine's file writer.
 *  Which is just a small wrapper around the c's FILE api, for speed and convenience.
 *  One note when using it though: file is blocked from access by other programs when open.
 *  But maybe that's a good feature ;)
 *
 *  Should not cause everything to break :)
 */

#pragma once

#ifndef ENGINE_FILE_WRITER_HPP
#define ENGINE_FILE_WRITER_HPP

// define this flag on compile time damn it!
//#define _FILE_OFFSET_BITS 64

#include "include/AEModuleBase.hpp"
#include "include/AEUtils.hpp"
#include "include/AETypedefs.hpp"
#include "include/AEFlags.hpp"
#include <cstring>
#include <vector>
#include <string>
#include <limits>
#include <cstdio>
#include <filesystem>

// Do NOT touch!

//File flags
/// Macro to put the write cursor at the end of the file, appending to the file.
/// @note Cursor change allowed
#define AEFW_FLAG_APPEND 1
/// Macro to put the write cursor at the end of the file, appending to the file.
/// @note Changing the cursor position/writing anywhere else (than eof) is not allowed
#define AEFW_FLAG_APPEND_NO_CURSOR_MOVE 2 
/// Macro to put the write cursor at the start of the file, truncating the contents if existed.
#define AEFW_FLAG_TRUNCATE 3
/// Macro for no write flags.
/// @note Same as AEFW_FLAG_TRUNCATE
#define AEFW_FLAG_NOFLAGS 0

//Error flags
/// Macro for the indicator that everything is good.
#define AEFW_ERR_NOERROR 0
/// Same as AEFW_ERR_NOERROR - value of a successfull write.
#define AEFW_ERR_WRITE_SUCCESS 0
/// Macro for the error if file isn't open and file operation functions of the file writer are used.
#define AEFW_ERR_FILE_NOT_OPEN -1
//file creation/manipulation
/// Macro for the error if the file name is empty.
#define AEFW_ERR_FILE_NAME_EMPTY -2
/// Macro for the error if the given file flag is wrong and nonexistent.
#define AEFW_ERR_FILE_WRONG_FLAG -3
/// Macro for the error if the file couldn't be created for some other reason, like missing permissions to access files.
#define AEFW_ERR_FILE_OPEN_ELSE -4
/// Macro for the error that has occurred in writing (if fwrite returned a non-zero value).
#define AEFW_ERR_WRITE_ERROR -5
/// Macro for the error that occurs if the data pointer, item count and item size is null/zero in AEFileWriter::writeData_ptr().
#define AEFW_ERR_WRITE_ZERO_SIZE -6
/// Macro for the error that has occurred in flushing (if fflush returned a non-zero value).
#define AEFW_ERR_FLUSH_ERROR -7

/// Macro for the shortened "check for opened file, set error flag and return error flag if closed", DO NOT TOUCH!
#define _AEFW_EXIT_ON_CLOSED_FILE if (this->isClosed()) { this->m_cLastError = AEFW_ERR_FILE_NOT_OPEN; return AEFW_ERR_FILE_NOT_OPEN; }
/// Macro for the shortened "check for opened file during writing, set error flag and return error flag if closed", DO NOT TOUCH!
#define _AEFW_EXIT_ON_WRITE_CLOSED_FILE if (this->isClosed()) { this->m_szLastWrittenAmount = 0; this->m_cLastError = AEFW_ERR_FILE_NOT_OPEN; return AEFW_ERR_FILE_NOT_OPEN; }
/// Macro for the shortened "check for the 'append no cursor move' flag to open file, set and return error flag ifo so", DO NOT TOUCH!
#define _AEFW_EXIT_ON_NO_CURSOR_MOVE if (this->m_cFlags == AEFW_FLAG_APPEND_NO_CURSOR_MOVE) { this->m_cLastError = AEFW_FLAG_APPEND_NO_CURSOR_MOVE; return AEFW_ERR_FILE_WRONG_FLAG; }



/// <summary>
/// ArtyK's Engine File Writer; umm, it writes data to given file.
/// Just create it and dump the gigabytes of data to your files.
/// Hungarian notation is fw.
/// Flags start with AEFW_
/// @warning This is not thread safe!
/// </summary>
class AEFileWriter : public __AEModuleBase<AEFileWriter> {
public:

//construction
	/// <summary>
	/// Class constructor -- creates the AEFileWriter instance and opens the file.
	/// @see AEFileWriter::openFile()
	/// </summary>
	/// <param name="filename">Name of the file, with extension</param>
	/// <param name="flags">Flags for file opening; look up AEFW_FLAG_* for more info</param>
	/// <param name="af_interval">interval in file writes between automatic file flushing </param>
	explicit AEFileWriter(const std::string_view filename, const cint flags = AEFW_FLAG_NOFLAGS, const ullint af_interval = AEFW_DEFAULT_AUTOFLUSH_INTERVAL);

	/// <summary>
	/// Class constructor -- creates the instance but doesn't do anything.
	/// </summary>
	AEFileWriter(void) noexcept : m_ullFlushInterval(AEFW_DEFAULT_AUTOFLUSH_INTERVAL), m_ullTotalWrites(0), m_szLastWrittenAmount(0), m_fpFilestr(nullptr), m_cLastError(AEFW_ERR_NOERROR), m_cFlags(AEFW_FLAG_NOFLAGS) {}

	/// <summary>
	/// Move constructor.
	/// </summary>
	/// <param name="fw">Object to be moved</param>
	AEFileWriter(AEFileWriter&& fw) noexcept;

//we don't need those
	/// <summary>
	/// Deleted copy constructor.
	/// </summary>
	AEFileWriter(const AEFileWriter&) = delete;

	/// <summary>
	/// Deleted copy assignment operator.
	/// </summary>
	AEFileWriter& operator=(const AEFileWriter&) = delete;

	/// <summary>
	/// Default Destructor. Just flushes and closes the file.
	/// </summary>
	~AEFileWriter(void) noexcept {	
		this->flushFile();
		this->closeFile();
	}


//file operations
	/// <summary>
	/// Open(and create) file with given name and flag. 
	/// On success sets AEFileWriter::m_cFlags to passed flag value.
	/// @note If the provided file flag is invalid, it returns false and sets the AEFW_ERR_FILE_WRONG_FLAG as the last error
	/// @note If provided autoflush interval is a default value (AEFW_DEFAULT_AUTOFLUSH_INTERVAL), then the interval is not changed from the previous value
	/// @warning Using the AEFW_FLAG_APPEND_NO_CURSOR_MOVE disables AEFileWriter::getCursorPos(), AEFileWriter::setCursorPos(), and AEFileWriter::getFileSize(), since they manipulate the cursor.
	/// </summary>
	/// <param name="str">Name of the file, with extension</param>
	/// <param name="flags">Flags for file opening, AEFW_FLAG_* macros. More info in the docs</param>
	/// <param name="af_interval">Interval in file writes between automatic file flushing.</param>
	/// <returns>AEFW_ERR_NOERROR if the file was opened without errors, other error flag value otherwise</returns>
	int openFile(const std::string_view str, const cint flags = AEFW_FLAG_NOFLAGS, const ullint af_interval = AEFW_DEFAULT_AUTOFLUSH_INTERVAL); //defined below class 
	
	/// <summary>
	/// Flushes the opened file. That's it. If file isn't open, sets the last error status to AEFW_ERR_FILE_NOT_OPEN.
	/// </summary>
	/// <returns>AEFW_ERR_WRITE_SUCCESS (0) on success, otherwise AEFW_ERR_FLUSH_ERROR on error, or AEFW_ERR_FILE_NOT_OPEN if not open + last error value set</returns>
	inline cint flushFile(void) noexcept {
		_AEFW_EXIT_ON_CLOSED_FILE;

		if (fflush(this->m_fpFilestr)) {
			return AEFW_ERR_FLUSH_ERROR;
		}
		return AEFW_ERR_WRITE_SUCCESS;

	}

	/// <summary>
	/// Closes the currently opened file, and also, in addition, clears the last error status.
	/// </summary>
	inline void closeFile(void) noexcept {
		if (this->isClosed()) {
			this->m_cLastError = AEFW_ERR_FILE_NOT_OPEN;
			return;
		}

		this->m_szLastWrittenAmount = 0;
		fclose(this->m_fpFilestr);
		this->m_fpFilestr = nullptr;
		this->m_sFilename.clear();
		this->clearError();
	}


//write stuff
	/// <summary>
	/// Generic function for writing data, invoking proper write functions for built-in types.
	/// @note String types don't include null-termination characters. Use separate functions for to control that
	/// @note Supported types: char, bool, integers, floats, strings; everything else is treated as binary stream.
	/// </summary>
	/// <typeparam name="T">Type of the variable to be written</typeparam>
	/// <param name="var">Variable/data piece to be written</param>
	/// <param name="datasz">Size of the data, in bytes. Only used if the T is a pointer to a binary stream, then it must be non-zero</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	/// <returns>AEFW_ERR_WRITE_SUCCESS (0) on successfull write, otherwise other error flag/non-zero value + last error value set</returns>
	/// @todo Add a way to add custom types to this generic write function.
	template<typename T>
	inline cint write(const T& var, const size_t datasz = 0, const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH); // defined below class


//write string
	/// <summary>
	/// Write std::string, as is, to file, and flush if necessary.
	/// </summary>
	/// <param name="str">String to write</param>
	/// <param name="includeNull">Flag to include the trailing null-terminating character in the string</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	/// <returns>AEFW_ERR_WRITE_SUCCESS (0) on successfull write, otherwise other error flag/non-zero value + last error value set</returns>
	inline cint writeString(const std::string& str, const bool includeNull = false, const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH) noexcept {
		_AEFW_EXIT_ON_WRITE_CLOSED_FILE;

		if (!str.empty()) {
			return this->writeData_ptr(str.c_str(), str.size() + includeNull, sizeof(char), useAutoFlush);
		}
		else {
			return AEFW_ERR_WRITE_ZERO_SIZE;
		}
	}

	/// <summary>
	/// Write std::vector<char> (string), as is, to file, and flush if necessary.
	/// @note It just writes the whole vector to file (since I cannot determine it without good ol' slow strlen). If you want to control the null-byte, pass the std::vector<char>.data() as the
	/// </summary>
	/// <param name="str">String(in form of vector<char>) to write</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	/// <returns>AEFW_ERR_WRITE_SUCCESS (0) on successfull write, otherwise other error flag/non-zero value + last error value set</returns>
	inline cint writeString(const std::vector<char>& str, const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH) noexcept {
		_AEFW_EXIT_ON_WRITE_CLOSED_FILE;
		
		if (!str.empty()) {
			return this->writeData_ptr(str.data(), str.size(), sizeof(char), useAutoFlush);
		}
		else {
			return AEFW_ERR_WRITE_ZERO_SIZE;
		}
	}

	/// <summary>
	/// Write c-string, as is, to file, and flush if necessary.
	/// </summary>
	/// <param name="cdata">С-String to write</param>
	/// <param name="includeNull">Flag to include the null-terminating character at the end of the string</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	/// <returns>AEFW_ERR_WRITE_SUCCESS (0) on successfull write, otherwise other error flag/non-zero value + last error value set</returns>
	inline cint writeString(const char* const cdata, const bool includeNull = false, const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH) noexcept {
		_AEFW_EXIT_ON_WRITE_CLOSED_FILE; 
		
		if (!cdata) {
			return AEFW_ERR_WRITE_ZERO_SIZE;
		}

		const std::size_t len = std::strlen(cdata); // if it's null, then it's 0 and fails the write
		if (len) {
			return this->writeData_ptr(cdata, len + includeNull, sizeof(char), useAutoFlush);
		}
		else {
			return AEFW_ERR_WRITE_ZERO_SIZE;
		}
	}


//write ints
	/// <summary>
	/// Writes the (signed) integer as text to open file.
	/// </summary>
	/// <param name="num">The signed integer to be written</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	/// <returns>AEFW_ERR_WRITE_SUCCESS (0) on successfull write, otherwise other error flag/non-zero value + last error value set</returns>
	inline cint writeInt(const llint num, const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH) noexcept {
		_AEFW_EXIT_ON_WRITE_CLOSED_FILE;

		char buf[21]{};
		snprintf(buf, sizeof(buf), "%lld", num);
		return this->writeString(buf, false, useAutoFlush);
	}
	
	/// <summary>
	/// Writes the (unsigned) integer as text to open file.
	/// </summary>
	/// <param name="num">The unsigned integer to be written</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	/// <returns>AEFW_ERR_WRITE_SUCCESS (0) on successfull write, otherwise other error flag/non-zero value + last error value set</returns>
	inline cint writeUInt(const ullint num, const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH) noexcept {
		_AEFW_EXIT_ON_WRITE_CLOSED_FILE;

		char buf[21]{};
		snprintf(buf, sizeof(buf), "%llu", num);
		return this->writeString(buf, false, useAutoFlush);
	}


//write floats
	/// <summary>
	/// Writes the float value as text to the opened file.
	/// </summary>
	/// <typeparam name="T">The floating point type of the variable</typeparam>
	/// <param name="num">The float number itself</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	/// <returns>AEFW_ERR_WRITE_SUCCESS (0) on successfull write, otherwise other error flag/non-zero value + last error value set</returns>
	/// @todo Add custom way to format the float number
	template<typename T>
	inline cint writeFloat(const T num, const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH); //defined below class


//write misc
	/// <summary>
	/// Writes the boolean to file as text (true/false).
	/// </summary>
	/// <param name="num">The bool to be written</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	/// <returns>AEFW_ERR_WRITE_SUCCESS (0) on successfull write, otherwise other error flag/non-zero value + last error value set</returns>
	inline cint writeBool(const bool num, const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH) noexcept {
		return this->writeString(ace::utils::boolToString(num), false, useAutoFlush);
	}

	/// <summary>
	/// Writes the single (ascii) character to the file.
	/// @note Essentially, it's the same as AEFileWriter::writeByte()
	/// </summary>
	/// <param name="c">The char to be written</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	/// <returns>AEFW_ERR_WRITE_SUCCESS (0) on successfull write, otherwise other error flag/non-zero value + last error value set</returns>
	inline cint writeChar(const char c, const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH) noexcept {
		return this->writeByte((unsigned char)c, useAutoFlush); //err...same thing.
	}


//write binary
	/// <summary>
	/// Writes one byte of data to the file.
	/// </summary>
	/// <param name="cdata">byte value</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	/// <returns>AEFW_ERR_WRITE_SUCCESS (0) on successfull write, otherwise other error flag/non-zero value + last error value set</returns>
	inline cint writeByte(const unsigned char cdata, const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH) noexcept {
		return this->writeData_ptr(&cdata, 1, sizeof(unsigned char), useAutoFlush);
	}

	/// <summary>
	/// Write a stream of bytes to file, from pointer.
	/// @note Basically just a shortcut for the AEFileWriter::writerData_ptr()
	/// </summary>
	/// <param name="cdata">Pointer to stream of bytes</param>
	/// <param name="dsize">Size of that stream</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	/// <returns>AEFW_ERR_WRITE_SUCCESS (0) on successfull write, otherwise other error flag/non-zero value + last error value set</returns>
	inline cint writeBytes(const void* const cdata, const std::size_t dsize, const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH) noexcept {
		return this->writeData_ptr(cdata, 1, dsize, useAutoFlush);
	}

	/// <summary>
	/// Write a stream of bytes to file, from std::vector<unsigned char>.
	/// </summary>
	/// <param name="cdata">A std::vector to the data bytes</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	/// <returns>AEFW_ERR_WRITE_SUCCESS (0) on successfull write, otherwise other error flag/non-zero value + last error value set</returns>
	inline cint writeBytes(const std::vector<unsigned char>& cdata, const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH) noexcept {
		return this->writeData_ptr(cdata.data(), sizeof(unsigned char), cdata.size(), useAutoFlush);
	}


//write data
	/// <summary>
	/// Write binary data, as is, to file, and flush if necessary.
	/// Difference with writeData_ref(): uses (const) pointer to the variable/data.
	/// @note Doesn't work with literals
	/// @note If file is closed, sets last error flat to AEFW_ERR_FILE_NOT_OPEN
	/// </summary>
	/// <param name="cdata">Pointer to object to write</param>
	/// <param name="dcount">Number of elements in an object</param>
	/// <param name="dsize">Size, in bytes, for each element</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	/// <returns>AEFW_ERR_WRITE_SUCCESS (0) on successfull write, otherwise other error flag/non-zero value + last error value set</returns>
	cint writeData_ptr(const void* const cdata, const std::size_t dcount, const std::size_t dsize = sizeof(char), const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH) noexcept; //defined below class

	/// <summary>
	/// Write binary data, as is, to file, and flush if necessary.
	/// Difference with writeData_ptr(): uses (const) reference to the variable.
	/// @note Works with literals
	/// </summary>
	/// <typeparam name="T">The type of the data to be written</typeparam>
	/// <param name="cdata">The variable/data itself</param>
	/// <param name="tsize">The size of the variable/data. Defaults to the output of sizeof(T)</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	/// <returns>AEFW_ERR_WRITE_SUCCESS (0) on successfull write, otherwise other error flag/non-zero value + last error value set</returns>
	template<typename T>
	inline cint writeData_ref(const T& cdata, const std::size_t tsize = sizeof(T), const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH) {
		return this->writeData_ptr(&cdata, sizeof(unsigned char), tsize, useAutoFlush);
	}


//file information getters
	/// <summary>
	/// Checks if a file is open by this file-writer.
	/// </summary>
	/// <returns>True if file is open, false if otherwise</returns>
	inline bool isOpen(void) const noexcept {
		return bool(this->m_fpFilestr);//null if closed, something other if opened
	}

	inline bool isClosed(void) const noexcept {
		return !this->isOpen();
	}

	/// <summary>
	/// Returns size of the file in bytes.
	/// @warning Fails and returns AEFW_ERR_FILE_WRONG_FLAG, if the flag that was used to open the current file is AEFW_FLAG_APPEND_NO_CURSOR_MOVE
	/// </summary>
	/// <returns>File size in bytes if file is open, if not -- AEFW_ERR_FILE_NOT_OPEN (+last error status set to the same thing).</returns>
	inline llint getFileSize(void) noexcept {
		_AEFW_EXIT_ON_WRITE_CLOSED_FILE;
		_AEFW_EXIT_ON_NO_CURSOR_MOVE;

		const llint curpos = ftell(this->m_fpFilestr); // get current position
		fseek(this->m_fpFilestr, 0, SEEK_END); // change pos to end of file
		const llint endpos = ftell(this->m_fpFilestr); // get pos of file at the end; file size essentially
		fseek(this->m_fpFilestr, curpos, SEEK_SET); // return to original pos
		return endpos;
	}

	/// <summary>
	/// Returns the full name of currently open file, including the relative path, if such was provided.
	/// </summary>
	/// <returns>std::string of the full file name, if the file was tried to be opened</returns>
	inline std::string getFullFileName(void) const noexcept {
		return this->m_sFilename;
	}

	/// <summary>
	/// Returns the relative path of the opened file.
	/// If the file was opened in the same directory as the executable, returns "./"
	/// </summary>
	/// <returns>std::string of the relative file path</returns>
	inline std::string getRelativePath(void) const noexcept {
		const std::size_t found = this->m_sFilename.rfind('/');
		if (found != std::string::npos) {
			return this->m_sFilename.substr(0, found);
		}
		else {
			return "./";
		}
	}

	/// <summary>
	/// Returns the name of the opened file, without any paths
	/// </summary>
	/// <returns>std::string of the opened file name</returns>
	inline std::string getFileName(void) const noexcept {
		const std::size_t found = this->m_sFilename.rfind('/');
		if (found != std::string::npos) {
			return this->m_sFilename.substr(found, this->m_sFilename.size()-found);
		}
		else {
			return this->m_sFilename;
		}
	}

	/// <summary>
	/// Returns the full, absolute path of the opened file
	/// </summary>
	/// <returns>std::string of the absolute file path</returns>
	inline std::string getFullPath(void) const {
		return std::filesystem::absolute(this->m_sFilename).generic_string();
	}

	/// <summary>
	/// Returns the file pointer of this file-writer.
	/// </summary>
	/// <returns>Pointer to FILE used in the file writer</returns>
	inline FILE* getFilePtr(void) const noexcept {
		return this->m_fpFilestr;
	}


//file cursor stuff
	/// <summary>
	/// Returns current write cursor position.
	/// @warning Fails and returns AEFW_ERR_FILE_WRONG_FLAG, if the flag that was used to open the current file is AEFW_FLAG_APPEND_NO_CURSOR_MOVE
	/// </summary>
	/// <returns>If file is open, Current writ cursor position, starting from 0. If not -- AEFW_ERR_FILE_NOT_OPEN (+last error status set to the same thing).</returns>
	inline llint getCursorPos(void) noexcept {
		_AEFW_EXIT_ON_CLOSED_FILE;
		_AEFW_EXIT_ON_NO_CURSOR_MOVE;
		return ftell(this->m_fpFilestr);
	}

	/// <summary>
	/// Sets read cursor position to pos from origin.
	/// @note If cursor is beyond eof, it fills space between eof and cursor with null-bytes when data is written.
	/// @warning Fails and returns AEFW_ERR_FILE_WRONG_FLAG, if the flag that was used to open the current file is AEFW_FLAG_APPEND_NO_CURSOR_MOVE
	/// @note If origin is not SEEK_SET, SEEK_CUR or SEEK_END returns AEFR_ERR_READING_EOF
	/// </summary>
	/// <param name="pos">Position to be set to relative to origin (same as "offset" in fseek)</param>
	/// <param name="origin">Relative origin for the operation. Google SEEK_SET, SEEK_CUR and SEEK_END for more details</param>
	/// <returns>0 on success, AEFW_ERR_FILE_NOT_OPEN if file's closed, or other things that fseek can return</returns>
	inline int setCursorPos(const llint pos, const int origin = SEEK_CUR) noexcept {
		_AEFW_EXIT_ON_CLOSED_FILE;
		_AEFW_EXIT_ON_NO_CURSOR_MOVE;

		if (origin != SEEK_SET || origin != SEEK_CUR || origin != SEEK_END) {
			this->m_cLastError = AEFW_ERR_WRITE_ERROR;
			return AEFW_ERR_WRITE_ERROR;
		}

		
		return fseek(this->m_fpFilestr, pos, origin);
	}


//misc stuff
	/// <summary>
	/// Returns the last error of the writer.
	/// </summary>
	/// <returns>Values of AEFW_ERR_* error codes</returns>
	inline cint getLastError(void) const noexcept {
		return this->m_cLastError;
	}

	/// <summary>
	/// Returns total write requests made to file.
	/// </summary>
	/// <returns>Amount of times the write operation has been called on the AEFileWriter instance</returns>
	inline ullint getTotalWrites(void) const noexcept {
		return this->m_ullTotalWrites;
	}

	/// <summary>
	/// Gets the last read amount of bytes from the opened file.
	/// @note If last operation failed and no bytes were read (closed file, full EOF) -- returns 0;
	/// </summary>
	/// <returns>std::size_t of the amount of bytes read in the last reading operation</returns>
	inline std::size_t getLastWrittenAmount(void) const noexcept {
		return this->m_szLastWrittenAmount;
	}

	/// <summary>
	/// Clears last error status variable and sets it to AEFW_ERR_NOERROR.
	/// </summary>
	inline void clearError(void) noexcept {
		this->m_cLastError = AEFW_ERR_NOERROR;
	}

	/// <summary>
	/// Interval in write operations before automatic flush operation.
	/// 1 -- flush every write operation, etc; -1 -- almost never
	/// </summary>
	ullint m_ullFlushInterval;


private:
	/// <summary>
	/// Checks for and performs the auto-flush operation if required.
	/// @note Does nothing if AEFW_AUTOFLUSH_ENABLE is not defined
	/// </summary>
	inline void autoFlush(void) noexcept {
#ifdef AEFW_AUTOFLUSH_ENABLE
		if (!(this->m_ullTotalWrites % this->m_ullFlushInterval)) {
			this->flushFile();
		}
#endif // AEFW_AUTOFLUSH_ENABLE

	//otherwise we don't do anything
	}


//variables
	/// Full filename and relative path
	std::string m_sFilename;
	/// Counter for total write operations for file
	ullint m_ullTotalWrites;
	/// The amount of written bytes during last operation
	std::size_t m_szLastWrittenAmount;
	/// Object for file writing
	FILE* m_fpFilestr;
	/// Writer's last error indicator; Values are AEFW_ERR_* macros
	cint m_cLastError;
	/// Flags that were used to open the file
	cint m_cFlags;
};

REGISTER_CLASS(AEFileWriter);


// inline function definitions
// (so the class declaration isn't cluttered

template<typename T>
inline cint AEFileWriter::writeFloat(const T num, const bool useAutoFlush) {
	static_assert(std::is_floating_point<T>::value, "Cannot use non-float types in AEFileWriter::writeFloat!");

	//check for the opened file here, before potentially setting 300+ digits for nothing
	_AEFW_EXIT_ON_WRITE_CLOSED_FILE;

	if constexpr (IS_SAME_NOC(T, float)) { // float
		char buf[FLT_MAX_10_EXP + FLT_DIG + 4]{}; // juuust 1 more than max of float value, so with -FLT_MAX it will have a trailing null
		snprintf(buf, sizeof(buf), "%.*f", FLT_DIG, num);
		return this->writeString(buf, false, useAutoFlush);
	}
	else if constexpr (IS_SAME_NOC(T, double)) { // double
		char buf[DBL_MAX_10_EXP + DBL_DIG + 4]{};
		snprintf(buf, sizeof(buf), "%.*lf", DBL_DIG, num);
		return this->writeString(buf, false, useAutoFlush);
	}
	else { // long double
		char buf[LDBL_MAX_10_EXP + LDBL_DIG + 4]{};
		snprintf(buf, sizeof(buf), "%.*Lf", LDBL_DIG, num);
		return this->writeString(buf, false, useAutoFlush);
	}
}

// write stuff dependant on data
template<typename T>
inline cint AEFileWriter::write(const T& var, const size_t datasz, const bool useAutoFlush) {

	//open file?
	if (this->isClosed()) {
		this->m_cLastError = AEFW_ERR_FILE_NOT_OPEN;
		return AEFW_ERR_FILE_NOT_OPEN;
	}

	// check for bool and char first
	// because they trip the std::is_integral<T>
	// and we dont want that  

	if constexpr (IS_SAME_NOC(T, bool)) { //bool type
		return this->writeBool(var);
	}
	// check also for unsigned char - since they call the same function anyway
	else if constexpr (IS_SAME_NOC(T, char) || IS_SAME_NOC(T, unsigned char)) { //char type
		return this->writeByte(var, useAutoFlush);
	}

	// now everything else
	else if constexpr (std::is_integral<T>::value) { //integral type
		if constexpr (std::is_unsigned<T>::value) { // what about unsigned?
			return this->writeUInt(var, useAutoFlush);
		}
		else { // or signed?
			return this->writeInt(var, useAutoFlush);
		}
	}
	else if constexpr (std::is_floating_point<T>::value) { // float types
		return this->writeFloat(var, useAutoFlush);
	}
	else if constexpr (IS_SAME_NOC(T, char*) || IS_SAME_NOC(T, std::string)) { // strings
		return this->writeString(var, false, useAutoFlush);
	}
	else if constexpr (IS_SAME_NOC(T, std::vector<char>)) { // strings in form of a std::vector<char>
		return this->writeString(var, useAutoFlush);
	}

	// alright. If it's not fundamental types
	// then maybe it's a binary data to be written 

	//pointer to data
	else if constexpr (std::is_pointer<T>::value && datasz > 0) { //and check if data stream size is not 0
		return this->writeBytes(var, datasz, useAutoFlush);
	}
	else if constexpr (IS_SAME_NOC(T, std::vector<unsigned char>)) {
		return this->writeBytes(var, useAutoFlush);
	}
	//or...direct object -> pass it as reference
	else {
		return this->writeData_ref(var, sizeof(T), useAutoFlush);
	}

}


#endif // !ENGINE_FILE_WRITER_HPP
