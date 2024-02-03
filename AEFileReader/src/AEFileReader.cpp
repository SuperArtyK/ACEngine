
/*! @file AEFileReader/src/AEFileReader.cpp
 *  This file contains the definitions of the engine's file reader's functions
 *  @see AEFileReader
 *  @see AEFileReader.hpp
 *
 *  Should not cause everything to break :)
 */

#include "../include/AEFileReader.hpp"



AEFileReader::AEFileReader(const std::string_view fname) :
	m_sFilename(fname), m_ullTotalReads(0), m_szLastReadAmount(0), m_fpFilestr(nullptr) {

	this->openFile(fname);
}

AEFileReader::AEFileReader(AEFileReader&& fr) noexcept :
	m_sFilename(std::move(fr.m_sFilename)), m_ullTotalReads(fr.m_ullTotalReads.load()),
	m_szLastReadAmount(fr.m_szLastReadAmount), m_fpFilestr(fr.m_fpFilestr) {

	fr.m_sFilename.clear();
	fr.m_ullTotalReads = 0;
	fr.m_szLastReadAmount = 0;
	fr.m_fpFilestr = nullptr;
}



cint AEFileReader::openFile(const std::string_view fname) {
	
	if (this->isOpen()) {
		return AEFR_ERR_OPEN_FILE_ALREADY_OPENED;
	}


	if (fname.empty()) {
		return AEFR_ERR_OPEN_FILE_NAME_EMPTY;
	}

	this->m_szLastReadAmount = 0;
	this->m_fpFilestr = ace::utils::fopenCC(fname.data(), "rb");

	if (this->isClosed()) {
		// this can also be triggered if program has no permission to access the file
		// but generally it just would be the absence of file
		return AEFR_ERR_OPEN_FILE_DOESNT_EXIST;
	}

	this->m_sFilename = fname;
	return AEFR_ERR_NOERROR;
}

//reads the string of set size, Unfilled characters (if eof) are zeroed. MUST be of the dcount+1 size!
//if file is closed, contents is not modified
cint AEFileReader::readString(char* str, const std::size_t dcount) noexcept {
	_AEFR_EXIT_ON_READ_CLOSED_FILE;

	if (!dcount || !str) {
		return AEFR_ERR_READ_ZERO_SIZE;
	}

	std::memset(str, AENULL, dcount + 1);
	return this->readData_ptr(str, dcount, sizeof(char));
}

//reads the string untill the newline or, as a safety measure, untill dcount. MUST be of the dcount+1 size!
cint AEFileReader::readStringNL(char* str, const int dcount) noexcept {
	_AEFR_EXIT_ON_READ_CLOSED_FILE;

	if (!str || !dcount) {
		return AEFR_ERR_READ_ZERO_SIZE;
	}

	std::memset(str, AENULL, std::size_t(dcount) + 1);
	const bool temp = std::fgets(str, dcount, this->m_fpFilestr);
	this->m_szLastReadAmount = std::strlen(str);
	this->m_ullTotalReads.fetch_add(1, std::memory_order::relaxed);

	if (!temp) {
		return AEFR_ERR_READ_ERROR;
	}

	if (std::feof(this->m_fpFilestr)) {
		return AEFR_ERR_READ_EOF;
	}
	return AEFR_ERR_READ_SUCCESS;
}

//reads the string untill the null, or untill dcount
//moves the cursor to the found null char+1
cint AEFileReader::readStringNULL(char* str, const llint dcount) noexcept {
	_AEFR_EXIT_ON_READ_CLOSED_FILE;

	if (!str || dcount < 1) {
		return AEFR_ERR_READ_ZERO_SIZE;
	}

	//clear buffer and read
	//std::memset(str, AENULL, dcount + 1);
	str[dcount] = '\0'; //set the 0 to the last character (dcount+1) (just in case user didnt)
	const cint temp = this->readData_ptr(str, dcount, sizeof(char));
	this->m_szLastReadAmount = std::strlen(str);
	const llint differenceRead = dcount - llint(this->m_szLastReadAmount); // it's in bounds of the llint dcount anyway;
	//fill the rest of the buffer with 0's
	std::memset(str + this->m_szLastReadAmount, AENULL, differenceRead);

	//look for the null char and move the read cursor to the next char after null (if appeared)
	//const char* const nulchar = std::strchr(str, '\0');
	fseek(m_fpFilestr, -differenceRead, SEEK_CUR);
	

	return temp;
}

cint AEFileReader::readBoolString(bool& num) noexcept {
	_AEFR_EXIT_ON_READ_CLOSED_FILE;
	char str[sizeof("false")]{};

	cint temp = this->readData_ptr(str, 4, sizeof(char)); //read possible "true"
	for (int i = 0; i < 4; i++) {
		str[i] = std::tolower(str[i]);
	}

	if (!std::memcmp(str, "true", sizeof("true"))) {
		num = true;
		return temp;
	}

	temp = this->readData_ptr(&str[4], 1, sizeof(char)); //read last letter to make up "false"
	if (!std::memcmp(str, "false", sizeof("false"))) {
		num = false;
		return temp;
	}
	return AEFR_ERR_READ_ERROR; //whoops, nothing to read!
}

// reads data to ptr
cint AEFileReader::readData_ptr(void* cdata, const std::size_t dcount, const std::size_t dsize) noexcept {
	_AEFR_EXIT_ON_READ_CLOSED_FILE;

	if (!cdata || !dcount || !dsize) {
		return AEFR_ERR_READ_ZERO_SIZE;
	}

	this->m_szLastReadAmount = fread(cdata, dsize, dcount, this->m_fpFilestr);
	this->m_ullTotalReads.fetch_add(1, std::memory_order::relaxed);
	if (this->m_szLastReadAmount != dcount) {
		if (std::feof(this->m_fpFilestr)) {
			return AEFR_ERR_READ_EOF;
		}
		if (std::ferror(this->m_fpFilestr)) {
			return AEFR_ERR_READ_ERROR;
		}
	}
	return AEFR_ERR_READ_SUCCESS;
}
