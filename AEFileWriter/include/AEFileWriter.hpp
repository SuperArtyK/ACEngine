
#pragma once

#ifndef AEFILEWRITER_HPP
#define AEFILEWRITER_HPP

#define _FILE_OFFSET_BITS 64


#include <cstring>
#include <vector>
#include <filesystem>
#include <string>
#include <limits>
#include <cstdio>
#include <stdexcept>

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



///File writer. Err, Writes data to given file
///Hungarian notation is fw
///Flags start with AEFW_
class AEFileWriter {
public:
	/// <summary>
	/// Class constructor
	/// </summary>
	/// <param name="filename">Name of the file, with extension</param>
	/// <param name="flags">Flags for file opening; look up AEFW_FLAG_* for more info</param>
	/// <param name="af_interval">interval in file writes between automatic file flushing </param>
	AEFileWriter(const std::string& filename = "", const uint8_t flags = AEFW_FLAG_APPEND, const uint64_t af_interval = 1) :
		m_autoflushInterval(af_interval), m_ullTotalWrites(0), m_fpFilestr(nullptr),
		m_ucLastError(AEFW_ERR_NOERROR) {

		this->open(filename.c_str(), flags);
	}

	/// <summary>
	/// Default Destructor. Just flushes and closes the file.
	/// </summary>
	~AEFileWriter() {
		flushFile();
		closeFile();
	}

	/// <summary>
	/// Write std::string, as is, to file, and flush if necessary
	/// </summary>
	/// <param name="str">String to write</param>
	/// <param name="includeNull">Flag to include 1 null-terminating character in the string</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in autoflush_interval</param>
	inline void writeString(const std::string& str, const bool includeNull = false, const bool useAutoFlush = true) {
		this->writeData_ptr(str.c_str(), str.size() + ((includeNull) ? 1 : 0), sizeof(char), useAutoFlush);
	}

	/// <summary>
	/// Write std::vector of char (string), as is, to file, and flush if necessary
	/// </summary>
	/// <param name="str">String(in form of vector<char>) to write</param>
	/// <param name="includeNull">Flag to include the null-terminating character in the end</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in autoflush_interval</param>
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
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in autoflush_interval</param>
	inline void writeString(const char* cdata, const bool includeNull = false, const bool useAutoFlush = true) {
		this->writeData_ptr(cdata, strlen(cdata) + ((includeNull) ? 1 : 0), sizeof(char), useAutoFlush);
	}

	/// <summary>
	/// Write binary data, as is, to file, and flush if necessary
	/// Difference with writeData_ref(): uses (const) pointer to the variable/data
	/// @note Doesn't work(fine) with literals
	/// </summary>
	/// <param name="cdata">pointer to object to write</param>
	/// <param name="dcount">number of elements in an object</param>
	/// <param name="dsize">size, in bytes, for each element</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in autoflush_interval</param>
	void writeData_ptr(const void* cdata, const uint64_t dcount, const uint64_t dsize = sizeof(char), const bool useAutoFlush = true); //defined below class

	/// <summary>
	/// Shortcut for the AEFileWriter::writerData_ptr()
	/// </summary>
	/// <param name="cdata">pointer to stream of bytes</param>
	/// <param name="dsize">size of that stream</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in autoflush_interval</param>
	inline void writeBytes(const void* cdata, const uint64_t dsize, const bool useAutoFlush = true){
		writeData_ptr(cdata, 1, dsize, useAutoFlush);
	}

	/// <summary>
	/// Writes one byte of data to the file
	/// </summary>
	/// <param name="cdata">byte value</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in autoflush_interval</param>
	inline void writeByte(const char cdata, const bool useAutoFlush = true) {
		this->writeData_ptr(&cdata, 1, 1, useAutoFlush);
	}

	/// <summary>
	/// Write binary data, as is, to file, and flush if necessary.
	/// Difference with writeData_ptr(): uses (const) reference to the variable.
	/// @note Works with literals
	/// </summary>
	/// <param name="cdata">(pointer to) object to write</param>
	/// <param name="tsize">size of the variable, default is the sizeof()</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in autoflush_interval</param>
	template<typename T>
	inline void writeData_ref(const T& cdata, const uint64_t tsize = sizeof(T), const bool useAutoFlush = true) {
		writeData_ptr(&cdata, 1, tsize, useAutoFlush);
	}

	//TODO: Maybe rewrite to million template overloads
	//idk
	/// <summary>
	/// Determines type of the data passed and uses according procedure to write it to file
	/// @note character string types(vector char, std::string, char*) do not include null-termination character
	/// Invokes other member functions
	/// </summary>
	/// <param name="var">variable/object to write</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in autoflush_interval</param>
	template<typename T>
	inline void write(const T& var, const bool useAutoFlush = true); // defined below class

	/// <summary>
	/// Open(and create) file with given name and flag
	/// </summary>
	/// <param name="filename">Name of the file, with extension</param>
	/// <param name="flags">Flags for file opening, AEFW_FLAG_* macros. More info in the docs</param></param>
	/// <returns>True if file is open(by currently used writer), false otherwise</returns>
	inline bool open(const std::string& str, const uint8_t flags = AEFW_FLAG_APPEND) {
		return this->open(str.c_str(), flags);
	}

	/// <summary>
	/// Open(and create) file with given name and flag
	/// </summary>
	/// <param name="filename">Name of the file, with extension</param>
	/// <param name="flags">Flags for file opening, AEFW_FLAG_* macros. More info in the docs</param></param>
	/// <returns>True if file is open(by currently used writer), false otherwise</returns>
	bool open(const char* str, const uint8_t flags = AEFW_FLAG_APPEND); //defined below class

	/// <summary>
	/// Flushes the opened file. That's it.
	/// </summary>
	inline void flushFile() const {
		if (m_fpFilestr)
			fflush(m_fpFilestr);
	}

	/// <summary>
	/// Closes the currently opened file. 
	/// </summary>
	inline void closeFile() {
		if (m_fpFilestr)
			fclose(m_fpFilestr);
		m_fpFilestr = nullptr;
	}

	/// <summary>
	/// Checks if a file is open by this file-writer
	/// </summary>
	/// <returns>True if file is open, false if otherwise</returns>
	inline bool isOpen() const {
		return bool(this->m_fpFilestr);//null if closed, something other if opened
	}

	/// <summary>
	/// Returns last error of writer
	/// </summary>
	/// <returns>Values of AEFW_ERR_* error codes</returns>
	inline int getLastError() const {
		return this->m_ucLastError;
	}

	/// <summary>
	/// Returns name of currently open file
	/// </summary>
	/// <returns>File name and its additional path(if was included in opening operation)</returns>
	inline std::string getFileName() const {
		return this->m_sFilename;
	}

	/// <summary>
	/// Returns total write requests made to file
	/// </summary>
	/// <returns>Amount of times the write operation has been called on the AEFileWriter instance</returns>
	inline uint64_t getTotalWrites() const {
		return this->m_ullTotalWrites;
	}

	/// <summary>
	/// Refreshes error status variable and sets it to AEFW_ERR_NOERROR
	/// </summary>
	inline void refreshErr() {
		this->m_ucLastError = AEFW_ERR_NOERROR;
	}


	/// <summary>
	/// Returns size of the file in bytes.
	/// </summary>
	/// <returns>File size in bytes if file is open, AEFW_ERR_WRITE_FILE_NULL if not(+error flag raised).</returns>
	inline int64_t getFileSize() {
		if (m_fpFilestr) {
			
			const int64_t curpos = ftell(m_fpFilestr); // get current position
			fseek(m_fpFilestr, 0, SEEK_END); // change pos to end of file
			const int64_t endpos = ftell(m_fpFilestr); // get pos of file at the end; file size essentially
			fseek(m_fpFilestr, curpos, SEEK_SET); // return to original pos
			
			return endpos;
		}
		else {
			m_ucLastError = AEFW_ERR_WRITE_FILE_NULL;
			return AEFW_ERR_WRITE_FILE_NULL;
		}
	}

	///returns current read cursor position, starting from 0 as beginning


	/// <summary>
	/// Returns current write cursor position
	/// </summary>
	/// <returns>Write cursor pos, starting from 0; AEFW_ERR_WRITE_FILE_NULL if not(+error)</returns>
	inline int64_t getCursorPos() {
		if (m_fpFilestr) {
			return ftell(m_fpFilestr);
		}
		else {
			m_ucLastError = AEFW_ERR_WRITE_FILE_NULL;
			return AEFW_ERR_WRITE_FILE_NULL;
		}
	}
	
	/// <summary>
	/// Sets read cursor position to <pos> from <origin>
	/// @note If cursor is beyond eof, it fills space between eof and cursor with null-bytes when data is written.
	/// </summary>
	/// <param name="pos">Position to be set to relative to origin ("offset" in fseek)</param>
	/// <param name="origin">Relative origin for the operation. See SEEK_SET, SEEK_CUR and SEEK_END for more details</param>
	inline void setCursorPos(const int64_t pos, const int origin = SEEK_CUR) {
		if (m_fpFilestr) {
			fseek(m_fpFilestr, pos, origin);
		}
		else {
			m_ucLastError = AEFW_ERR_WRITE_FILE_NULL;
		}

	}
	
	/// <summary>
	/// Interval in write operations before automatic flush operation
	/// 1 -- flush every write operation, etc.
	/// </summary>
	uint64_t m_autoflushInterval;





private:
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

	inline int seekFile(FILE* stream, const int64_t offset, const int origin) {
#ifdef _MSC_VER 
		_fseeki64(stream, offset, origin);
		//fopen_s(&this->m_fpFilestr, fname, flags);
#else
#ifdef 
		//or some other stuff
		m_fpFilestr = fopen(fname, flags);
#endif // _MSC_VER 
#endif
	}


	/// Full filename
	std::string m_sFilename;
	/// Counter for total write operations for file
	uint64_t m_ullTotalWrites;
	/// Object for file writing
	FILE* m_fpFilestr;
	/// Writer's error indicator; Values are AEFW_ERR_* macros
	uint8_t m_ucLastError;
};

// inline function definitions
// (so the class declaration isn't cluttered


// write data as binary function
// uses const char
void AEFileWriter::writeData_ptr(const void* cdata, const uint64_t dcount, const uint64_t dsize, const bool useAutoFlush) {
	
	if (isOpen()) {
		fwrite(cdata, dsize, dcount, m_fpFilestr);
		if (useAutoFlush) {
			if (!(++m_ullTotalWrites % m_autoflushInterval)) {
				flushFile();
			}
		}
		m_ucLastError = AEFW_ERR_NOERROR;
	}
	else {
		m_ucLastError = AEFW_ERR_WRITE_FILE_NULL;
		return;
	}
}



// write stuff dependant on data
template<typename T>
inline void AEFileWriter::write(const T& var, const bool useAutoFlush) {

	//char array for formatting
	//classic 4*size of the variable
	char formArr[sizeof(var) * 4]{};

	//great line of if's to check the types
	//since we don't have a constexpr version of switch
	//and we don't need it anyway

	//checks for types that don't require formatting with sprintf
	//single chars
	if constexpr (std::is_same<T, char>::value || std::is_same<T, unsigned char>::value ||
		//const versions
		std::is_same<T, const char>::value || std::is_same<T, const unsigned char>::value) {

		//the type is well, char
		writeData_ptr(&var, 1, sizeof(char), useAutoFlush);
		return;
	}
	//c-strings
	else if constexpr (std::is_same<typename std::decay<T>::type, char*>::value || std::is_same<typename std::decay<T>::type, unsigned char*>::value ||
		//const versions
		std::is_same<typename std::decay<T>::type, const char*>::value || std::is_same<typename std::decay<T>::type, const unsigned char*>::value ||
		//std::vector's of char or std::string's
		std::is_same<T, std::vector<char>>::value || std::is_same<T, std::string>::value ||
		//const versions
		std::is_same<T, const std::vector<char>>::value || std::is_same<T, const std::string>::value) {
		//no null-terminating char
		//use writeString() directly instead
		writeString(var, false, useAutoFlush);
		return;
	}

	//stuff that needs formatting
	// integer types:
	//shorts and signed ints and booleans
	else if constexpr (std::is_same<T, short>::value || std::is_same<T, unsigned short>::value || std::is_same<T, int>::value || std::is_same<T, bool>::value ||
		//const versions
		std::is_same<T, const short>::value || std::is_same<T, const unsigned short>::value || std::is_same<T, const int>::value || std::is_same<T, const bool>::value) {

		sprintf(formArr, "%d", (int)var);
	}
	//unsigned ints
	else if constexpr (std::is_same<T, unsigned int>::value ||
		//const versions
		std::is_same<T, const unsigned int>::value) {

		sprintf(formArr, "%u", var);
	}
	//signed longs and long longs
	else if constexpr (std::is_same<T, long int>::value || std::is_same<T, long long int>::value ||
		//const versions
		std::is_same<T, const long int>::value || std::is_same<T, const long long int>::value) {

		sprintf(formArr, "%lld", (long long int)var);
	}
	//unsigned long and long longs
	else if constexpr (std::is_same<T, unsigned long int>::value || std::is_same<T, unsigned long long int>::value ||
		//const versions
		std::is_same<T, const unsigned long int>::value || std::is_same<T, const unsigned long long int>::value) {

		sprintf(formArr, "%llu", (unsigned long long int)var);
	}
	//float types:
	// default precision(6), use sprintf on your array and writeString manually
	//float and double
	else if constexpr (std::is_same<T, float>::value || std::is_same<T, double>::value ||
		//const versions
		std::is_same<T, const float>::value || std::is_same<T, const double>::value) {

		sprintf(formArr, "%f", var);
	}
	else if constexpr (std::is_same<T, long double>::value ||
		//const versions
		std::is_same<T, const long double>::value) {

		sprintf(formArr, "%L", var);
	}
	//none above applies, custom type
	else {
		writeData_ref(var);
		//printf("None of the types apply!\n");
		return;
	}
	writeString(formArr, false, useAutoFlush);
}


// open file with flags
// uses const char* as all normal string function do
bool AEFileWriter::open(const char* str, const uint8_t flags) {
	if (strlen(str)) {
		m_sFilename = str;
		uint64_t found = m_sFilename.rfind("/");
		if (found != std::string::npos) {
			std::filesystem::create_directories(m_sFilename.substr(0, found));
		}

		switch (flags) {


		case AEFW_FLAG_APPEND://cursor at end, allow changing cursor position
			openDirectly(str, "r+b");
			if (!m_fpFilestr) {//couldnt open, force create
				openDirectly(str, "wb");
				if (!m_fpFilestr) {
					throw std::runtime_error("File could not be opened nor created!");
				}
			}
			fseek(m_fpFilestr, 0L, SEEK_END);
			break;

		case AEFW_FLAG_APPEND_NO_CURSOR_MOVE://only appending, not allowing to change cursor position
			openDirectly(str, "ab");
			break;


		case AEFW_FLAG_NOFLAGS://no flags, defaulting to truncation
		case AEFW_FLAG_TRUNCATE://truncate file
			openDirectly(str, "wb");
			break;

		default:
			m_ucLastError = AEFW_ERR_FILE_WRONG_FLAG;
			break;
		}
		if (!m_fpFilestr) {//file is still somehow nonexistent
			m_ucLastError = AEFW_ERR_FILE_OPEN_ELSE;
			return false;
		}
		return true;
	}
	else {
		m_sFilename.clear();
		m_ucLastError = AEFW_ERR_FILE_NAME_EMPTY;
		return false;
	}

}



#endif //!AEFILEWRITER_HPP
