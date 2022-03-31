
#pragma once

#ifndef ENGINE_AEFILEWRITER_HPP
#define ENGINE_AEFILEWRITER_HPP

#include <cstring>
#include <vector>
#include <filesystem>
#include "AEModuleBase.hpp"


//File flags

///Write cursor at the end of the file, adding to the file
#define AEFW_FLAG_APPEND 1
///Write cursor at the end of the file, adding to the file
///Changing the cursor position(fseek)/writing anywhere else is not allowed
#define AEFW_FLAG_APPEND_NO_CURSOR_MOVE 2
///Write cursor at the start of the file, truncating the contents if existed
#define AEFW_FLAG_TRUNCATE 3
///No Write flags
#define AEFW_FLAG_NOFLAGS 255

//Error flags

///indicator that everything is good
#define AEFW_ERR_NOERROR 0


//file creation/manipulation
///If the file name is empty
#define AEFW_ERR_FILE_NAME_EMPTY 1
///If the flag given is wrong during the creation(creates file anyway, assuming AEFW_FLAG_NOFLAGS)
#define AEFW_ERR_FILE_WRONG_FLAG 2
///If the file couldn't be created for some other reason, like missing permissions to access files
#define AEFW_ERR_FILE_ELSE 3
///If the file object we're trying to write to is null,
///aka file not open.
///Usually will come after AEFW_ERR_FILE_NAME_EMPTY or AEFW_ERR_FILE_ELSE,
///if we continue to access the writer.
#define AEFW_ERR_WRITE_FILE_NULL 4


///File writer. Err, Writes data to given file
///Hungarian notation is fw
///Flags start with AEFW_
class AEFileWriter : public __AEModuleBase<AEFileWriter>{
public:
	/// <summary>
	/// Class constructor
	/// </summary>
	/// <param name="filename">Name of the file, with extension</param>
	/// <param name="flags">Flags for file opening: 0 -- nothing, 1 -- append to end; 2 -- clear file when opening</param>
	/// <param name="af_interval">interval in file writes between automatic file flushing </param>
	AEFileWriter(const std::string& filename = "", const smalluint flags = AEFW_FLAG_APPEND, const biguint af_interval = 1) :
		m_autoflushInterval(af_interval), m_ullTotalWrites(0), m_fpFilestr(nullptr),
		m_ucLastError(AEFW_ERR_NOERROR) {

		this->open(filename.c_str(), flags);
	}

	~AEFileWriter(){
		flushFile();
		closeFile();
	}
	/// <summary>
	/// Write std::string, as is, to file, and flush if necessary
	/// </summary>
	/// <param name="str">String to write</param>
	/// <param name="includeNull">Flag to include 1 null-terminating character in the string</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in autoflush_interval</param>
	inline void writeString(const std::string& str, const bool includeNull = false, const bool useAutoFlush = true){
		this->writeData_ptr(str.c_str(), str.size() + ((includeNull)?1:0), sizeof(char), useAutoFlush);
	}


	/// <summary>
	/// Write std::vector of char (string), as is, to file, and flush if necessary
	/// </summary>
	/// <param name="str">String(in form of vector<char>) to write</param>
	/// <param name="includeNull">Flag to include 1 null-terminating character in the string</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in autoflush_interval</param>
	inline void writeString(const std::vector<char>& str, const bool includeNull = false, const bool useAutoFlush = true){
		// since vector may actually have NULL-terminating char
		// remove it if includeNull is false
		// decrease str.size() by 1
		this->writeData_ptr(str.data(), str.size() - ( ( !includeNull && str[str.size()-1] == '\0' )?1:0), sizeof(char), useAutoFlush);
	}


	/// <summary>
	/// Write c-string, as is, to file, and flush if necessary
	/// </summary>
	/// <param name="cdata">С-String to write</param>
	/// <param name="dcount">length of string</param>
	/// <param name="includeNull">Flag to include 1 null-terminating character in the string</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in autoflush_interval</param>
	inline void writeString(const char* cdata, const bool includeNull = false, const bool useAutoFlush = true){
		this->writeData_ptr(cdata, strlen(cdata) + ((includeNull) ? 1 : 0), sizeof(char), useAutoFlush);
	}


	/// <summary>
	/// Write binary data, as is, to file, and flush if necessary
	/// Difference with writeData_ref(): uses (const) pointer to the variable/data
	/// @note Doesn't work(fine) with literals
	/// </summary>
	/// <param name="cdata">(pointer to) object to write</param>
	/// <param name="dcount">number of elements in an object</param>
	/// <param name="dsize">size, in bytes, for each element</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in autoflush_interval</param>
	void writeData_ptr(const char* cdata, const biguint dcount, const biguint dsize = sizeof(char), const bool useAutoFlush = true){
		if(isOpen()){
			fwrite(cdata, dsize, dcount, m_fpFilestr);
			if(useAutoFlush){
				if(!(++m_ullTotalWrites % m_autoflushInterval)){
					flushFile();
				}
			}
			m_ucLastError = AEFW_ERR_NOERROR;
		}
		else{
			m_ucLastError = AEFW_ERR_WRITE_FILE_NULL;
			return;
		}
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
	inline void writeData_ref(const T& cdata, const biguint tsize = sizeof(T), const bool useAutoFlush = true){
		writeData_ptr(&cdata, 1, tsize, useAutoFlush);
	}

//TODO: Maybe rewrite to million template overloads
//idk
	/// <summary>
	/// Determines type of the data passed and uses according procedure to write it to file
	/// @note character string types(vector char, std::string, char*) do not include null-termination character
	/// Invokes other member functions
	/// </summary>
	/// <param name="cdata">(pointer to) object to write</param>
	/// <param name="tsize">size of the variable, default is the sizeof()</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in autoflush_interval</param>
	template<typename T>
	inline void write(const T& var, const bool useAutoFlush = true){

		//char array for formatting
		//classic 4*size of the variable
		char formArr[sizeof(var)*4];
		
		//great line of if's to check the types
		//since we don't have a constexpr version of switch
		//and we don't need it anyway
		
		//checks for types that don't require formatting with sprintf
		//single chars
		if constexpr(std::is_same<T, char>::value || std::is_same<T, smalluint>::value ||
			//const versions
			std::is_same<T, const char>::value || std::is_same<T, const smalluint>::value){
			
			//the type is well, char
			writeData_ptr(&var, 1, sizeof(char), useAutoFlush);
			return;
		}
		//c-strings
		else if constexpr (std::is_same<T, char*>::value || std::is_same<T, smalluint*>::value || 
			//const versions
			std::is_same<T, const char*>::value || std::is_same<T, const smalluint*>::value) {
			//no null-terminating char
			//use writeString() directly instead
			writeString((const char*)var, false, useAutoFlush);
			return;
		}
		//std::vector's of char or std::string's
		//no unsigned char because you cannot just
		//cast std::vector of unsigned char to std::vector of char
		else if constexpr (std::is_same<T, std::vector<char>>::value || std::is_same<T, std::string>::value ||
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
			std::is_same<T, const unsigned int>::value ) {
			
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
		else{
			writeData_ref(var);
			//printf("None of the types apply!\n");
			return;
		}
		writeString(formArr, false, useAutoFlush);
	}

	/// <summary>
	/// Open(and create) file with given name and flag
	/// </summary>
	/// <param name="filename">Name of the file, with extension</param>
	/// <param name="flags">Flags for file opening, AEFW_FLAG_* macros. More info in the docs</param></param>
	/// <returns>True if file is open(by currently used writer), false otherwise</returns>
	inline bool open(const std::string& str, const smalluint flags = AEFW_FLAG_APPEND){
		return this->open(str.c_str(), flags);
	}
	/// <summary>
	/// Open(and create) file with given name and flag
	/// </summary>
	/// <param name="filename">Name of the file, with extension</param>
	/// <param name="flags">Flags for file opening, AEFW_FLAG_* macros. More info in the docs</param></param>
	/// <returns>True if file is open(by currently used writer), false otherwise</returns>
	bool open(const char* str, const smalluint flags = AEFW_FLAG_APPEND){
		if(strlen(str)){
			m_sFilename = str;
			biguint found = m_sFilename.rfind("/");
			if (found!=std::string::npos){
				std::filesystem::create_directories(m_sFilename.substr(0,found));
			}
			
			switch(flags){


				case AEFW_FLAG_APPEND://cursor at end, allow changing cursor position
					openDirectly(str, "r+b");
					if(!m_fpFilestr){//couldnt open, force create
						openDirectly(str, "wb");
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
			if(!m_fpFilestr){//file is still somehow nonexistent
				m_ucLastError = AEFW_ERR_FILE_ELSE;
				return false;
			}
			return true;
		}
		else{
			m_sFilename.clear();
			m_ucLastError = AEFW_ERR_FILE_NAME_EMPTY;
			return false;
		}

	}
	
	///Flushes the opened file. That's it.
	inline void flushFile() const{
		if(m_fpFilestr)
			fflush(m_fpFilestr);
	}

	///close current file. return 0 if closed
	inline bool closeFile(){
		if(m_fpFilestr)
			fclose(m_fpFilestr);
		m_fpFilestr = nullptr;
		return bool(this->m_fpFilestr);//null if closed, something other if opened
	}

	///checks if file is open, true if it is, false otherwise
	inline bool isOpen() const{
		return bool(this->m_fpFilestr);//null if closed, something other if opened
	}
	
	///returns last error of writer
	inline smalluint getLastError() const{
		return this->m_ucLastError;
	}
	
	///returns name of currently open file
	inline std::string getFileName() const{
		return this->m_sFilename;
	}

	///retuns total write requests made to file
	inline biguint getTotalWrites() const{
		return this->m_ullTotalWrites;
	}
	///refreshes error status variable and sets it to AEFW_ERR_NOERROR
	inline void refreshErr(){
		this->m_ucLastError = AEFW_ERR_NOERROR;
	}
	/// Interval in file writings before flush.
	/// Set to 1 -- flush every time;
	/// Set to BINT_MAX macro -- (practically) never explicitly auto-flush,
	/// flushing falls back to whatever OS does.
	biguint m_autoflushInterval;
private:
	/// <summary>
	/// Opens file with given name and flags.
	/// Needed so i dont need to retype ifdef for each of file openings
	/// Makes code cleaner.
	/// </summary>
	/// <param name="fname"></param>
	/// <param name="flags"></param>
	inline void openDirectly(const char* fname, const char* flags) {
		//safety, so our compiler shuts up about the unsafe and deprecated function
		//and trigger only on vc++

//if our compiler is vc++
#ifdef _MSC_VER 
		fopen_s(&m_fpFilestr, fname, flags);
#else
//or some other stuff
		m_fpFilestr = fopen(fname, flags);
#endif // _MSC_VER 

	}

	///full filename
	std::string m_sFilename;
	///counter for total write requests for file
	biguint m_ullTotalWrites;
	///object for file writing
	FILE* m_fpFilestr;
	///writer's error indicator
	///Values are AEFW_ERR_* macros
	///More info in the docs
	smalluint m_ucLastError;

};

ADD_MODULE_TO_ENGINE(AEFileWriter)


#endif //!ENGINE_AEFILEWRITER_HPP
