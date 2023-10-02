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


class AEFileReader : public __AEModuleBase<AEFileReader> {

public:



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
	/// Flags that were used to open the file
	ucint m_ucFlags;


};

REGISTER_CLASS(AEFileReader)


#endif // !ENGINE_NAME_HPP
