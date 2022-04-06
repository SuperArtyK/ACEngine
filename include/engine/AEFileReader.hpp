
#pragma once

#ifndef ENGINE_AEFILEREDAER_HPP
#define ENGINE_AEFILEREADER_HPP

//code for AEFileReader module
//reads files
#include <cstring>
#include <vector>
#include <filesystem>
#include "AEModuleBase.hpp"

//Error flags

///indicator that everything is good
#define AEFR_ERR_NOERROR 0
//file creation/manipulation
///If the file name is empty
#define AEFR_ERR_FILE_NAME_EMPTY 1
///If the file couldn't be opened for some other reason, like missing permissions to access files
#define AEFR_ERR_FILE_OPEN_ELSE 2
///If the file object we're trying to read from is null,
///aka file not open.
///Usually will come after AEFR_ERR_FILE_NAME_EMPTY or AEFR_ERR_FILE_OPEN_ELSE,
///if we continue to access the reader.
#define AEFR_ERR_READ_FILE_NULL 3
///Raised if encountered EOF when reaing a file
#define AEFR_ERR_EOF 4

/// File reader. Err, reads data from given file
/// Hungarian notation is fr
/// Flags start with AEFR_
class AEFileReader : public __AEBasicModule<AEFileReader>{
public:
	explicit AEFileReader(const std::string& filename = "") :
	m_ullReadBytes(0), m_fpFilestr(nullptr),
		m_ucLastError(AEFR_ERR_NOERROR) {

		this->open(filename.c_str());
	}

	~AEFileReader(){
		closeFile();
	}
//open/close operations

	/// <summary>
	/// Open(and create) file with given name and flag
	/// </summary>
	/// <param name="filename">Name of the file, with extension</param>
	/// <param name="flags">Flags for file opening, AEFW_FLAG_* macros. More info in the docs</param></param>
	/// <returns>True if file is open(by currently used reader), false otherwise</returns>
	bool open(const char* str){
		if(strlen(str)){
			m_sFilename = str;
			biguint found = m_sFilename.rfind("/");
			if (found!=std::string::npos){
				std::filesystem::create_directories(m_sFilename.substr(0,found));
			}
			//if our compiler is vc++
#ifdef _MSC_VER 
		fopen_s(&m_fpFilestr, str, "r");
#else
//or some other stuff
		m_fpFilestr = fopen(str, "r");
#endif // _MSC_VER 
			if(!m_fpFilestr){//file is still somehow not opened
				m_ucLastError = AEFR_ERR_FILE_OPEN_ELSE;
				return false;
			}
			return true;
		}
		else{
			m_sFilename.clear();
			m_ucLastError = AEFR_ERR_FILE_NAME_EMPTY;
			return false;
		}

	}

	///close current file. return 0 if closed
	inline bool closeFile(){
		if(m_fpFilestr)
			fclose(m_fpFilestr);
		m_fpFilestr = nullptr;
		return bool(this->m_fpFilestr);//null if closed, something other if opened
	}

// readers
	/// <summary>
	/// Reads <amount> bytes from opened file to array in memory<arr>
	/// </summary>
	/// @warning Function doesn't do any safety checks for array. It just writes to memory address
	/// @note If encountered end of file(eof) when reading bytes, error flag AEFR_ERR_EOF is set. All bytes before eof are read.
	/// <param name="arr">Array in memory to read data to. It *has* to be same size/bigger than amount</param>
	/// <param name="amount">Amount of bytes to read</param></param>
	inline void readBytes(void* arr, const biguint amount) {
		if(m_fpFilestr){
			m_ullReadBytes = fread(arr, 1, amount, m_fpFilestr);
			if(amount != m_ullReadBytes){
				m_ucLastError = AEFR_ERR_EOF;
			}
		}
		else{
			m_ucLastError = AEFR_ERR_READ_FILE_NULL;
		}
	}

	/// <summary>
	/// Reads <amount> bytes from opened file to std::vector<unsigned char><arr>
	/// Resizes given std::vector<unsigned char> to hold (at least) <amount> of chars.
	/// </summary>
	/// @note If encountered end of file(eof) when reading bytes, error flag AEFR_ERR_EOF is set. All bytes before eof are read.
	/// <param name="arr">Array in memory to read data to. It *has* to be same size/bigger than amount</param>
	/// <param name="amount">Amount of bytes to read</param></param>
	inline void readBytes(std::vector<smalluint>& arr, const biguint amount) {
		if(m_fpFilestr){
			if(arr.size()<amount){
				arr.resize(amount); // just to be sure it's big enough
			}
			m_ullReadBytes = fread(arr.data(), 1, amount, m_fpFilestr);
			if(amount != m_ullReadBytes){
				m_ucLastError = AEFR_ERR_EOF;
			}
		}
		else{
			m_ucLastError = AEFR_ERR_READ_FILE_NULL;
		}
	}

	// lazy :)
	/// <summary>
	/// Reads <amount> of character from opened file to c-string <str>
	/// </summary>
	/// @warning Function doesn't do any safety checks for array. It just writes to memory address
	/// @note If encountered end of file(eof) when reading bytes, error flag AEFR_ERR_EOF is set. All bytes before eof are read.
	/// @note Function doesn't check for non-ascii characters, deal with them yourself.
	/// <param name="str">Array in memory to read data to. It *has* to be same size/bigger than amount</param>
	/// <param name="amount">Amount of bytes to read. Shall be greater than 0</param></param>
	inline void readString(char* str, const biguint amount){
		readBytes(str, amount * sizeof(char));
	}

	/// <summary>
	/// Reads <amount> of character from opened file to std::string <str>
	/// Resizes given std::string to hold (at least) <amount> of chars.
	/// </summary>
	/// @note If encountered end of file(eof) when reading bytes, error flag AEFR_ERR_EOF is set. All bytes before eof are read.
	/// @note Function doesn't check for non-ascii characters, deal with them yourself.
	/// <param name="str">std::tring to read data to. It *has* to be same size/bigger than amount</param>
	/// <param name="amount">Amount of bytes to read. Shall be greater than 0</param></param>
	inline void readString(std::string& str, const biguint amount){
		if(str.size()<amount){
			str.resize(amount); // just to be sure it's big enough
		}
		// evil hack, avoid c_str, use address of first element
		//readBytes(&str[0], amount * sizeof(char));
		//use c++17 feature instead; string::data has non-const overload
		readBytes(str.data(), amount * sizeof(char));
	}

	/// <summary>
	/// Reads <amount> of character from opened file to std::vector<char> <str>
	/// Resizes given std::vector<char> to hold (at least) <amount> of chars.
	/// </summary>
	/// @note If encountered end of file(eof) when reading bytes, error flag AEFR_ERR_EOF is set. All bytes before eof are read.
	/// @note Function doesn't check for non-ascii characters, deal with them yourself.
	/// <param name="str">std::vector<char> to read data to. It *has* to be same size/bigger than amount</param>
	/// <param name="amount">Amount of bytes to read. Shall be greater than 0</param></param>
	inline void readString(std::vector<char>& str, const biguint amount){
		if(str.size()<amount){
			str.resize(amount); // just to be sure it's big enough
		}
		// evil hack, avoid c_str, use address of first element
		//readBytes(&str[0], amount * sizeof(char));
		//use c++17 feature instead; string::data has non-const overload
		readBytes(str.data(), amount * sizeof(char));
	}
	//TODO: Maybe rewrite to million template overloads
//idk
	/// <summary>
	/// Determines type of the data passed and uses according procedure to read to it from file
	/// Invokes other member functions
	/// @note character string types(vector char, std::string, char*) do not include null-termination character
	/// </summary>
	/// <param name="var">variable to read to</param>
	/// <param name="amount">Amount of data to read. Required if passing string types(std::string, c-string, std::vector [unsigned] char), not checked otherwise.</param>
	/// <param name="isbinary">Flag if data to read should be interpreted as binary</param>
	template<typename T>
	void read(T& var, const biguint amount = 0, const bool isbinary = false){

		//if asked for binary or char, just read to memory what is in file
		if constexpr (std::is_same<T, char>::value || 
			std::is_same<T, unsigned char>::value ||
			//const versions
			std::is_same<T, const char>::value || 
			std::is_same<T, const unsigned char>::value ||
			std::is_same<T, bool>::value) {
			readBytes(&var, sizeof(var));
			return;
		}
		if(isbinary){
			readBytes(&var, sizeof(var));
		}

		//strings		c-strings
		if constexpr (std::is_same<typename std::decay<T>::type, char*>::value || std::is_same<typename std::decay<T>::type, unsigned char*>::value ||
			//const versions
			std::is_same<typename std::decay<T>::type, const char*>::value || std::is_same<typename std::decay<T>::type, const unsigned char*>::value ||
			//std::vector's of char or std::string's
			std::is_same<T, std::vector<char>>::value || std::is_same<T, std::string>::value ||
			//const versions
			std::is_same<T, const std::vector<char>>::value || std::is_same<T, const std::string>::value) {
			//no null-terminating char
			//use writeString() directly instead
			readString(var, amount);
			return;
		}

		//char array for formatting
		//classic 4*size of the variable
		char formArr[sizeof(var)*4];

		if constexpr (std::is_same<T, int>::value || std::is_same<T, const int>::value) {
			fscanf(m_fpFilestr, "%d", &var);
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


	}

//getters and setters
	///checks if file is open, true if it is, false otherwise
	inline bool isOpen() const{
		return bool(this->m_fpFilestr);//nullptr if closed, something other if opened
	}
	
	///returns last error of reader
	inline int getLastError() const{
		return this->m_ucLastError;
	}
	
	///returns name of currently open file
	inline std::string getFileName() const{
		return this->m_sFilename;
	}

	inline biguint getReadBytes() const{
		return this->m_ullReadBytes;
	}
	
	///refreshes error status variable and sets it to AEFW_ERR_NOERROR
	inline void refreshErr(){
		this->m_ucLastError = AEFR_ERR_NOERROR;
	}

	///retursn size of the file in bytes
	inline bigint getFileSize(){
		if(m_fpFilestr){
			const bigint temp[2] = {
				ftell(m_fpFilestr),//get current position
				fseek(m_fpFilestr, 0, SEEK_END) + ftell(m_fpFilestr),//eval to end pos of file. If everything is alright, fseek evaluates to 0, and everything's great
			};
			return temp[1] +// end pos
				fseek(m_fpFilestr, temp[0], SEEK_SET); // evaluate to 0 if everything is alright
		}
		else{
			m_ucLastError = AEFR_ERR_READ_FILE_NULL;
			return 0;
		}
	}

	///returns current read cursor position, starting from 0 as beginning
	inline bigint getCursorPos() {
		if (m_fpFilestr){
			return ftell(m_fpFilestr);
		}
		else{
			m_ucLastError = AEFR_ERR_READ_FILE_NULL;
			return 0;
		}
	}

	/// sets read cursor position to <offset> from <origin>
	/// See SEEK_SET, SEEK_CUR and SEEK_END for more details
	/// @warning If cursor is reqeusted to be set further than file size, error flag is set to AEFR_ERR_EOF. (I mean, you get EOF when you try to read a byte with cursor outside of file.)
	// ex: 1 from SEEK_SET is 1 byte from beginning == 2nd byte in file
	inline void setCursorPos(const bigint offset, const int origin = SEEK_CUR){
		if(m_fpFilestr){
			fseek(m_fpFilestr, offset, origin);
			if(feof(m_fpFilestr)){
				m_ucLastError = AEFR_ERR_EOF;
			}
		}
		else{
			m_ucLastError = AEFR_ERR_READ_FILE_NULL;
		}
		
	}


private:
	///full filename
	std::string m_sFilename;
	///counter for read bytes by last operation
	biguint m_ullReadBytes;
	///object for file writing
	FILE* m_fpFilestr;
	///reader's error indicator
	///Values are AEFR_ERR_* macros
	///More info in the docs
	smalluint m_ucLastError;
};

ADD_MODULE_TO_ENGINE(AEFileReader)

#endif // !ENGINE_AEFILEREADER_HPP
