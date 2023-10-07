#include "../include/AEFileWriter.hpp"


AEFileWriter::AEFileWriter(const std::string_view filename, const cint flags, const ullint af_interval) :
	m_ullFlushInterval(af_interval), m_ullTotalWrites(0), m_fpFilestr(nullptr),
	m_cLastError(AEFW_ERR_NOERROR), m_cFlags(flags) {

	this->openFile(filename, flags);
}


AEFileWriter::AEFileWriter(AEFileWriter&& fw) noexcept :
	m_ullFlushInterval(fw.m_ullFlushInterval), m_sFilename(fw.m_sFilename),
	m_ullTotalWrites(fw.m_ullTotalWrites), m_fpFilestr(fw.m_fpFilestr), m_cLastError(fw.m_cLastError), m_cFlags(fw.m_cFlags) {

	fw.m_ullFlushInterval = 0;
	fw.m_ullTotalWrites = 0;
	fw.m_fpFilestr = nullptr;
	fw.m_cLastError = 0;
	fw.m_sFilename.clear();
	fw.m_cFlags = 0;
}

// write data as binary function
// uses const char
cint AEFileWriter::writeData_ptr(const void* cdata, const std::size_t dcount, const std::size_t dsize, const bool useAutoFlush) {

	_AEFW_EXIT_ON_CLOSED_FILE;

	if (!dcount || !dsize || !cdata) {
		return AEFW_ERR_WRITE_ZERO_SIZE;
	}

	const std::size_t writestatus = fwrite(cdata, dsize, dcount, m_fpFilestr);
	this->m_ullTotalWrites++;
	if (useAutoFlush) {
		this->autoFlush();
	}

	if (writestatus != dcount) {
		return AEFW_ERR_WRITE_ERROR;
	}

	return AEFW_ERR_WRITE_SUCCESS;
}


// open file with flags
// uses const char* as all normal string function do
int AEFileWriter::openFile(const std::string_view str, const cint flags, const ullint af_interval) {

	if (this->m_fpFilestr) { // open already -> pls close
		this->closeFile();
	}

	if (!str.empty()) {
		this->m_sFilename.clear();
		this->m_cLastError = AEFW_ERR_FILE_NAME_EMPTY;
		return AEFW_ERR_FILE_NAME_EMPTY;
	}


	this->m_sFilename = str;
	std::size_t found = this->m_sFilename.rfind('/');
	if (found != std::string::npos) {
		std::filesystem::create_directories(this->m_sFilename.substr(0, found));
	}

	switch (flags) {

	case AEFW_FLAG_APPEND://cursor at end, allow changing cursor position
		this->m_fpFilestr = ace::utils::fopenCC(str.data(), "r+b");
		if (!this->m_fpFilestr) {//couldnt open, force create
			this->m_fpFilestr = ace::utils::fopenCC(str.data(), "wb");
			if (!this->m_fpFilestr) {
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
	if (!this->m_fpFilestr) {
		this->m_cLastError = AEFW_ERR_FILE_OPEN_ELSE; //file is still somehow nonexistent
		return AEFW_ERR_FILE_OPEN_ELSE;
	}

	//then everything is good, 
	this->m_cFlags = flags;
	this->m_ullFlushInterval = af_interval;
	return AEFW_ERR_NOERROR;
}

