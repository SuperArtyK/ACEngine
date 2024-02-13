
/*! @file AELogParser/include/AELogParser.hpp
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
#include "include/AELogEntryInfo.hpp"
#include <array>
#include <string_view>
#include <unordered_map>
#include <vector>


//Error flags
/// Macro for the indicator that everything is good/no error was encountered in the process
#define AELP_ERR_NOERROR ENGINE_MODULE_ERR_NOERROR
/// Macro for the error when the passed module name doesn't match the parsed modulenames in the log
/// @see AELogParser::nextEntry()
#define AELP_ERR_INVALID_MODULE_NAME -20
/// Macro for the severity value that includes all entries/severities in parsing
/// @see AELogParser::nextEntry()
#define AELP_SEVERITY_ALL AELOG_TYPE_DEBUG
/// Macro for the "no"/empty modulename to pass for parsing
#define AELP_NO_MODULENAME ""

#define _AELP_CHECK_IF_FILE_OPEN { if (this->isClosed()) { return AEFR_ERR_FILE_NOT_OPEN; } }


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
	/// Class constructor -- just initialises the object
	/// </summary>
	/// <param name=""></param>
	AELogParser(void) :
		m_arrEntryAmount({}), m_ullCurrentEntry(-1)
	{}

	/// <summary>
	/// Class constructor -- opens the file and start the indexing process.
	/// </summary>
	/// <param name="fname">The name of the file to open</param>
	explicit AELogParser(const std::string_view fname) :
		m_arrEntryAmount({}), m_ullCurrentEntry(-1) 
	{
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
	cint openLog(const std::string_view fname);

	/// <summary>
	/// Closes the log file and clears the memory
	/// </summary>
	/// <returns>return value of the AEFileReader::closefile() (AEFR_ERR_NOERROR if file was closed successfully; AEFR_ERR_FILE_NOT_OPEN if file isn't open)</returns>
	inline cint closeLog(void) {
		this->m_vecEntryIndices.clear();
		this->m_vecEntryIndices.shrink_to_fit();

		this->m_vecInvalidEntryIndices.clear();
		this->m_vecInvalidEntryIndices.shrink_to_fit();

		this->m_mapModuleNames.clear();
		
		// the most stupid decision, but it works (whyy tf is using nested types ill-formed??)
		std::memset(this->m_arrEntryAmount.data(), AENULL, this->m_arrEntryAmount.size()* sizeof(this->m_arrEntryAmount[0]));
		this->m_ullCurrentEntry = -1;
		return this->m_frLogReader.closeFile();
	}

	/// <summary>
	/// Read the next *valid* entry in the log file of the given severity and module name filter, and parse it to the given AELogEntry object
	/// @note If the strictSeverity is false, then the severity value just changes the lowest limit of the log severity. Otherwise it sets the exact severity to look for
	/// @note The module name filter is applied after the severity filter.
	/// @note AELOG_TYPE_INVALID works the same as AELOG_TYPE_DEBUG. This function parses only *valid* entries.
	/// </summary>
	/// <param name="entry">The log entry object to parse things into</param>
	/// <param name="severity">The severity of the log entry to look for</param>
	/// <param name="mname">The module name of the log entry to search for</param>
	/// <param name="strictSeverity">The flag to indicate whether the search for severity should be strict (exact)</param>
	/// <returns>AELP_ERR_NOERROR (0) on success, or AEFR_ERR_* (-1 to -8) or AELE_ERR_* (-11 to -15) flags on error; AELP_NO_MODULENAME if invalid module name was passed (-20)</returns>
	cint nextEntry(AELogEntry& entry, const cint severity = AELOG_TYPE_DEBUG, const std::string_view mname = AELP_NO_MODULENAME, const bool strictSeverity = false);

	/// <summary>
	/// Read the next *valid* entry in the log file of the given module name, and parse it into the given AELogEntry object
	/// @see AELogParser::nextEntry()
	/// </summary>
	/// <param name="entry">The log entry object to parse things into</param>
	/// <param name="mname">The module name of the log entry to search for</param>
	/// <returns>AELP_ERR_NOERROR (0) on success, or AEFR_ERR_* (-1 to -8) or AELE_ERR_* (-11 to -15) flags on error; AELP_NO_MODULENAME if invalid module name was passed (-20)</returns>
	inline cint nextEntryName(AELogEntry& entry, const std::string_view mname = AELP_NO_MODULENAME) { return this->nextEntry(entry, AELP_SEVERITY_ALL, mname, false); }

	/// <summary>
	/// Read the next *valid* entry in the log file of the given severity, and parse it to the given AELogEntry object
	/// @note This severity search is non-strict (calls AELogParser::nextEntry() and sets strictSeverity to false)
	/// @see AELogEntry::nextEntry()
	/// </summary>
	/// <param name="entry">The log entry object to parse things into</param>
	/// <param name="severity">The lowest limit of severity of the log entry to look for</param>
	/// <returns>AELP_ERR_NOERROR (0) on success, or AEFR_ERR_* (-1 to -8) or AELE_ERR_* (-11 to -15) flags on error</returns>
	inline cint nextEntryType(AELogEntry& entry, const cint severity) {
		return this->nextEntry(entry, severity, AELP_NO_MODULENAME, false);
	}

	/// <summary>
	/// Read the next *valid* entry in the log file of the given severity, and parse it to the given AELogEntry object
	/// @note This severity search is strict (calls AELogParser::nextEntry() and sets strictSeverity to true)
	/// @see AELogEntry::nextEntry()
	/// </summary>
	/// <param name="entry">The log entry object to parse things into</param>
	/// <param name="severity">The severity of the log entry to look for</param>
	/// <returns>AELP_ERR_NOERROR (0) on success, or AEFR_ERR_* (-1 to -8) or AELE_ERR_* (-11 to -15) flags on error</returns>
	inline cint nextEntryTypeStrict(AELogEntry& entry, const cint severity) {
		return this->nextEntry(entry, severity, AELP_NO_MODULENAME, true);
	}

	/// <summary>
	/// Read next *valud* entry in the log file of "debug" type, and parse it to the given AELogEntry object
	/// @note This severity search is strict (calls AELogParser::nextEntry() and sets strictSeverity to true)
	/// @see AELogEntry::nextEntry()
	/// </summary>
	/// <param name="entry">The log entry object to parse things into</param>
	/// <returns>AELP_ERR_NOERROR (0) on success, or AEFR_ERR_* (-1 to -8) or AELE_ERR_* (-11 to -15) flags on error</returns>
	inline cint nextEntryDebug(AELogEntry& entry) { this->nextEntryTypeStrict(entry, AELOG_TYPE_DEBUG); }

	/// <summary>
	/// Read next *valud* entry in the log file of "info" type, and parse it to the given AELogEntry object
	/// @note This severity search is strict (calls AELogParser::nextEntry() and sets strictSeverity to true)
	/// @see AELogEntry::nextEntry()
	/// </summary>
	/// <param name="entry">The log entry object to parse things into</param>
	/// <returns>AELP_ERR_NOERROR (0) on success, or AEFR_ERR_* (-1 to -8) or AELE_ERR_* (-11 to -15) flags on error</returns>
	inline cint nextEntryInfo(AELogEntry& entry) { this->nextEntryTypeStrict(entry, AELOG_TYPE_INFO); }

	/// <summary>
	/// Read next *valud* entry in the log file of "warning" type, and parse it to the given AELogEntry object
	/// @note This severity search is strict (calls AELogParser::nextEntry() and sets strictSeverity to true)
	/// @see AELogEntry::nextEntry()
	/// </summary>
	/// <param name="entry">The log entry object to parse things into</param>
	/// <returns>AELP_ERR_NOERROR (0) on success, or AEFR_ERR_* (-1 to -8) or AELE_ERR_* (-11 to -15) flags on error</returns>
	inline cint nextEntryWarn(AELogEntry& entry) { this->nextEntryTypeStrict(entry, AELOG_TYPE_WARN); }

	/// <summary>
	/// Read next *valud* entry in the log file of "severe warning" type, and parse it to the given AELogEntry object
	/// @note This severity search is strict (calls AELogParser::nextEntry() and sets strictSeverity to true)
	/// @see AELogEntry::nextEntry()
	/// </summary>
	/// <param name="entry">The log entry object to parse things into</param>
	/// <returns>AELP_ERR_NOERROR (0) on success, or AEFR_ERR_* (-1 to -8) or AELE_ERR_* (-11 to -15) flags on error</returns>
	inline cint nextEntrySevereWarn(AELogEntry& entry) { this->nextEntryTypeStrict(entry, AELOG_TYPE_SEVERE_WARN); }

	/// <summary>
	/// Read next *valud* entry in the log file of "OK" type, and parse it to the given AELogEntry object
	/// @note This severity search is strict (calls AELogParser::nextEntry() and sets strictSeverity to true)
	/// @see AELogEntry::nextEntry()
	/// </summary>
	/// <param name="entry">The log entry object to parse things into</param>
	/// <returns>AELP_ERR_NOERROR (0) on success, or AEFR_ERR_* (-1 to -8) or AELE_ERR_* (-11 to -15) flags on error</returns>
	inline cint nextEntryOK(AELogEntry& entry) { this->nextEntryTypeStrict(entry, AELOG_TYPE_OK); }

	/// <summary>
	/// Read next *valud* entry in the log file of "success" type, and parse it to the given AELogEntry object
	/// @note This severity search is strict (calls AELogParser::nextEntry() and sets strictSeverity to true)
	/// @see AELogEntry::nextEntry()
	/// </summary>
	/// <param name="entry">The log entry object to parse things into</param>
	/// <returns>AELP_ERR_NOERROR (0) on success, or AEFR_ERR_* (-1 to -8) or AELE_ERR_* (-11 to -15) flags on error</returns>
	inline cint nextEntrySuccess(AELogEntry& entry) { this->nextEntryTypeStrict(entry, AELOG_TYPE_SUCCESS); }

	/// <summary>
	/// Read next *valud* entry in the log file of "error" type, and parse it to the given AELogEntry object
	/// @note This severity search is strict (calls AELogParser::nextEntry() and sets strictSeverity to true)
	/// @see AELogEntry::nextEntry()
	/// </summary>
	/// <param name="entry">The log entry object to parse things into</param>
	/// <returns>AELP_ERR_NOERROR (0) on success, or AEFR_ERR_* (-1 to -8) or AELE_ERR_* (-11 to -15) flags on error</returns>
	inline cint nextEntryError(AELogEntry& entry) { this->nextEntryTypeStrict(entry, AELOG_TYPE_ERROR); }

	/// <summary>
	/// Read next *valud* entry in the log file of "fatal error" type, and parse it to the given AELogEntry object
	/// @note This severity search is strict (calls AELogParser::nextEntry() and sets strictSeverity to true)
	/// @see AELogEntry::nextEntry()
	/// </summary>
	/// <param name="entry">The log entry object to parse things into</param>
	/// <returns>AELP_ERR_NOERROR (0) on success, or AEFR_ERR_* (-1 to -8) or AELE_ERR_* (-11 to -15) flags on error</returns>
	inline cint nextEntryFatalError(AELogEntry& entry) { this->nextEntryTypeStrict(entry, AELOG_TYPE_FATAL_ERROR); }


	/// <summary>
	/// Read the next indexed *valid* log entry in the file of the given severity filter and return it's file cursors in the file
	/// @note The severity value just changes the lowest limit of the log severity (lowest by default is debug). If a higher severity is encountered, it's read as well.
	/// @note AELOG_TYPE_INVALID works the same as AELOG_TYPE_DEBUG. This function parses only *valid* entries.
	/// </summary>
	/// <param name="severity">The lowest severity of the log to find</param>
	/// <returns>The file cursors of the next valid entry (in the currently-opened log file); AEFR_ERR_FILE_NOT_OPEN if the file isn't open</returns>
	inline llint nextEntryCursor(const cint severity = AELOG_TYPE_DEBUG) {
		_AELP_CHECK_IF_FILE_OPEN;

		//cycle the current entry number untill we find an entry with proper severity level
		while (1) {
			if (++this->m_ullCurrentEntry >= this->m_vecEntryIndices.size()) {
				return AEFR_ERR_READ_EOF;
			}
			if (this->m_vecEntryIndices[this->m_ullCurrentEntry].logType >= severity) {
				break;
			}
		}
		return this->m_vecEntryIndices[this->m_ullCurrentEntry].cursorIndex;
	}

	/// <summary>
	/// Read the next indexed valid log entry in the opened file of type "debug", and return the file cursor to it.
	/// @see AELogParser::nextEntryCursor()
	/// </summary>
	/// <returns>The file cursors of the next valid debug entry (in the currently-opened log file); AEFR_ERR_FILE_NOT_OPEN if the file isn't open</returns>
	inline llint nextEntryCursorDebug(void) { _AELP_CHECK_IF_FILE_OPEN; this->nextEntryCursor(AELOG_TYPE_DEBUG); }

	/// <summary>
	/// Read the next indexed valid log entry in the opened file of type "info", and return the file cursor to it.
	/// @see AELogParser::nextEntryCursor()
	/// </summary>
	/// <returns>The file cursors of the next valid info entry (in the currently-opened log file); AEFR_ERR_FILE_NOT_OPEN if the file isn't open</returns>
	inline llint nextEntryCursorInfo(void) { _AELP_CHECK_IF_FILE_OPEN; this->nextEntryCursor(AELOG_TYPE_INFO); }

	/// <summary>
	/// Read the next indexed valid log entry in the opened file of type "warning", and return the file cursor to it.
	/// @see AELogParser::nextEntryCursor()
	/// </summary>
	/// <returns>The file cursors of the next valid warning entry (in the currently-opened log file); AEFR_ERR_FILE_NOT_OPEN if the file isn't open</returns>
	inline llint nextEntryCursorWarn(void) { _AELP_CHECK_IF_FILE_OPEN; this->nextEntryCursor(AELOG_TYPE_WARN); }

	/// <summary>
	/// Read the next indexed valid log entry in the opened file of type "severe warning", and return the file cursor to it.
	/// @see AELogParser::nextEntryCursor()
	/// </summary>
	/// <returns>The file cursors of the next valid severe warning entry (in the currently-opened log file); AEFR_ERR_FILE_NOT_OPEN if the file isn't open</returns>
	inline llint nextEntryCursorSevereWarn(void) { _AELP_CHECK_IF_FILE_OPEN; this->nextEntryCursor(AELOG_TYPE_SEVERE_WARN); }

	/// <summary>
	/// Read the next indexed valid log entry in the opened file of type "OK", and return the file cursor to it.
	/// @see AELogParser::nextEntryCursor()
	/// </summary>
	/// <returns>The file cursors of the next valid OK entry (in the currently-opened log file); AEFR_ERR_FILE_NOT_OPEN if the file isn't open</returns>
	inline llint nextEntryCursorOK(void) { _AELP_CHECK_IF_FILE_OPEN; this->nextEntryCursor(AELOG_TYPE_OK); }

	/// <summary>
	/// Read the next indexed valid log entry in the opened file of type "success", and return the file cursor to it.
	/// @see AELogParser::nextEntryCursor()
	/// </summary>
	/// <returns>The file cursors of the next valid success entry (in the currently-opened log file); AEFR_ERR_FILE_NOT_OPEN if the file isn't open</returns>
	inline llint nextEntryCursorSuccess(void) { _AELP_CHECK_IF_FILE_OPEN; this->nextEntryCursor(AELOG_TYPE_SUCCESS); }

	/// <summary>
	/// Read the next indexed valid log entry in the opened file of type "rror", and return the file cursor to it.
	/// @see AELogParser::nextEntryCursor()
	/// </summary>
	/// <returns>The file cursors of the next valid rror entry (in the currently-opened log file); AEFR_ERR_FILE_NOT_OPEN if the file isn't open</returns>
	inline llint nextEntryCursorError(void) { _AELP_CHECK_IF_FILE_OPEN; this->nextEntryCursor(AELOG_TYPE_ERROR); }

	/// <summary>
	/// Read the next indexed valid log entry in the opened file of type "fatal error", and return the file cursor to it.
	/// @see AELogParser::nextEntryCursor()
	/// </summary>
	/// <returns>The file cursors of the next valid fatal error entry (in the currently-opened log file); AEFR_ERR_FILE_NOT_OPEN if the file isn't open</returns>
	inline llint nextEntryCursorFatalError(void) { _AELP_CHECK_IF_FILE_OPEN; this->nextEntryCursor(AELOG_TYPE_FATAL_ERROR); }

	/// <summary>
	/// Get the file cursors of the current valid entry
	/// This entry is the one that was read by a previous call to AELogParser::nextEntry() or AELogParser::getNextEntryIndex()
	/// </summary>
	/// <returns>The file cursors of the current valid entry (in the currently-opened log file); AEFR_ERR_FILE_NOT_OPEN if the file isn't open</returns>
	inline llint currentEntryCursor(void) const {
		_AELP_CHECK_IF_FILE_OPEN;
		return this->m_vecEntryIndices[this->m_ullCurrentEntry].cursorIndex;
	}

	inline std::size_t getCurrentEntryIndex(void) const {
		return this->m_ullCurrentEntry.load();
	}

	inline cint setCurrentEntryIndex(const std::size_t entryIndex) {
		_AELP_CHECK_IF_FILE_OPEN;
		if (entryIndex >= this->m_vecEntryIndices.size()) {
			return AEFR_ERR_READ_EOF;
		}
		this->m_ullCurrentEntry = entryIndex;
	}

	/// <summary>
	/// Get the list of file cursors of the valid entries, separated by type.
	/// @note If the file is not open, the returned vector is empty
	/// </summary>
	/// <returns>(by value) The vector of AELogEntryInfo, each having the (cursor) index, index of the module name, and type of each valid entry</returns>
	inline std::vector<AELogEntryInfo> getValidEntryCursorAll(void) const noexcept {
		return this->m_vecEntryIndices;
	}

	/// <summary>
	/// Get the list of file cursors of the invalid entries.
	/// In the return vector, llint is the index of the cursor in the file
	/// @note If the file is not open, the returned vector is empty
	/// </summary>
	/// <returns>(by value) The vector of llint's, having the cursor indices for each invalid entry</returns>
	inline std::vector<llint> getInvalidEntryCursorAll(void) const noexcept {
		return this->m_vecInvalidEntryIndices;
	}

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
	/// @see AEFileReader::isOpen()
	/// </summary>
	/// <returns>true if file is opened, false otherwise</returns>
	inline bool isOpen(void) const noexcept {
		return this->m_frLogReader.isOpen();
	}

	/// <summary>
	/// Checks if the file is closed by the AELogParser
	/// @see AEFileReader::isClosed()
	/// </summary>
	/// <returns>true is file is closed (no file opened), false otherwise</returns>
	inline bool isClosed(void) const noexcept {
		return this->m_frLogReader.isClosed();
	}

	/// <summary>
	/// Returns the full name of currently open file, including the relative path, if such was provided.
	/// @see AEFileReader::getFullFileName()
	/// </summary>
	/// <returns>std::string of the opened file (including relative path if was given); empty string otherwise</returns>
	inline std::string getFullFileName(void) const noexcept {
		return this->m_frLogReader.getFullFileName();
	}

	/// <summary>
	/// Returns the relative path of the opened file.
	/// @see AEFileReader::getRelativePath()
	/// </summary>
	/// <returns>std::string of the relative file path of opened file; empty string otherwise</returns>
	inline std::string getRelativePath(void) const {
		return this->m_frLogReader.getRelativePath();
	}

	/// <summary>
	/// Returns the name of the opened file, devoid of the path
	/// @see AEFileReader::getFileName()
	/// </summary>
	/// <returns>std::string of the opened file name; emtpy string otherwise</returns>
	inline std::string getFileName(void) const {
		return this->m_frLogReader.getFileName();
	}

	/// <summary>
	/// Returns the full, absolute path of the opened file
	/// @see AEFileReader::getFullPath()
	/// </summary>
	/// <returns>std::string of the absolute path of the opened file; empty string otherwise</returns>
	inline std::string getFullPath(void) const {
		return this->m_frLogReader.getFullPath();
	}

	/// <summary>
	/// Return the reference to the AEFileReader object of this instance of AELogParser
	/// @warning Be carefull with it
	/// </summary>
	/// <returns>the reference to the AEFileReader object</returns>
	inline AEFileReader& getFileReader(void) noexcept {
		return this->m_frLogReader;
	}


private:

	/// The file reader of the opened log file.
	AEFileReader m_frLogReader;
	/// The list of all indexed *valid* entries in the log file.
	/// Each item contains their corresponding cursor position in the file and their type/severity.
	std::vector<AELogEntryInfo> m_vecEntryIndices;
	/// The list of all indexed *invalid* entries in the log file
	/// Each item contains their corresponding cursor position in the file.
	std::vector<llint> m_vecInvalidEntryIndices;
	/// The map of the all module names parsed in the log file
	std::unordered_map<std::string, short> m_mapModuleNames;
	/// The amount of log entries read in the file, separated by type/severity.
	std::array<ullint, 9> m_arrEntryAmount;
	/// The number corresponding to the currently-read *valid* entry in the log file.
	/// The maximum value corresponds to the size of m_vecEntryIndices
	std::atomic<std::size_t> m_ullCurrentEntry;

	//aaaand register it
	REGISTER_MODULE(AELogParser)
};




#endif // !ENGINE_LOG_PARSER_HPP
