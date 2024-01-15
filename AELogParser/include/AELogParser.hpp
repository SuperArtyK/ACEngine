
/** @file AELogParser/include/AELogParser.hpp
 *  This file contains the engine's log parser module.
 *  It parses the log files that AELogger writes and turns them back to AELogEntry objects
 *
 *  Should not cause everything to break :)
 */

#pragma once

#ifndef ENGINE_LOG_PARSER_HPP
#define ENGINE_LOG_PARSER_HPP

#include "include/AEFileReader.hpp"
#include "include/AELogEntry.hpp"
#include <array>
#include <string_view>
#include <vector>


//Error flags
/// Macro for the indicator that everything is good/no error was encountered in the process
#define AELP_ERR_NOERROR ENGINE_MODULE_ERR_NOERROR

/// <summary>
/// ArtyK's Engine's Log Parser; it parses the log files that AELogger writes.
/// Wrapper around AELogEntry for parsing and AEFileWriter for reading functionality, (ab)using them both.
/// 
/// When opening a file, it reads it and indexes it for log information,
/// and after the parsing/reading the log file to memory can be requested (one entry at a time/the whole thing).
/// Such info would be the amount of entries, amount of entries of each type, etc.
/// Also it allows to filter the log by severity and read only important (to the use-case) data.
/// 
/// Hungarian notation is lp. (m_lpMyLogParser)
/// </summary>
class AELogParser : public __AEModuleBase<AELogParser> {

public:
//Constructor
	/// <summary>
	/// Class constructor -- opens the file and start the indexing process.
	/// </summary>
	/// <param name="fname">The name of the file to open</param>
	explicit AELogParser(const std::string_view fname) : 
		m_vecEntryIndices({}), m_vecInvalidEntryIndices({}), m_arrEntryAmount({}), m_ullCurrentEntry(0) {
		m_vecEntryIndices.reserve(AELOG_DEFAULT_QUEUE_SIZE * 10);
		m_vecInvalidEntryIndices.reserve(AELOG_DEFAULT_QUEUE_SIZE);
		this->openLog(fname);
		
	}

	/// <summary>
	/// Class destructor -- closes the log file
	/// </summary>
	~AELogParser(void) {
		this->closeLog();
	}

	/// <summary>
	/// Opens the log file, reading and parsing every single entry, classifying it by type.
	/// </summary>
	/// <param name="fname">The name of the file to open</param>
	/// <returns>AELP_ERR_NOERROR (0) on success, or AEFR_ERR_* (-1 to -8) or AELE_ERR_* (-11 to -15) flags on error</returns>
	inline cint openLog(const std::string_view fname) {
		if (this->isOpen()) {
			return AEFR_ERR_OPEN_FILE_ALREADY_OPENED;
		}
		const cint ret = this->m_frLogReader.openFile(fname);
		if (ret != AELP_ERR_NOERROR) {
			return ret;
		}
		
		

		// and index/parse log
		char fileline[AELE_FORMAT_MAX_SIZE + 1]{}; //+1 more character to determine the validity)
		cint fileRead = AEFR_ERR_NOERROR;
		cint parsedEntry = AELP_ERR_NOERROR;
		ullint cursor = 0;
		AELogEntry dummyentry{};

		while (1) {
			cursor = this->m_frLogReader.getCursorPos();
			fileRead = this->m_frLogReader.readStringNL(fileline, sizeof(fileline) - 1);

			if (fileline[0] == '\n') { //first character is an empty line -- skipping...
				break;
			}
			parsedEntry = AELogEntry::parseStringEntry(dummyentry, fileline, AELE_PARSE_STRING_WRITE_TYPE);

			if (parsedEntry != AELP_ERR_NOERROR) {
				if (fileRead != AEFR_ERR_NOERROR) {
					break;
				}
				this->m_vecInvalidEntryIndices.push_back(cursor);
				this->m_arrEntryAmount[0]++;
			}
			else {
				this->m_vecEntryIndices.emplace_back(cursor, dummyentry.m_cLogType);
				this->m_arrEntryAmount[dummyentry.m_cLogType + 1]++;
			}
		}



		return ret;
	}

	/// <summary>
	/// Closes the log file and clears the memory
	/// </summary>
	/// <returns>return value of the AEFileReader::closefile() (AEFR_ERR_NOERROR if file was closed successfully; AEFR_ERR_FILE_NOT_OPEN if file isn't open)</returns>
	inline cint closeLog(void) {
		m_vecEntryIndices.clear();
		m_vecInvalidEntryIndices.clear();
		std::memset(this->m_arrEntryAmount.data(), NULL, sizeof(this->m_arrEntryAmount.data()));
		m_ullCurrentEntry = 0;
		return this->m_frLogReader.closeFile();
	}
	
	/// <summary>
	/// Read the next entry in the log file of the given severity filter, and parse it to the given AELogEntry object
	/// @note The severity value just changes the lowest limit of the log severity (lowest by default is debug). If a higher severity is encountered, it's read as well.
	/// </summary>
	/// <param name="entry">The log entry object to parse things into</param>
	/// <param name="severity">The lowest severity of the log to find</param>
	/// <returns>AELP_ERR_NOERROR (0) on success, or AEFR_ERR_* (-1 to -8) or AELE_ERR_* (-11 to -15) flags on error</returns>
	cint nextEntry(AELogEntry& entry, const cint severity = AELOG_TYPE_DEBUG);
	
	/// <summary>
	/// Same as the AELogParser::nextEntry() but on massive scale -- scans the whole log file and parses it to the freshly-allocated queue.
	/// @warning If this queue isn't deallocated (deleted) before dropping the queue pointer, this **WILL** lead to memory leaks!
	/// </summary>
	/// <param name="begin">The pointer to which the queue will be allocated</param>
	/// <param name="severity">The lowest severity of the log to find</param>
	/// <returns>AELP_ERR_NOERROR (0) on success, or AEFR_ERR_* (-1 to -8) or AELE_ERR_* (-11 to -15) flags on error</returns>
	cint logToQueue(AELogEntry*& begin, const cint severity = AELOG_TYPE_DEBUG);
	
	/// <summary>
	/// Get the amount of valid entries in the log (with optional lowest severity setting).
	/// Valid entries are entries that are not AELOG_TYPE_INVALID
	/// </summary>
	/// <param name="severity">The lowest severity of the log to find</param>
	/// <returns>ULLINT_MAX if the severity is outside of the AELOG_TYPE_* range; ullint amount of entries</returns>
	inline ullint amountValidEntries(const cint severity = AELOG_TYPE_DEBUG) const noexcept {
		if (!ace::utils::isInRange<cint>(AELOG_TYPE_INVALID, AELOG_TYPE_FATAL_ERROR, severity)) {
			return ULLINT_MAX;
		}
		ullint tempres = 0;
		for (int i = severity; i < 8; i++) {
			tempres += this->m_arrEntryAmount[i + 1];
		}
		return tempres;
	}

	/// <summary>
	/// Get the amount of invalid entries in the log. Invalid entries are of type AELOG_TYPE_INVALID
	/// </summary>
	/// <returns>ullint amount of invalid entries</returns>
	inline ullint amountInvalidEntries(void) const noexcept {
		return m_arrEntryAmount[0]; // shortened version of AELOG_TYPE_INVALID (which is -1) + 1
	}

	/// <summary>
	/// Get the amount of entries of a certain type/severity
	/// </summary>
	/// <param name="severity">The severity/type of the log to find</param>
	/// <returns>ULLINT_MAX if the severity is outside of the AELOG_TYPE_* range; ullint amount of entries</returns>
	inline ullint amountTypeEntries(const cint severity) const noexcept {
		if (!ace::utils::isInRange<cint>(AELOG_TYPE_INVALID, AELOG_TYPE_FATAL_ERROR, severity)) {
			return ULLINT_MAX;
		}
		return this->m_arrEntryAmount[severity + 1];
	}

	/// <summary>
	/// Checks if the file is opened by the AELogParser
	/// </summary>
	/// <returns>true if file is opened, false otherwise</returns>
	inline bool isOpen(void) const noexcept {
		return this->m_frLogReader.isOpen();
	}

	/// <summary>
	/// Checks if the file is closed by the AELogParser
	/// </summary>
	/// <returns>true is file is closed (no file opened), false otherwise</returns>
	inline bool isClosed(void) const noexcept {
		return this->m_frLogReader.isClosed();
	}


private:

	/// The file reader of the opened log file.
	AEFileReader m_frLogReader;
	/// The list of all indexed *valid* entries in the log file.
	/// Each item contains their corresponding cursor position in the file and their type/severity.
	std::vector<std::pair<ullint, cint>> m_vecEntryIndices;
	/// The list of all indexed *invalid* entries in the log file
	/// Each item contains their corresponding cursor position in the file.
	std::vector<ullint> m_vecInvalidEntryIndices;
	/// The amount of log entries read in the file, separated by type/severity.
	std::array<ullint, 9> m_arrEntryAmount;
	/// The number corresponding to the currently-read *valid* entry in the log file.
	/// The maximum value corresponds to the size of m_vecEntryIndices
	std::atomic<ullint> m_ullCurrentEntry;

};

//aaaand register it
REGISTER_MODULE(AELogParser)


#endif // !ENGINE_LOG_PARSER_HPP
