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
/// If the file object we're trying to write to not open, file pointer is null
/// @note Functions that access the file will either return this flag or raise this flag
#define AEFR_ERR_NOT_OPEN -1
//file creation/manipulation
/// Macro for the error if the file name is empty
#define AEFR_ERR_FILE_NAME_EMPTY -2
/// Macro for the error if the file couldn't be created for some other reason, like missing permissions to access files
#define AEFR_ERR_FILE_DOESNT_EXIST -3



/// @todo add default constructor with no argumets
/// @todo add function to get the file size
/// @todo add function to get/set cursor position
/// @todo add reading of x bytes from file to a void* pointer
/// @todo add reading of x bytes from file to a std::vector<char>/std::vector<unsigned char>
/// @todo add reading of strings of x length
/// @todo add reading of strings of arbitrary length untill null-termination (and max length for...some security)
/// @todo add reading of strings untill the newline character
/// @todo add reading of int types as strings
/// @todo add reading of int types as stream of bytes
/// @todo add reading of float types as strings
/// @todo add reading of float types as stream of bytes
/// @todo add reading of single characters/bytes
/// @todo add reading of bytes to fill the passed variable (ex, pass var of 4 bytes size-> dump 4 bytes from file there)
class AEFileReader : public __AEModuleBase<AEFileReader> {

public:

	explicit AEFileReader(const std::string_view fname) : 
		m_sFilename(fname), m_ullTotalWrites(0), m_fpFilestr(nullptr), m_cLastError(AEFR_ERR_NOERROR) {

		this->openFile(fname);
	}
	

	inline int openFile(const std::string_view fname) {
		if (!fname.size()) {
			this->m_cLastError = AEFR_ERR_FILE_NAME_EMPTY;
			return AEFR_ERR_FILE_NAME_EMPTY;
		}

		this->m_sFilename = fname;
		this->m_fpFilestr = ace::utils::fopenCC(fname.data(), "r");

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

private:

	/// Full filename and relative path
	std::string m_sFilename;
	/// Counter for total write operations for file
	ullint m_ullTotalWrites;
	/// Object for file writing
	FILE* m_fpFilestr;
	/// Writer's last error indicator; Values are AEFW_ERR_* macros
	cint m_cLastError;

};

REGISTER_CLASS(AEFileReader)


#endif // !ENGINE_NAME_HPP
