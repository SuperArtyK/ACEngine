#include "../include/AEFileReader.hpp"



AEFileReader::AEFileReader(const std::string_view fname) :
	m_sFilename(fname), m_ullTotalReads(0), m_szLastReadAmount(0), m_fpFilestr(nullptr), m_cLastError(AEFR_ERR_NOERROR) {

	this->openFile(fname);
}

int AEFileReader::openFile(const std::string_view fname) {
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

//reads the string of set size, Unfilled characters (if eof) are zeroed. MUST be of the dcount+1 size!
//if file is closed, contents is not modified
cint AEFileReader::readString(char* str, const std::size_t dcount) {
	_AEFR_EXIT_ON_CLOSED_FILE;
	if (!dcount || !str) {
		this->m_cLastError = AEFR_ERR_READ_ZERO_SIZE;
		return AEFR_ERR_READ_ZERO_SIZE;
	}
	std::memset(str, NULL, dcount + 1);
	return this->readData_ptr(str, dcount, sizeof(char));
}

//reads the string untill the newline or, as a safety measure, untill dcount. MUST be of the dcount+1 size!
cint AEFileReader::readStringNL(char* str, const unsigned int dcount) {
	_AEFR_EXIT_ON_CLOSED_FILE;

	if (!dcount || !str) {
		this->m_cLastError = AEFR_ERR_READ_ZERO_SIZE;
		return AEFR_ERR_READ_ZERO_SIZE;
	}

	std::memset(str, NULL, dcount + 1);
	const bool temp = std::fgets(str, dcount, this->m_fpFilestr);
	this->m_szLastReadAmount = std::strlen(str);
	this->m_ullTotalReads++;
	if (!temp) {
		this->m_cLastError = AEFR_ERR_READING_ERROR;
		return AEFR_ERR_READING_ERROR;
	}
	if (std::feof(this->m_fpFilestr)) {
		this->m_cLastError = AEFR_ERR_READING_EOF;
		return AEFR_ERR_READING_EOF;
	}
	return AEFR_ERR_READ_SUCCESS;
}

//reads the string untill the null, or untill dcount
//moves the cursor to the found null char+1
cint AEFileReader::readStringNULL(char* str, const std::size_t dcount) {
	_AEFR_EXIT_ON_CLOSED_FILE;

	if (!dcount || !str) {
		this->m_cLastError = AEFR_ERR_READ_ZERO_SIZE;
		return AEFR_ERR_READ_ZERO_SIZE;
	}

	//clear buffer and read
	std::memset(str, NULL, dcount + 1);
	const cint temp = this->readData_ptr(str, dcount, sizeof(char));

	//look for the null char and move the read cursor to the next char after null (if appeared)
	const char* const nulchar = std::strchr(str, '\0');
	fseek(m_fpFilestr, -llint((dcount - (nulchar - str) - 1)), SEEK_CUR);
	this->m_szLastReadAmount = std::strlen(str);

	return temp;
}

// reads data to ptr
cint AEFileReader::readData_ptr(void* cdata, const std::size_t dcount, const std::size_t dsize) {
	_AEFR_EXIT_ON_CLOSED_FILE;

	if (!dcount || !dsize || !cdata) {
		this->m_cLastError = AEFR_ERR_READ_ZERO_SIZE;
		return AEFR_ERR_READ_ZERO_SIZE;
	}

	this->m_szLastReadAmount = fread(cdata, dsize, dcount, this->m_fpFilestr);
	this->m_ullTotalReads++;
	if (this->m_szLastReadAmount != dcount) {
		if (std::feof(this->m_fpFilestr)) {
			return AEFR_ERR_READING_EOF;
		}
		if (std::ferror(this->m_fpFilestr)) {
			return AEFR_ERR_READING_ERROR;
		}
	}
	return AEFR_ERR_READ_SUCCESS;
}
