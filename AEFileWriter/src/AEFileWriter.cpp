
/** @file AEFileWriter/src/AEFileWriter.cpp
 *  This file contains the definitions of the engine's file writer's functions.
 *  @see AEFileWriter
 *  @see AEFileWriter.hpp
 *
 *  Should not cause everything to break.
 */

#include "../include/AEFileWriter.hpp"

AEFileWriter::AEFileWriter(const std::string_view filename, const cint flags, const ullint af_interval) :
	m_ullFlushInterval(af_interval), m_ullTotalWrites(0), m_szLastWrittenAmount(0),
	 m_fpFilestr(nullptr), m_cLastError(AEFW_ERR_NOERROR), m_cFlags(flags) {

	this->openFile(filename, flags);
}


AEFileWriter::AEFileWriter(AEFileWriter&& fw) noexcept :
	m_ullFlushInterval(fw.m_ullFlushInterval), m_sFilename(std::move(fw.m_sFilename)),
	m_ullTotalWrites(fw.m_ullTotalWrites), m_szLastWrittenAmount(fw.m_szLastWrittenAmount),
	m_fpFilestr(fw.m_fpFilestr), m_cLastError(fw.m_cLastError), m_cFlags(fw.m_cFlags) {

	fw.m_ullFlushInterval = 0;
	fw.m_ullTotalWrites = 0;
	fw.m_szLastWrittenAmount = 0;
	fw.m_fpFilestr = nullptr;
	fw.m_cLastError = 0;
	fw.m_sFilename.clear();
	fw.m_cFlags = 0;
}

// write data as binary function
// uses const char
cint AEFileWriter::writeData_ptr(const void* const cdata, const std::size_t dcount, const std::size_t dsize, const bool useAutoFlush) noexcept {

	_AEFW_EXIT_ON_WRITE_CLOSED_FILE;

	if (!bool(cdata) || !bool(dcount) || !bool(dsize)) {
		return AEFW_ERR_WRITE_ZERO_SIZE;
	}

	this->m_szLastWrittenAmount = fwrite(cdata, dsize, dcount, this->m_fpFilestr);
	this->m_ullTotalWrites++;
	if (useAutoFlush) {
		this->autoFlush();
	}

	if (m_szLastWrittenAmount != dcount) {
		return AEFW_ERR_WRITE_ERROR;
	}

	return AEFW_ERR_WRITE_SUCCESS;
}


// open file with flags
int AEFileWriter::openFile(const std::string_view str, const cint flags, const ullint af_interval) {

	if (this->isOpen()) { // open already -> pls close
		this->closeFile();
	}

	if (str.empty()) {
		this->m_sFilename.clear();
		this->m_cLastError = AEFW_ERR_FILE_NAME_EMPTY;
		return AEFW_ERR_FILE_NAME_EMPTY;
	}

	this->m_szLastWrittenAmount = 0;
	this->m_sFilename = str;
	std::replace(m_sFilename.begin(), m_sFilename.end(), '\\', '/');
	const std::size_t found = this->m_sFilename.rfind('/');
	if (found != std::string::npos) {
		std::filesystem::create_directories(this->m_sFilename.substr(0, found));
	}

	switch (flags) {

	case AEFW_FLAG_APPEND://cursor at end, allow changing cursor position
		this->m_fpFilestr = ace::utils::fopenCC(str.data(), "r+b");
		if (this->isClosed()) {//couldn't open, force create
			this->m_fpFilestr = ace::utils::fopenCC(str.data(), "wb");
			if (this->isClosed()) {
				break; // some other error
			}
		}
		fseek(this->m_fpFilestr, 0L, SEEK_END);
		break;

	case AEFW_FLAG_APPEND_NO_CURSOR_MOVE://only appending, not allowing to change cursor position
		this->m_fpFilestr = ace::utils::fopenCC(str.data(), "ab");
		break;


	case AEFW_FLAG_NOFLAGS://no flags, defaulting to truncation
	case AEFW_FLAG_TRUNCATE://truncate file
		this->m_fpFilestr = ace::utils::fopenCC(str.data(), "wb");
		break;

	default:
		this->m_cLastError = AEFW_ERR_FILE_WRONG_FLAG; //wrong flag, not opening the file
		return AEFW_ERR_FILE_WRONG_FLAG;
	}

	// last check, to see if everything is okay
	if (this->isClosed()) {
		this->m_cLastError = AEFW_ERR_FILE_OPEN_ELSE; //file is still somehow nonexistent
		return AEFW_ERR_FILE_OPEN_ELSE;
	}

	//then everything is good, 
	this->m_cFlags = flags;
	if (this->m_ullFlushInterval != AEFW_DEFAULT_AUTOFLUSH_INTERVAL) {
		this->m_ullFlushInterval = af_interval;
	}
	return AEFW_ERR_NOERROR;
}
