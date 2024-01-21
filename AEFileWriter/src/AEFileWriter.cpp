
/*! @file AEFileWriter/src/AEFileWriter.cpp
 *  This file contains the definitions of the engine's file writer's functions.
 *  @see AEFileWriter
 *  @see AEFileWriter.hpp
 *
 *  Should not cause everything to break :)
 */

#include "../include/AEFileWriter.hpp"

AEFileWriter::AEFileWriter(const std::string_view filename, const cint flags, const ullint af_interval) :
	m_ullFlushInterval(af_interval), m_ullTotalWrites(0), m_szLastWrittenAmount(0),
	 m_fpFilestr(nullptr), m_cFlags(flags) {

	this->openFile(filename, flags);
}


AEFileWriter::AEFileWriter(AEFileWriter&& fw) noexcept :
	m_ullFlushInterval(fw.m_ullFlushInterval), m_sFilename(std::move(fw.m_sFilename)),
	m_ullTotalWrites(fw.m_ullTotalWrites.load()), m_szLastWrittenAmount(fw.m_szLastWrittenAmount),
	m_fpFilestr(fw.m_fpFilestr), m_cFlags(fw.m_cFlags) {

	fw.m_ullFlushInterval = 0;
	fw.m_ullTotalWrites = 0;
	fw.m_szLastWrittenAmount = 0;
	fw.m_fpFilestr = nullptr;
	fw.m_sFilename.clear();
	fw.m_cFlags = 0;
}

// write data as binary function
// uses const char
cint AEFileWriter::writeData_ptr(const void* const cdata, const std::size_t dcount, const std::size_t dsize, const bool useAutoFlush) noexcept {

	_AEFW_EXIT_ON_WRITE_CLOSED_FILE;

	if (!cdata || !dcount || !dsize) {
		return AEFW_ERR_WRITE_ZERO_SIZE;
	}

	this->m_szLastWrittenAmount = fwrite(cdata, dsize, dcount, this->m_fpFilestr);
	this->m_ullTotalWrites.fetch_add(1, std::memory_order::relaxed);
	if (useAutoFlush) {
		this->autoFlush();
	}

	if (m_szLastWrittenAmount != dcount) {
		return AEFW_ERR_WRITE_ERROR;
	}

	return AEFW_ERR_WRITE_SUCCESS;
}


// open file with flags
cint AEFileWriter::openFile(const std::string_view str, const cint flags, const ullint af_interval) {

	if (this->isOpen()) { // open already -> pls close
		return AEFW_ERR_OPEN_FILE_ALREADY_OPENED;
	}

	if (str.empty()) {
		this->m_sFilename.clear();
		return AEFW_ERR_OPEN_FILE_NAME_EMPTY;
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
		return AEFW_ERR_OPEN_FILE_WRONG_FLAG;
	}

	// last check, to see if everything is okay
	if (this->isClosed()) {
		return AEFW_ERR_OPEN_FILE_ELSE;
	}

	//then everything is good, 
	this->m_cFlags = flags;
	if (this->m_ullFlushInterval != AEFW_DEFAULT_AUTOFLUSH_INTERVAL) {
		this->m_ullFlushInterval = af_interval;
	}
	return AEFW_ERR_NOERROR;
}
