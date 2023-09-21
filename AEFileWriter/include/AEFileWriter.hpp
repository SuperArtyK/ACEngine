
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
#include "../ACEngine/include/AETypedefs.hpp"

// stuff for crosscompilation and 64bit access
// dont touch
#ifdef _WIN32
#define ftell _ftelli64
#define fseek _fseeki64
#else
#define ftell ftello
#define fseek fseeko
#endif


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
///indicator that everything is good
#define AEFW_ERR_NOERROR 0

//error flags
///If the file object we're trying to write to is null,
///aka file not open.
///Usually will come after AEFW_ERR_FILE_NAME_EMPTY or AEFW_ERR_FILE_OPEN_ELSE,
///if we continue to access the writer.
#define AEFW_ERR_WRITE_FILE_NULL -1
//file creation/manipulation
///If the file name is empty
#define AEFW_ERR_FILE_NAME_EMPTY -2
///If the flag given is wrong during the creation(creates file anyway, assuming AEFW_FLAG_NOFLAGS)
#define AEFW_ERR_FILE_WRONG_FLAG -3
///If the file couldn't be created for some other reason, like missing permissions to access files
#define AEFW_ERR_FILE_OPEN_ELSE -4


/// A macro to check if the given type T is about the same as Y
/// @note It decays both types and omits const-ness
#define IS_SAME_NOC(T,Y) (std::is_same<typename std::decay<const T>::type, typename std::decay<const Y>::type>::value)


//TODO: cleanup the code for performance/readability
//TODO: add docs
//TODO: add std::vector<unsigned char> version of the writeBytes

///File writer. Err, Writes data to given file
///Hungarian notation is fw
///Flags start with AEFW_
class AEFileWriter {
public:

//construction
	/// <summary>
	/// Class constructor
	/// </summary>
	/// <param name="filename">Name of the file, with extension</param>
	/// <param name="flags">Flags for file opening; look up AEFW_FLAG_* for more info</param>
	/// <param name="af_interval">interval in file writes between automatic file flushing </param>
	AEFileWriter(const std::string& filename = "", const ucint flags = AEFW_FLAG_NOFLAGS, const ullint af_interval = 1) :
		m_ullFlushInterval(af_interval), m_ullTotalWrites(0), m_fpFilestr(nullptr),
		m_ucLastError(AEFW_ERR_NOERROR) {

		this->open(filename.c_str(), flags);
	}

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
	/// <param name="filename">Name of the file, with extension</param>
	/// <param name="flags">Flags for file opening, AEFW_FLAG_* macros. More info in the docs</param></param>
	/// <returns>True if file is open(by currently used writer), false otherwise</returns>
	inline bool open(const std::string& str, const ucint flags = AEFW_FLAG_APPEND) {
		return this->open(str.c_str(), flags);
	}

	/// <summary>
	/// Open(and create) file with given name and flag
	/// </summary>
	/// <param name="filename">Name of the file, with extension</param>
	/// <param name="flags">Flags for file opening, AEFW_FLAG_* macros. More info in the docs</param></param>
	/// <returns>True if file is open(by currently used writer), false otherwise</returns>
	bool open(const char* str, const ucint flags = AEFW_FLAG_APPEND); //defined below class

//write stuff
	/// <summary>
	/// Generic function for writing data. Invokes proper write functions for built-in types
	/// @note String types don't include null-termination characters. Use separate functions for that control.
	/// @note Supported types: char, bool, integers, floats, strings; everything else is treated as binary stream.
	/// </summary>
	/// <typeparam name="T">Type of the variable to be written</typeparam>
	/// <param name="var">Variable/data piece to be written</param>
	/// <param name="datasz">Size of the data, in bytes. Only used if the T is a pointer to a binary stream, then it must be non-zero</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in m_ullFlushInterval</param>
	template<typename T>
	inline void write(const T& var, const size_t datasz = 0, const bool useAutoFlush = true); // defined below class


//write string
	/// <summary>
	/// Write std::string, as is, to file, and flush if necessary
	/// </summary>
	/// <param name="str">String to write</param>
	/// <param name="includeNull">Flag to include 1 null-terminating character in the string</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in m_ullFlushInterval</param>
	inline void writeString(const std::string& str, const bool includeNull = false, const bool useAutoFlush = true) {
		this->writeData_ptr(str.c_str(), str.size() + ((includeNull) ? 1 : 0), sizeof(char), useAutoFlush);
	}

	/// <summary>
	/// Write std::vector<char> (string), as is, to file, and flush if necessary
	/// </summary>
	/// <param name="str">String(in form of vector<char>) to write</param>
	/// <param name="includeNull">Flag to include the null-terminating character in the end</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in m_ullFlushInterval</param>
	inline void writeString(const std::vector<char>& str, const bool includeNull = false, const bool useAutoFlush = true) {
		// since vector may actually have NULL-terminating char
		// remove it if includeNull is false
		// decrease str.size() by 1
		this->writeData_ptr(str.data(), ((includeNull) ? str.size() : strlen(str.data())), sizeof(char), useAutoFlush);
	}

	/// <summary>
	/// Write c-string, as is, to file, and flush if necessary
	/// </summary>
	/// <param name="cdata">С-String to write</param>
	/// <param name="dcount">length of string</param>
	/// <param name="includeNull">Flag to include the null-terminating character at the end of the string</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in m_ullFlushInterval</param>
	inline void writeString(const char* cdata, const bool includeNull = false, const bool useAutoFlush = true) {
		this->writeData_ptr(cdata, strlen(cdata) + ((includeNull) ? 1 : 0), sizeof(char), useAutoFlush);
	}


//write ints
	/// <summary>
	/// Writes the (signed) integer as text to open file
	/// </summary>
	/// <param name="num">The signed integer to be written</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in m_ullFlushInterval</param>
	inline void writeInt(const llint num, const bool useAutoFlush = true) {
		char buf[21]{};
		snprintf(buf, sizeof(buf), "%lld", num);
		this->writeString(buf, false, useAutoFlush);
	}
	
	/// <summary>
	/// Writes the (unsigned) integer as text to open file
	/// </summary>
	/// <param name="num">The unsigned integer to be written</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in m_ullFlushInterval</param>
	inline void writeUInt(const ullint num, const bool useAutoFlush = true) {
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
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in m_ullFlushInterval</param>
	template<typename T>
	inline void writeFloat(const T num, const bool useAutoFlush = true) {
		static_assert(std::is_floating_point<T>::value, "Cannot use non-float types in AEFileWriter::writeFloat!");

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
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in m_ullFlushInterval</param>
	inline void writeBool(const bool num, const bool useAutoFlush = true) {
		if (num) {
			this->writeString("true", false, useAutoFlush);
		}
		else {
			this->writeString("false", false, useAutoFlush);
		}

	}

	/// <summary>
	/// Writes the single (ascii) character to the file.
	/// Essentially, it's the same as AEFileWriter::writeByte()
	/// </summary>
	/// <param name="c">The char to be written</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in m_ullFlushInterval</param>
	inline void writeChar(const char c, const bool useAutoFlush = true) {
		this->writeByte(c, useAutoFlush); //err...same thing.
	}

//write binary
	/// <summary>
	/// Write a stream of bytes to file, from pointer
	/// @note Basically just a shortcut for the AEFileWriter::writerData_ptr()
	/// </summary>
	/// <param name="cdata">pointer to stream of bytes</param>
	/// <param name="dsize">size of that stream</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in m_ullFlushInterval</param>
	inline void writeBytes(const void* cdata, const ullint dsize, const bool useAutoFlush = true) {
		this->writeData_ptr(cdata, 1, dsize, useAutoFlush);
	}

	/// <summary>
	/// Write a stream of bytes to file, from vector
	/// </summary>
	/// <param name="cdata">A vector to the data bytes</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in m_ullFlushInterval</param>
	inline void writeBytes(const std::vector<unsigned char>& cdata, const bool useAutoFlush = true) {
		this->writeData_ptr(cdata.data(), 1, cdata.size(), useAutoFlush);
	}

	/// <summary>
	/// Writes one byte of data to the file
	/// </summary>
	/// <param name="cdata">byte value</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in m_ullFlushInterval</param>
	inline void writeByte(const unsigned char cdata, const bool useAutoFlush = true) {
		this->writeData_ptr(&cdata, 1, 1, useAutoFlush);
	}


//write data
	/// <summary>
	/// Write binary data, as is, to file, and flush if necessary
	/// Difference with writeData_ref(): uses (const) pointer to the variable/data
	/// @note Doesn't work(fine) with literals
	/// </summary>
	/// <param name="cdata">pointer to object to write</param>
	/// <param name="dcount">number of elements in an object</param>
	/// <param name="dsize">size, in bytes, for each element</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in m_ullFlushInterval</param>
	void writeData_ptr(const void* cdata, const ullint dcount, const ullint dsize = sizeof(char), const bool useAutoFlush = true); //defined below class

	/// <summary>
	/// Write binary data, as is, to file, and flush if necessary.
	/// Difference with writeData_ptr(): uses (const) reference to the variable.
	/// @note Works with literals
	/// </summary>
	/// <param name="cdata">(pointer to) object to write</param>
	/// <param name="tsize">size of the variable, default is the sizeof()</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in m_ullFlushInterval</param>
	template<typename T>
	inline void writeData_ref(const T& cdata, const ullint tsize = sizeof(T), const bool useAutoFlush = true) {
		writeData_ptr(&cdata, 1, tsize, useAutoFlush);
	}


//file operations
	/// <summary>
	/// Flushes the opened file. That's it.
	/// </summary>
	inline void flushFile() const {
		if (this->m_fpFilestr)
			fflush(this->m_fpFilestr);
	}

	/// <summary>
	/// Closes the currently opened file. 
	/// </summary>
	inline void closeFile() {
		if (this->m_fpFilestr)
			fclose(this->m_fpFilestr);
		this->m_fpFilestr = nullptr;
	}

	/// <summary>
	/// Checks if a file is open by this file-writer
	/// </summary>
	/// <returns>True if file is open, false if otherwise</returns>
	inline bool isOpen() const {
		return bool(this->m_fpFilestr);//null if closed, something other if opened
	}

	/// <summary>
	/// Returns size of the file in bytes.
	/// </summary>
	/// <returns>File size in bytes if file is open, AEFW_ERR_WRITE_FILE_NULL if not(+error flag raised).</returns>
	inline llint getFileSize() {
		if (this->m_fpFilestr) {

			const llint curpos = ftell(this->m_fpFilestr); // get current position
			fseek(this->m_fpFilestr, 0, SEEK_END); // change pos to end of file
			const llint endpos = ftell(this->m_fpFilestr); // get pos of file at the end; file size essentially
			fseek(this->m_fpFilestr, curpos, SEEK_SET); // return to original pos

			return endpos;
		}
		else {
			this->m_ucLastError = AEFW_ERR_WRITE_FILE_NULL;
			return AEFW_ERR_WRITE_FILE_NULL;
		}
	}

	/// <summary>
	/// Returns name of currently open file
	/// </summary>
	/// <returns>File name and its additional path(if was included in opening operation)</returns>
	inline std::string getFileName() const {
		return this->m_sFilename;
	}


//file cursor stuff
	/// <summary>
	/// Returns current write cursor position
	/// </summary>
	/// <returns>Write cursor pos, starting from 0; AEFW_ERR_WRITE_FILE_NULL if not(+error)</returns>
	inline llint getCursorPos() {
		if (this->m_fpFilestr) {
			return ftell(this->m_fpFilestr);
		}
		else {
			this->m_ucLastError = AEFW_ERR_WRITE_FILE_NULL;
			return AEFW_ERR_WRITE_FILE_NULL;
		}
	}

	/// <summary>
	/// Sets read cursor position to <pos> from <origin>
	/// @note If cursor is beyond eof, it fills space between eof and cursor with null-bytes when data is written.
	/// </summary>
	/// <param name="pos">Position to be set to relative to origin ("offset" in fseek)</param>
	/// <param name="origin">Relative origin for the operation. See SEEK_SET, SEEK_CUR and SEEK_END for more details</param>
	inline void setCursorPos(const llint pos, const int origin = SEEK_CUR) {
		if (this->m_fpFilestr) {
			fseek(this->m_fpFilestr, pos, origin);
		}
		else {
			this->m_ucLastError = AEFW_ERR_WRITE_FILE_NULL;
		}

	}


//misc stuff
	/// <summary>
	/// Returns last error of writer
	/// </summary>
	/// <returns>Values of AEFW_ERR_* error codes</returns>
	inline int getLastError() const {
		return this->m_ucLastError;
	}

	/// <summary>
	/// Returns total write requests made to file
	/// </summary>
	/// <returns>Amount of times the write operation has been called on the AEFileWriter instance</returns>
	inline ullint getTotalWrites() const {
		return this->m_ullTotalWrites;
	}

	/// <summary>
	/// Refreshes error status variable and sets it to AEFW_ERR_NOERROR
	/// </summary>
	inline void refreshErr() {
		this->m_ucLastError = AEFW_ERR_NOERROR;
	}

	
	/// <summary>
	/// Interval in write operations before automatic flush operation
	/// 1 -- flush every write operation, etc.
	/// </summary>
	ullint m_ullFlushInterval;


private:

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
	/// Writer's error indicator; Values are AEFW_ERR_* macros
	ucint m_ucLastError;
};



// inline function definitions
// (so the class declaration isn't cluttered

// write data as binary function
// uses const char
void AEFileWriter::writeData_ptr(const void* cdata, const ullint dcount, const ullint dsize, const bool useAutoFlush) {
	
	if (isOpen()) {
		fwrite(cdata, dsize, dcount, m_fpFilestr);
		if (useAutoFlush) {
			if (!(++m_ullTotalWrites % m_ullFlushInterval)) {
				flushFile();
			}
		}
		this->m_ucLastError = AEFW_ERR_NOERROR;
	}
	else {
		this->m_ucLastError = AEFW_ERR_WRITE_FILE_NULL;
		return;
	}
}



// write stuff dependant on data
template<typename T>
inline void AEFileWriter::write(const T& var, const size_t datasz, const bool useAutoFlush) {

	//open file?
	if (!this->isOpen()) {
		this->m_ucLastError = AEFW_ERR_WRITE_FILE_NULL;
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
	else if constexpr (IS_SAME_NOC(T, char*) || IS_SAME_NOC(T, std::string) || IS_SAME_NOC(T, std::vector<char>)) { // strings
		this->writeString(var, false, useAutoFlush);
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
bool AEFileWriter::open(const char* str, const ucint flags) {
	if (strlen(str)) {
		this->m_sFilename = str;
		ullint found = this->m_sFilename.rfind("/");
		if (found != std::string::npos) {
			std::filesystem::create_directories(this->m_sFilename.substr(0, found));
		}

		switch (flags) {


		case AEFW_FLAG_APPEND://cursor at end, allow changing cursor position
			this->openDirectly(str, "r+b");
			if (!this->m_fpFilestr) {//couldnt open, force create
				this->openDirectly(str, "wb");
				if (!m_fpFilestr) {
					throw std::runtime_error("File could not be opened nor created!");
				}
			}
			fseek(m_fpFilestr, 0L, SEEK_END);
			break;

		case AEFW_FLAG_APPEND_NO_CURSOR_MOVE://only appending, not allowing to change cursor position
			this->openDirectly(str, "ab");
			break;


		case AEFW_FLAG_NOFLAGS://no flags, defaulting to truncation
		case AEFW_FLAG_TRUNCATE://truncate file
			this->openDirectly(str, "wb");
			break;

		default:
			this->m_ucLastError = AEFW_ERR_FILE_WRONG_FLAG;
			break;
		}
		if (!this->m_fpFilestr) {//file is still somehow nonexistent
			this->m_ucLastError = AEFW_ERR_FILE_OPEN_ELSE;
			return false;
		}
		return true;
	}
	else {
		this->m_sFilename.clear();
		this->m_ucLastError = AEFW_ERR_FILE_NAME_EMPTY;
		return false;
	}

}



#endif //!AEFILEWRITER_HPP
