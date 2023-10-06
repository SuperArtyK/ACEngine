#pragma once

#ifndef ENGINE_NAME_HPP
#define ENGINE_NAME_HPP

#include "include/AEModuleBase.hpp"
#include "include/AETypedefs.hpp"
#include "include/AEUtils.hpp"
#include <cstring>
#include <vector>
#include <filesystem>
#include <string>
#include <string_view>
#include <limits>
#include <cstdio>
#include <stdexcept>


//Error flags
/// Indicator that everything is good.
#define AEFR_ERR_NOERROR 0
#define AEFR_ERR_READ_SUCCESS 0
/// If the file object we're trying to write to not open, file pointer is null
/// @note Functions that access the file will either return this flag or raise this flag
#define AEFR_ERR_FILE_NOT_OPEN -1
//file creation/manipulation
/// Macro for the error if the file name is empty
#define AEFR_ERR_FILE_NAME_EMPTY -2
/// Macro for the error if the file couldn't be created for some other reason, like missing permissions to access files
#define AEFR_ERR_FILE_DOESNT_EXIST -3
#define AEFR_ERR_READING_EOF -4
#define AEFR_ERR_READING_ERROR -5
/// Macro for the error that occurs if the data pointer, item count and item size is null/zero in AEFileReader::readData_ptr()
#define AEFR_ERR_WRITE_ZERO_SIZE -6


/// Macro for the shortened "check for opened file, set error flag and return error flag if closed", DO NOT TOUCH!
#define _AEFR_EXIT_ON_CLOSED_FILE if (!this->m_fpFilestr) { this->m_cLastError = AEFR_ERR_FILE_NOT_OPEN; return AEFR_ERR_FILE_NOT_OPEN; }

// crosscompile stuff
// do not touch
#ifdef _WIN32
#define fscanf fscanf_s
#endif

/// @todo add default constructor with no argumets
/// @todo add function to get the file size
/// @todo add function to get/set cursor position
/// @todo add function to get file reads
/// @todo add function to check if file is open
/// @todo add function to clear error status
/// @todo add readString function overloads for std::string and std::vector<char>
/// @todo think about returning the values from the readX function. Except readString?
class AEFileReader : public __AEModuleBase<AEFileReader> {

public:

	explicit AEFileReader(const std::string_view fname) : 
		m_sFilename(fname), m_ullTotalReads(0), m_fpFilestr(nullptr), m_cLastError(AEFR_ERR_NOERROR) {

		this->openFile(fname);
	}
	

	inline int openFile(const std::string_view fname) {
		if (!fname.size()) {
			this->m_cLastError = AEFR_ERR_FILE_NAME_EMPTY;
			return AEFR_ERR_FILE_NAME_EMPTY;
		}

		this->m_sFilename = fname;
		this->m_fpFilestr = ace::utils::fopenCC(fname.data(), "rb");

		if (!this->m_fpFilestr) {
			// this can also be triggered if program has no permission to access the file
			// but generally it just would be the absence of file
			this->m_cLastError = AEFR_ERR_FILE_DOESNT_EXIST;
			return AEFR_ERR_FILE_DOESNT_EXIST;
		}

		return AEFR_ERR_NOERROR;
	}

	/// <summary>
	/// Closes the currently opened file, and also, in addition, clears the last error status.
	/// </summary>
	inline void closeFile(void) {
		if (this->m_fpFilestr)
			fclose(this->m_fpFilestr);
		this->m_fpFilestr = nullptr;
		this->m_sFilename.clear();
		clearError();
	}


	/// <summary>
	/// Clears last error status variable and sets it to AEFW_ERR_NOERROR.
	/// </summary>
	inline void clearError(void) {
		this->m_cLastError = AEFR_ERR_NOERROR;
	}
	

	template<typename T>
	inline cint readFloatString(T& num) {
		static_assert(std::is_floating_point<T>::value, "Cannot use non-float types in AEFileReader::readFloatString()");

		_AEFR_EXIT_ON_CLOSED_FILE;

		std::memset(&num, NULL, sizeof(T));
		cint temp = 0;

		if constexpr (IS_SAME_NOC(T, float)) {
			temp = fscanf(m_fpFilestr, "%f", &num);
		}
		else if constexpr (IS_SAME_NOC(T, long double)) {
			temp = fscanf(m_fpFilestr, "%lf", &num);
		}
		else {
			temp = fscanf(m_fpFilestr, "%Lf", &num);
		}

		this->m_ullTotalReads++;
		if (temp != 1) {
			this->m_cLastError = AEFR_ERR_READING_EOF;
			return AEFR_ERR_READING_EOF;
		}
		return AEFR_ERR_READ_SUCCESS;
	}

	template<typename T>
	inline cint readIntString(T& num) {
		static_assert(std::is_integral<T>::value, "Cannot use non-integral types in AEFileReader::readIntString()");

		_AEFR_EXIT_ON_CLOSED_FILE;

		std::memset(&num, NULL, sizeof(T));	
		cint temp = 0;
		if constexpr (IS_SAME_NOC(T, short)) {
			temp = fscanf(m_fpFilestr, "%hd", &num);
		}
		else if constexpr (IS_SAME_NOC(T, unsigned short)) {
			temp = fscanf(m_fpFilestr, "%hud", &num);
		}
		else if constexpr (IS_SAME_NOC(T, int)) {
			temp = fscanf(m_fpFilestr, "%d", &num);
		}
		else if constexpr (IS_SAME_NOC(T, unsigned int)) {
			temp = fscanf(m_fpFilestr, "%ud", &num);
		}
		else if constexpr (IS_SAME_NOC(T, long)) {
			temp = fscanf(m_fpFilestr, "%ld", &num);
		}
		else if constexpr (IS_SAME_NOC(T, unsigned long)) {
			temp = fscanf(m_fpFilestr, "%lud", &num);
		}
		else if constexpr (IS_SAME_NOC(T, long long)) {
			temp = fscanf(m_fpFilestr, "%lld", &num);
		}
		else if constexpr (IS_SAME_NOC(T, std::size_t)) {
			temp = fscanf(m_fpFilestr, "%zd", &num);
		}
		else {
			temp = fscanf(m_fpFilestr, "%llud", &num);
		}

		this->m_ullTotalReads++;
		if (temp != 1) {
			this->m_cLastError = AEFR_ERR_READING_EOF;
			return AEFR_ERR_READING_EOF;
		}
		return AEFR_ERR_READ_SUCCESS;
	}
	
	//reads the string untill the null, or untill dcount
	//moves the head back to the found null
	inline cint readStringNULL(char* str, const std::size_t dcount) {
		_AEFR_EXIT_ON_CLOSED_FILE;


		if (!dcount || !str) {
			this->m_cLastError = AEFR_ERR_WRITE_ZERO_SIZE;
			return AEFR_ERR_WRITE_ZERO_SIZE;
		}
		
		std::memset(str, NULL, dcount+1);
		const cint temp = this->readData_ptr(str, 1, dcount);

		char* const nulchar = std::strchr(str, '\0');
		std::memset(nulchar, NULL, dcount - (nulchar - str));

		fseek(m_fpFilestr, -llint((dcount - (nulchar - str) - 1)), SEEK_CUR);

		return temp;
	}

	//reads the string untill the newline or, as a safety measure, untill dcount
	inline cint readStringNL(char* str, const std::size_t dcount) {
		_AEFR_EXIT_ON_CLOSED_FILE;

		if (!dcount || !str) {
			this->m_cLastError = AEFR_ERR_WRITE_ZERO_SIZE;
			return AEFR_ERR_WRITE_ZERO_SIZE;
		}

		std::memset(str, NULL, dcount + 1);
		const bool temp = std::fgets(str, dcount, this->m_fpFilestr);
		this->m_szLastReadAmount = std::strlen(str);
		this->m_ullTotalReads++;
		if (temp) {
			this->m_cLastError = AEFR_ERR_READING_EOF;
			return AEFR_ERR_READING_EOF;
		}
		return AEFR_ERR_READ_SUCCESS;
	}

	//reads the string of set size, Unfilled characters (if eof) are zeroed. MUST be of the dcount size!
	//if file is closed, contents is not modified
	inline cint readString(char* str, const std::size_t dcount) {
		_AEFR_EXIT_ON_CLOSED_FILE;

		if (!dcount || !str) {
			this->m_cLastError = AEFR_ERR_WRITE_ZERO_SIZE;
			return AEFR_ERR_WRITE_ZERO_SIZE;
		}
		std::memset(str, NULL, dcount + 1);
		return this->readData_ptr(str, sizeof(char), dcount);
	}

	// reads the bytes of the size of T and dumps them into given variable
	template<typename T>
	inline cint readVariable(T& var) {
		_AEFR_EXIT_ON_CLOSED_FILE;

		std::memset(&var, NULL, sizeof(T));
		return this->readData_ptr(&var, 1, sizeof(T));
	}

	//reads the bytes of the size of t and interprets them as float
	template<typename T>
	inline cint readFloat(T& num) {
		static_assert(std::is_floating_point<T>::value, "Cannot use non-float types in AEFileReader::readFloat()");
		_AEFR_EXIT_ON_CLOSED_FILE;
		
		std::memset(&num, NULL, sizeof(T));
		return this->readData_ptr(&num, 1, sizeof(T));
	}


	//reads the bytes of the size of t and interprets them as int
	template<typename T>
	inline cint readInt(T& num) {
		static_assert(std::is_integral<T>::value, "Cannot use non-integral types in AEFileReader::readInt()");
		_AEFR_EXIT_ON_CLOSED_FILE;

		std::memset(&num, NULL, sizeof(T));
		return this->readData_ptr(&num, 1, sizeof(T));
	}

	//reads 1 byte of data and dumps it into char
	inline cint readChar(char& num) {
		_AEFR_EXIT_ON_CLOSED_FILE;
		return this->readData_ptr(&num, 1, sizeof(char));
	}

	//reads 1 byte of data and dumps it into unsigned char
	inline cint readByte(unsigned char& num) {
		_AEFR_EXIT_ON_CLOSED_FILE;
		return this->readData_ptr(&num, 1, sizeof(unsigned char));
	}


	//reads bytes to vector
	//if file is closed, vector is not modified
	inline cint readBytes(std::vector<unsigned char>& cdata, const std::size_t dcount) {
		_AEFR_EXIT_ON_CLOSED_FILE;
		cdata.resize(dcount);
		const cint tempreturn = this->readData_ptr(cdata.data(), sizeof(unsigned char), dcount);

		if(!tempreturn) cdata.resize(this->m_szLastReadAmount); //resize to what was actually read
		return tempreturn;
	}

	// reads bytes to ptr
	inline cint readBytes(void* cdata, const std::size_t dcount) {
		return this->readData_ptr(cdata, sizeof(char), dcount);
	}

	// reads data to ptr
	inline cint readData_ptr(void* cdata, const std::size_t dcount, const std::size_t dsize = sizeof(char)) {
		_AEFR_EXIT_ON_CLOSED_FILE;

		if (!dcount || !dsize || !cdata) {
			this->m_cLastError = AEFR_ERR_WRITE_ZERO_SIZE;
			return AEFR_ERR_WRITE_ZERO_SIZE;
		}
		
		this->m_szLastReadAmount = fread(cdata, dsize, dcount, this->m_fpFilestr);
		this->m_ullTotalReads++;
		if (this->m_szLastReadAmount != dcount) {
			return AEFR_ERR_READING_EOF;
		}
		return AEFR_ERR_READ_SUCCESS;
	}


private:

	/// Full filename and relative path
	std::string m_sFilename;
	/// Counter for total write operations for file
	ullint m_ullTotalReads;
	/// The amount of read bytes during last operation
	std::size_t m_szLastReadAmount;
	/// Object for file writing
	FILE* m_fpFilestr;
	/// Writer's last error indicator; Values are AEFW_ERR_* macros
	cint m_cLastError;
};

REGISTER_CLASS(AEFileReader)

#undef fscanf

#endif // !ENGINE_NAME_HPP
