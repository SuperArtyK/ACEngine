
#pragma once

#ifndef AEFILEWRITER_HPP
#define AEFILEWRITER_HPP

// define this flag on compile time damn it!S
//#define _FILE_OFFSET_BITS 64


#include <cstring>
#include <vector>
#include <filesystem>
#include <string>
#include <limits>
#include <cstdio>
#include <stdexcept>
#include "include/AETypedefs.hpp"
#include "include/AEFlags.hpp"
#include "include/AEModuleBase.hpp"
#include "include/AEUtils.hpp"

// stuff for crosscompilation and 64bit access
// dont touch
#ifdef _WIN32
#define ftell _ftelli64
#define fseek _fseeki64
#else
#define ftell ftello
#define fseek fseeko
#endif


//Do NOT touch!

//File flags
///Write cursor at the end of the file, adding to the file
///Cursor change allowed
#define AEFW_FLAG_APPEND 1
///Write cursor at the end of the file, adding to the file
///Changing the cursor position/writing anywhere else (than eof) is not allowed
#define AEFW_FLAG_APPEND_NO_CURSOR_MOVE 2 
///Write cursor at the start of the file, truncating the contents if existed
#define AEFW_FLAG_TRUNCATE 3
///No Write flags; Same as truncate (fopen)
#define AEFW_FLAG_NOFLAGS 255


//Error flags
///Indicator that everything is good
#define AEFW_ERR_NOERROR 0
///If the file object we're trying to write to is null,
///aka file not open.
///Usually will come after AEFW_ERR_FILE_NAME_EMPTY or AEFW_ERR_FILE_OPEN_ELSE,
///if we continue to access the writer.
#define AEFW_ERR_NOT_OPEN -1
//file creation/manipulation
///If the file name is empty
#define AEFW_ERR_FILE_NAME_EMPTY -2
///If the flag given is wrong during the creation(creates file anyway, assuming AEFW_FLAG_NOFLAGS)
#define AEFW_ERR_FILE_WRONG_FLAG -3
///If the file couldn't be created for some other reason, like missing permissions to access files
#define AEFW_ERR_FILE_OPEN_ELSE -4


/// <summary>
/// ArtyK's Engine File Writer. Umm, it writes data to given file.
/// Just create it and dump the gigabytes of data to your files.
/// Hungarian notation is fw
/// Flags start with AEFW_
/// </summary>
class AEFileWriter : public __AEModuleBase<AEFileWriter> {
public:

//construction
	/// <summary>
	/// Class constructor
	/// </summary>
	/// <param name="filename">Name of the file, with extension</param>
	/// <param name="flags">Flags for file opening; look up AEFW_FLAG_* for more info</param>
	/// <param name="af_interval">interval in file writes between automatic file flushing </param>
	AEFileWriter(const std::string& filename, const ucint flags = AEFW_FLAG_NOFLAGS, const ullint af_interval = AEFW_DEFAULT_AUTOFLUSH_INTERVAL) :
		m_ullFlushInterval(af_interval), m_ullTotalWrites(0), m_fpFilestr(nullptr),
		m_cLastError(AEFW_ERR_NOERROR) {

		this->open(filename.c_str(), flags);
	}

	/// <summary>
	/// Class constructor
	/// </summary>
	/// <param name="filename">Name of the file, with extension</param>
	/// <param name="flags">Flags for file opening; look up AEFW_FLAG_* for more info</param>
	/// <param name="af_interval">Interval in file writes between automatic file flushing </param>
	AEFileWriter(const char* filename = "", const ucint flags = AEFW_FLAG_NOFLAGS, const ullint af_interval = AEFW_DEFAULT_AUTOFLUSH_INTERVAL) :
		m_ullFlushInterval(af_interval), m_ullTotalWrites(0), m_fpFilestr(nullptr),
		m_cLastError(AEFW_ERR_NOERROR) {

		this->open(filename, flags);
	}

	/// <summary>
	/// Move constructor
	/// </summary>
	/// <param name="fw">Object to be moved</param>
	AEFileWriter(AEFileWriter&& fw) noexcept :
		m_ullFlushInterval(fw.m_ullFlushInterval), m_sFilename(fw.m_sFilename),
		m_ullTotalWrites(fw.m_ullTotalWrites), m_fpFilestr(fw.m_fpFilestr), m_cLastError(fw.m_cLastError), m_ucFlags(fw.m_ucFlags) {

		fw.m_ullFlushInterval = 0;
		fw.m_ullTotalWrites = 0;
		fw.m_fpFilestr = nullptr;
		fw.m_cLastError = 0;
		fw.m_sFilename.clear();
		fw.m_ucFlags = 0;
	}

	//we don't need those
	AEFileWriter(const AEFileWriter&) = delete;
	AEFileWriter& operator=(const AEFileWriter&) = delete;

	/// <summary>
	/// Default Destructor. Just flushes and closes the file.
	/// </summary>
	~AEFileWriter() {	
		this->flushFile();
		this->closeFile();
	}


//open files
	/// <summary>
	/// Open(and create) file with given name and flag
	/// </summary>
	/// <param name="str">Name of the file, with extension</param>
	/// <param name="flags">Flags for file opening, AEFW_FLAG_* macros. More info in the docs</param>
	/// <returns>True if file was able to be open for writing, false otherwise</returns>
	inline bool open(const std::string& str, const ucint flags = AEFW_FLAG_NOFLAGS, const ullint af_interval = AEFW_DEFAULT_AUTOFLUSH_INTERVAL) {
		return this->open(str.c_str(), flags, af_interval);
	}

	/// <summary>
	/// Open(and create) file with given name and flag. 
	/// On success sets AEFileWriter::m_ucFlags to passed flag value.
	/// @warn Using the AEFW_FLAG_APPEND_NO_CURSOR_MOVE disables AEFileWriter::getCursorPos(), AEFileWriter::setCursorPos(), and AEFileWriter::getFileSize(), since they manipulate the cursor.
	/// </summary>
	/// <param name="str">Name of the file, with extension</param>
	/// <param name="flags">Flags for file opening, AEFW_FLAG_* macros. More info in the docs</param>
	/// <returns>True if file was able to be open for writing, false otherwise</returns>
	inline bool open(const char* str, const ucint flags = AEFW_FLAG_NOFLAGS, const ullint af_interval = AEFW_DEFAULT_AUTOFLUSH_INTERVAL); //defined below class 
	
//write stuff
	//TODO: Add a way to add custom types to this generic write function.
	/// <summary>
	/// Generic function for writing data. Invokes proper write functions for built-in types
	/// @note String types don't include null-termination characters. Use separate functions for to control that
	/// @note Supported types: char, bool, integers, floats, strings; everything else is treated as binary stream.
	/// </summary>
	/// <typeparam name="T">Type of the variable to be written</typeparam>
	/// <param name="var">Variable/data piece to be written</param>
	/// <param name="datasz">Size of the data, in bytes. Only used if the T is a pointer to a binary stream, then it must be non-zero</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	template<typename T>
	inline void write(const T& var, const size_t datasz = 0, const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH); // defined below class


//write string
	/// <summary>
	/// Write std::string, as is, to file, and flush if necessary
	/// </summary>
	/// <param name="str">String to write</param>
	/// <param name="includeNull">Flag to include the trailing null-terminating character in the string</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	inline void writeString(const std::string& str, const bool includeNull = false, const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH) {
		if (!this->m_fpFilestr) {
			this->m_cLastError = AEFW_ERR_NOT_OPEN;
			return;
		}

		if(!str.empty())
			this->writeData_ptr(str.c_str(), str.size() + includeNull, sizeof(char), useAutoFlush);
	}

	/// <summary>
	/// Write std::vector<char> (string), as is, to file, and flush if necessary
	/// @note It just writes the whole vector to file (since I cannot determine it without good ol' slow strlen). If you want to control the null-byte, pass the std::vector<char>.data() as the
	/// </summary>
	/// <param name="str">String(in form of vector<char>) to write</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	inline void writeString(const std::vector<char>& str, const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH) {
		if (!this->m_fpFilestr) {
			this->m_cLastError = AEFW_ERR_NOT_OPEN;
			return;
		}
		this->writeData_ptr(str.data(), str.size(), sizeof(char), useAutoFlush);
	}

	/// <summary>
	/// Write c-string, as is, to file, and flush if necessary
	/// </summary>
	/// <param name="cdata">С-String to write</param>
	/// <param name="includeNull">Flag to include the null-terminating character at the end of the string</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	inline void writeString(const char* cdata, const bool includeNull = false, const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH) {
		if (!this->m_fpFilestr) {
			this->m_cLastError = AEFW_ERR_NOT_OPEN;
			return;
		}
		this->writeData_ptr(cdata, strlen(cdata) + includeNull, sizeof(char), useAutoFlush);
	}


//write ints
	/// <summary>
	/// Writes the (signed) integer as text to open file
	/// </summary>
	/// <param name="num">The signed integer to be written</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	inline void writeInt(const llint num, const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH) {
		if (!this->m_fpFilestr) {
			this->m_cLastError = AEFW_ERR_NOT_OPEN;
			return;
		}

		char buf[21]{};
		snprintf(buf, sizeof(buf), "%lld", num);
		this->writeString(buf, false, useAutoFlush);
	}
	
	/// <summary>
	/// Writes the (unsigned) integer as text to open file
	/// </summary>
	/// <param name="num">The unsigned integer to be written</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	inline void writeUInt(const ullint num, const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH) {
		if (!this->m_fpFilestr) {
			this->m_cLastError = AEFW_ERR_NOT_OPEN;
			return;
		}

		char buf[21]{};
		snprintf(buf, sizeof(buf), "%llu", num);
		this->writeString(buf, false, useAutoFlush);
	}

//write floats
	/// <summary>
	/// Writes the float value as text to the opened file
	/// </summary>
	/// <typeparam name="T">The floating point type of the variable</typeparam>
	/// <param name="num">The float number itself</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	template<typename T>
	inline void writeFloat(const T num, const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH) {
		static_assert(std::is_floating_point<T>::value, "Cannot use non-float types in AEFileWriter::writeFloat!");

		//check for the opened file here, before potentially setting 300+ digits for nothing
		if (!this->m_fpFilestr) {
			this->m_cLastError = AEFW_ERR_NOT_OPEN;
			return;
		}

		if constexpr (std::is_same<T, float>::value) { // float
			char buf[FLT_MAX_10_EXP + FLT_DIG + 4]{}; // juuust 1 more than max of float value, so with -FLT_MAX it will have a trailing null
			snprintf(buf, sizeof(buf), "%.*f", FLT_DIG, num);
			this->writeString(buf, false, useAutoFlush);
		}
		else if constexpr (std::is_same<T, double>::value) { // double
			char buf[DBL_MAX_10_EXP + DBL_DIG + 4]{};
			snprintf(buf, sizeof(buf), "%.*f", DBL_DIG, num);
			this->writeString(buf, false, useAutoFlush);
		}
		else if constexpr (std::is_same<T, long double>::value) { // long double
			char buf[LDBL_MAX_10_EXP + LDBL_DIG + 4]{};
			snprintf(buf, sizeof(buf), "%.*f", LDBL_DIG, num);
			this->writeString(buf, false, useAutoFlush);
		}

	}

	/// <summary>
	/// Writes the boolean to file as text (true/false)
	/// </summary>
	/// <param name="num">The bool to be written</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	inline void writeBool(const bool num, const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH) {
		this->writeString(ace::utils::boolToString(num), false, useAutoFlush);
	}

	/// <summary>
	/// Writes the single (ascii) character to the file.
	/// Essentially, it's the same as AEFileWriter::writeByte()
	/// </summary>
	/// <param name="c">The char to be written</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	inline void writeChar(const char c, const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH) {
		this->writeByte(c, useAutoFlush); //err...same thing.
	}

//write binary
	/// <summary>
	/// Write a stream of bytes to file, from pointer
	/// @note Basically just a shortcut for the AEFileWriter::writerData_ptr()
	/// </summary>
	/// <param name="cdata">Pointer to stream of bytes</param>
	/// <param name="dsize">Size of that stream</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	inline void writeBytes(const void* cdata, const std::size_t dsize, const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH) {
		this->writeData_ptr(cdata, 1, dsize, useAutoFlush);
	}

	/// <summary>
	/// Write a stream of bytes to file, from std::vector<unsigned char>
	/// </summary>
	/// <param name="cdata">A std::vector to the data bytes</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	inline void writeBytes(const std::vector<unsigned char>& cdata, const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH) {
		this->writeData_ptr(cdata.data(), sizeof(unsigned char), cdata.size(), useAutoFlush);
	}

	/// <summary>
	/// Writes one byte of data to the file
	/// </summary>
	/// <param name="cdata">byte value</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	inline void writeByte(const unsigned char cdata, const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH) {
		this->writeData_ptr(&cdata, sizeof(unsigned char), sizeof(unsigned char), useAutoFlush);
	}


//write data
	/// <summary>
	/// Write binary data, as is, to file, and flush if necessary
	/// Difference with writeData_ref(): uses (const) pointer to the variable/data
	/// @note Doesn't work with literals
	/// @note If file is closed, sets last error flat to AEFW_ERR_NOT_OPEN
	/// </summary>
	/// <param name="cdata">Pointer to object to write</param>
	/// <param name="dcount">Number of elements in an object</param>
	/// <param name="dsize">Size, in bytes, for each element</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	inline void writeData_ptr(const void* cdata, const std::size_t dcount, const std::size_t dsize = sizeof(char), const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH); //defined below class

	/// <summary>
	/// Write binary data, as is, to file, and flush if necessary.
	/// Difference with writeData_ptr(): uses (const) reference to the variable.
	/// @note Works with literals
	/// </summary>
	/// <typeparam name="T">The type of the data to be written</typeparam>
	/// <param name="cdata">The variable/data itself</param>
	/// <param name="tsize">The size of the variable/data. Defaults to the output of sizeof(T)</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes, specified by m_ullFlushInterval</param>
	template<typename T>
	inline void writeData_ref(const T& cdata, const std::size_t tsize = sizeof(T), const bool useAutoFlush = AEFW_DEFAULT_AUTOFLUSH) {
		writeData_ptr(&cdata, sizeof(unsigned char), tsize, useAutoFlush);
	}


//file operations
	/// <summary>
	/// Flushes the opened file. That's it. If file isn't open, sets the last error status to AEFW_ERR_NOT_OPEN.
	/// </summary>
	inline void flushFile(void) {
		if (this->m_fpFilestr) {
			fflush(this->m_fpFilestr);
		}
		else {
			this->m_cLastError = AEFW_ERR_NOT_OPEN;
		}
	}

	/// <summary>
	/// Closes the currently opened file. In addition, clears the last error status.
	/// </summary>
	inline void closeFile(void) {
		if (this->m_fpFilestr)
			fclose(this->m_fpFilestr);
		this->m_fpFilestr = nullptr;
		this->m_sFilename.clear();
		clearError();
	}

	/// <summary>
	/// Checks if a file is open by this file-writer.
	/// </summary>
	/// <returns>True if file is open, false if otherwise</returns>
	inline bool isOpen(void) const {
		return bool(this->m_fpFilestr);//null if closed, something other if opened
	}

	/// <summary>
	/// Returns size of the file in bytes.
	/// @warn Fails and returns AEFW_ERR_FILE_WRONG_FLAG if the flag used to open the current file is AEFW_FLAG_APPEND_NO_CURSOR_MOVE
	/// </summary>
	/// <returns>File size in bytes if file is open, if not -- AEFW_ERR_NOT_OPEN.</returns>
	inline llint getFileSize(void) const {

		if (!this->m_fpFilestr) {
			//this->m_cLastError = AEFW_ERR_NOT_OPEN;
			return AEFW_ERR_NOT_OPEN;
		}
		if (this->m_ucFlags == AEFW_FLAG_APPEND_NO_CURSOR_MOVE) {
			return AEFW_ERR_FILE_WRONG_FLAG;
		}

		const llint curpos = ftell(this->m_fpFilestr); // get current position
		fseek(this->m_fpFilestr, 0, SEEK_END); // change pos to end of file
		const llint endpos = ftell(this->m_fpFilestr); // get pos of file at the end; file size essentially
		fseek(this->m_fpFilestr, curpos, SEEK_SET); // return to original pos

		return endpos;
	}

	/// <summary>
	/// Returns name of currently open file
	/// </summary>
	/// <returns>File name and its additional path(if was included in opening operation)</returns>
	inline std::string getFileName(void) const {
		return this->m_sFilename;
	}


//file cursor stuff
	/// <summary>
	/// Returns current write cursor position
	/// @warn Fails and returns AEFW_ERR_FILE_WRONG_FLAG if the flag used to open the current file is AEFW_FLAG_APPEND_NO_CURSOR_MOVE
	/// </summary>
	/// <returns>Write cursor pos, starting from 0 if file is open, if not -- AEFW_ERR_NOT_OPEN (+last error status set to the same thing).</returns>
	inline llint getCursorPos(void) const {
		if (!this->m_fpFilestr) {
			//this->m_cLastError = AEFW_ERR_NOT_OPEN;
			return AEFW_ERR_NOT_OPEN;
		}
		if (this->m_ucFlags == AEFW_FLAG_APPEND_NO_CURSOR_MOVE) {
			return AEFW_ERR_FILE_WRONG_FLAG;
		}

		return ftell(this->m_fpFilestr);
	}

	/// <summary>
	/// Sets read cursor position to <pos> from <origin>
	/// @note If cursor is beyond eof, it fills space between eof and cursor with null-bytes when data is written.
	/// @warn Fails and returns AEFW_ERR_FILE_WRONG_FLAG if the flag used to open the current file is AEFW_FLAG_APPEND_NO_CURSOR_MOVE
	/// </summary>
	/// <param name="pos">Position to be set to relative to origin (same as "offset" in fseek)</param>
	/// <param name="origin">Relative origin for the operation. Google SEEK_SET, SEEK_CUR and SEEK_END for more details</param>
	/// <returns>0 on success, AEFW_ERR_NOT_OPEN if file's closed, or other things that fseek can return</returns>
	inline int setCursorPos(const llint pos, const int origin = SEEK_CUR) const {

		if (!this->m_fpFilestr) {
			return AEFW_ERR_NOT_OPEN;
		}
		if (this->m_ucFlags == AEFW_FLAG_APPEND_NO_CURSOR_MOVE) {
			return AEFW_ERR_FILE_WRONG_FLAG;
		}
		
		return fseek(this->m_fpFilestr, pos, origin);
	}


//misc stuff
	/// <summary>
	/// Returns the last error of the writer
	/// </summary>
	/// <returns>Values of AEFW_ERR_* error codes</returns>
	inline cint getLastError(void) const {
		return this->m_cLastError;
	}

	/// <summary>
	/// Returns total write requests made to file
	/// </summary>
	/// <returns>Amount of times the write operation has been called on the AEFileWriter instance</returns>
	inline ullint getTotalWrites(void) const {
		return this->m_ullTotalWrites;
	}

	/// <summary>
	/// Clears last error status variable and sets it to AEFW_ERR_NOERROR
	/// </summary>
	inline void clearError(void) {
		this->m_cLastError = AEFW_ERR_NOERROR;
	}
	
	/// <summary>
	/// Interval in write operations before automatic flush operation
	/// 1 -- flush every write operation, etc; -1 -- almost never
	/// </summary>
	ullint m_ullFlushInterval;


private:
	/// <summary>
	/// Checks for and performs the auto-flush operation if required.
	/// @note Does nothing if AEFW_AUTOFLUSH_ENABLE is not defined
	/// </summary>
	inline void autoFlush() {
#ifdef AEFW_AUTOFLUSH_ENABLE
		if (!(m_ullTotalWrites % m_ullFlushInterval)) {
			flushFile();
		}
#endif // !AEFW_AUTOFLUSH_ENABLE

	//otherwise we don't do anything
	}

//crosscompile crap
	/// <summary>
	/// Opens file with given name and flags.
	/// Needed so i dont need to retype ifdef for each of file openings
	/// Makes code cleaner...relatively
	/// </summary>
	/// <param name="fname">File name, as you'd pass to fopen</param>
	/// <param name="flags">File flags, as you'd pass to fopen</param>
	inline void openDirectly(const char* fname, const char* flags) {
		//safety, so our compiler shuts up about the unsafe and deprecated function
		//and trigger only on vc++
//if our compiler is vc++
#ifdef _MSC_VER 
		fopen_s(&this->m_fpFilestr, fname, flags);
#else
//or some other stuff
		m_fpFilestr = fopen(fname, flags);
#endif // _MSC_VER 
	}


//variables
	/// Full filename and relative path
	std::string m_sFilename;
	/// Counter for total write operations for file
	ullint m_ullTotalWrites;
	/// Object for file writing
	FILE* m_fpFilestr;
	/// Writer's last error indicator; Values are AEFW_ERR_* macros
	cint m_cLastError;
	/// Flags that were used to open the file
	ucint m_ucFlags;
};

REGISTER_CLASS(AEFileWriter);


// inline function definitions
// (so the class declaration isn't cluttered

// write data as binary function
// uses const char
inline void AEFileWriter::writeData_ptr(const void* cdata, const std::size_t dcount, const std::size_t dsize, const bool useAutoFlush) {
	
	if (!this->m_fpFilestr) {
		this->m_cLastError = AEFW_ERR_NOT_OPEN;
		return;
	}

	fwrite(cdata, dsize, dcount, m_fpFilestr);
	this->m_ullTotalWrites++;
	if (useAutoFlush) {
		this->autoFlush();
	}
	this->m_cLastError = AEFW_ERR_NOERROR;
}

// write stuff dependant on data
template<typename T>
inline void AEFileWriter::write(const T& var, const size_t datasz, const bool useAutoFlush) {

	//open file?
	if (!this->m_fpFilestr) {
		this->m_cLastError = AEFW_ERR_NOT_OPEN;
		return;
	}

	// check for bool and char first
	// because they trip the std::is_integral<T>
	// and we dont want that  

	if constexpr (IS_SAME_NOC(T, bool)) { //bool type
		this->writeBool(var);
	}
	// check also for unsigned char - since they call the same function anyway
	else if constexpr (IS_SAME_NOC(T, char) || IS_SAME_NOC(T, unsigned char)) { //char type
		this->writeByte(var, useAutoFlush);
	}

	// now everything else
	else if constexpr (std::is_integral<T>::value) { //integral type
		if constexpr (std::is_unsigned<T>::value) { // what about unsigned?
			this->writeUInt(var, useAutoFlush);
		}
		else { // or signed?
			this->writeInt(var, useAutoFlush);
		}
	}
	else if constexpr (std::is_floating_point<T>::value) { // float types
		this->writeFloat(var, useAutoFlush);
	}
	else if constexpr (IS_SAME_NOC(T, char*) || IS_SAME_NOC(T, std::string)) { // strings
		this->writeString(var, false, useAutoFlush);
	}
	else if constexpr (IS_SAME_NOC(T, std::vector<char>)) { // strings in form of a std::vector<char>
		this->writeString(var, useAutoFlush);
	}

	// alright. If it's not fundamental types
	// then maybe it's a binary data to be written 

	//pointer to data
	else if constexpr (std::is_pointer<T>::value && datasz > 0) { //and check if data stream size is not 0
		this->writeBytes(var, datasz, useAutoFlush);
	}
	else if constexpr (IS_SAME_NOC(T, std::vector<unsigned char>)) {
		this->writeBytes(var, useAutoFlush);
	}
	//or...direct object -> pass it as reference
	else {
		this->writeData_ref(var, sizeof(T), useAutoFlush);
	}

}

// open file with flags
// uses const char* as all normal string function do
bool AEFileWriter::open(const char* str, const ucint flags, const ullint af_interval) {

	if (this->m_fpFilestr) { // open already -> pls close
		this->closeFile();
	}

	if (!strlen(str)) {
		this->m_sFilename.clear();
		this->m_cLastError = AEFW_ERR_FILE_NAME_EMPTY;
		return false;
	}


	this->m_sFilename = str;
	std::size_t found = this->m_sFilename.rfind("/");
	if (found != std::string::npos) {
		std::filesystem::create_directories(this->m_sFilename.substr(0, found));
	}

	switch (flags) {

	case AEFW_FLAG_APPEND://cursor at end, allow changing cursor position
		this->openDirectly(str, "r+b");
		if (!this->m_fpFilestr) {//couldnt open, force create
			this->openDirectly(str, "wb");
			if (!this->m_fpFilestr) {
				break; // some other error
			}
		}
		fseek(this->m_fpFilestr, 0L, SEEK_END);
		break;

	case AEFW_FLAG_APPEND_NO_CURSOR_MOVE://only appending, not allowing to change cursor position
		this->openDirectly(str, "ab");
		break;


	case AEFW_FLAG_NOFLAGS://no flags, defaulting to truncation
	case AEFW_FLAG_TRUNCATE://truncate file
		this->openDirectly(str, "wb");
		break;

	default:
		this->m_cLastError = AEFW_ERR_FILE_WRONG_FLAG; //wrong flag, not opening the file
		return false;
	}

	// last check, to see if everything is okay
	if (!this->m_fpFilestr) {
		this->m_cLastError = AEFW_ERR_FILE_OPEN_ELSE; //file is still somehow nonexistent
		return false;
	}

	//then everything is good, 
	this->m_ucFlags = flags;
	this->m_ullFlushInterval = af_interval;
	return true;
}



#endif //!AEFILEWRITER_HPP
