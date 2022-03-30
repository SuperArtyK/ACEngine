
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
#define AEFR_ERR_FILE_ELSE 2
///If the file object we're trying to read from is null,
///aka file not open.
///Usually will come after AEFR_ERR_FILE_NAME_EMPTY or AEFR_ERR_FILE_ELSE,
///if we continue to access the reader.
#define AEFR_ERR_READ_FILE_NULL 3
///Raised if encountered EOF when reaing a file
#define AEFR_ERR_EOF 4

/// File reader. Err, reads data from given file
/// Hungarian notation is fr
/// Flags start with AEFR_
class AEFileReader : public __AEModuleBase<AEFileReader>{
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
	/// <returns>True if file is open(by currently used writer), false otherwise</returns>
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
				m_ucLastError = AEFR_ERR_FILE_ELSE;
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
	/// Reads <amount> bytes from opened file to <arr>
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
	// lazy :)
	/// <summary>
	/// Reads <amount> of character from opened file to string <str>
	/// </summary>
	/// @warning Function doesn't do any safety checks for array. It just writes to memory address
	/// @note If encountered end of file(eof) when reading bytes, error flag AEFR_ERR_EOF is set. All bytes before eof are read.
	/// @note Function doesn't check for non-ascii characters, deal with them yourself.
	/// <param name="str">Array in memory to read data to. It *has* to be same size/bigger than amount</param>
	/// <param name="amount">Amount of bytes to read</param></param>
	inline void readString(char* str, const biguint amount){
		readBytes(str, amount * sizeof(char));
	}

	/// <summary>
	/// Reads <amount> of character from opened file to string <str>
	/// </summary>
	/// @warning Function doesn't do any safety checks for array. It just writes to memory address
	/// @note If encountered end of file(eof) when reading bytes, error flag AEFR_ERR_EOF is set. All bytes before eof are read.
	/// @note Function doesn't check for non-ascii characters, deal with them yourself.
	/// <param name="str">Array in memory to read data to. It *has* to be same size/bigger than amount</param>
	/// <param name="amount">Amount of bytes to read</param></param>
	inline void readString(std::string& str, const biguint amount)
	{
		if(str.size()<amount){
			str.resize(amount); // just to be sure
		}
		// evil hack, avoid c_str, use address of first element
		readBytes(&str[0], amount * sizeof(char));
	}
	/// sets read cursor position to <offset> from <origin>
	/// See SEEK_SET, SEEK_CUR and SEEK_END for more details
	// ex: 1 from SEEK_SET is 1 byte from beginning == 2nd byte in file
	inline void changeCursorPos(const bigint offset, const int origin = SEEK_CUR){
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

//getters
	///checks if file is open, true if it is, false otherwise
	inline bool isOpen() const{
		return bool(this->m_fpFilestr);//nullptr if closed, something other if opened
	}
	
	///returns last error of writer
	inline smalluint getLastError() const{
		return this->m_ucLastError;
	}
	
	///returns name of currently open file
	inline std::string getFileName() const{
		return this->m_sFilename;
	}

	inline biguint getReadBytes() const{
		return this->m_ullReadBytes;
	}

	inline bigint getFileSize(){
		if(m_fpFilestr){
			bigint temp = ftell(m_fpFilestr);
			fseek(m_fpFilestr, 0, SEEK_END);
			
		}
		else{
			m_ucLastError = AEFR_ERR_READ_FILE_NULL;
			return 0;
		}
	}

	inline bigint getCursorPos() {
		if (m_fpFilestr)
		{
		}
		else
		{
			m_ucLastError = AEFR_ERR_READ_FILE_NULL;
			return 0;
		}
	}


private:
	///full filename
	std::string m_sFilename;
	///counter for read bytes by last operation
	biguint m_ullReadBytes;
	///object for file writing
	FILE* m_fpFilestr;
	///writer's error indicator
	///Values are AEFR_ERR_* macros
	///More info in the docs
	smalluint m_ucLastError;
};

ADD_MODULE_TO_ENGINE(AEFileReader)

#endif // !ENGINE_AEFILEREADER_HPP
