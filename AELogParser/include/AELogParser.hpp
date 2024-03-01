
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
#include "include/AEMath.hpp"
#include <array>
#include <string_view>
#include <unordered_map>
#include <vector>


//Error flags
/// Macro for the indicator that everything is good/no error was encountered in the process
#define AELP_ERR_NOERROR ENGINE_MODULE_ERR_NOERROR
/// Macro for the error when the passed module name doesn't match the parsed modulenames in the log
/// @see AELogParser::findNextEntry()
#define AELP_ERR_INVALID_MODULE_NAME -20
/// Macro for the error when the passed severity is invalid in the current context
/// Ex. non-existent severity, or the severity wasn't found in the log file
#define AELP_ERR_INVALID_SEVERITY -21
/// Macro for the error when the passed severity and modulename filter didn't produce a result when applied
/// Ex. both the severity and the module name exist in the parsed log file, but never together in the same entry
/// @see AELogParser::findNextEntry()
#define AELP_ERR_FILTER_NO_MATCHES -22
/// Macro for the error when the passed severity and module name filter is invalid (e.g. empty)
#define AELP_ERR_INVALID_FILTER -23
/// Macro for the error when the null queue pointer was passed into the functions
#define AELP_ERR_INVALID_QUEUE -24
/// Macro for the severity value that includes all entries/severities in parsing
/// Essentially disables severity filtering
/// @note It disables (makes useless) the "strict severity" in the functions that set it
/// @see AELogParser::findNextEntry()
#define AELP_SEVERITY_ALL AELOG_TYPE_INVALID
/// Macro for the "no"/empty modulename to pass for parsing
#define AELP_NO_MODULENAME ""

/// Helper macro to check if the file is closed, and if so return AEFR_ERR_FILE_NOT_OPEN
/// @note Wouldn't advise to use -- it just helped shorten the code
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
/// Flags start with AELP_
/// </summary>
class AELogParser : public __AEModuleBase<AELogParser> {

public:
//Constructor
	/// <summary>
	/// Class constructor -- just initialises the object
	/// </summary>
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
	/// <param name="strictSeverity">The flag to indicate whether the search for severity should be strict (exact)</param>
	/// <param name="mname">The module name of the log entry to search for</param>
	/// <returns>AELP_ERR_NOERROR (0) on success, or AEFR_ERR_* (-1 to -8) or AELE_ERR_* (-11 to -15) flags on error; error codes from AELogParser::errorFromAELEI()</returns>
	/// @see AELogParser::errorFromAELEI()
	cint nextEntry(AELogEntry& entry, const cint severity = AELOG_TYPE_DEBUG, const bool strictSeverity = false, const std::string_view mname = AELP_NO_MODULENAME);

	/// <summary>
	/// Read the next *valid* entry in the log file of the given module name, and parse it into the given AELogEntry object
	/// @see AELogParser::nextEntry()
	/// </summary>
	/// <param name="entry">The log entry object to parse things into</param>
	/// <param name="mname">The module name of the log entry to search for</param>
	/// <returns>AELP_ERR_NOERROR (0) on success, or AEFR_ERR_* (-1 to -8) or AELE_ERR_* (-11 to -15) flags on error; error codes from AELogParser::errorFromAELEI()</returns>
	/// @see AELogParser::errorFromAELEI()
	inline cint nextEntryName(AELogEntry& entry, const std::string_view mname) { return this->nextEntry(entry, AELP_SEVERITY_ALL, false, mname); }

	/// <summary>
	/// Read the next *valid* entry in the log file of the given severity, and parse it to the given AELogEntry object
	/// @see AELogEntry::nextEntry()
	/// </summary>
	/// <param name="entry">The log entry object to parse things into</param>
	/// <param name="severity">The lowest limit of severity of the log entry to look for</param>
	/// <param name="strictSeverity">The flag to indicate whether the search for severity should be strict (exact)</param>
	/// <returns>AELP_ERR_NOERROR (0) on success, or AEFR_ERR_* (-1 to -8) or AELE_ERR_* (-11 to -15) flags on error; error codes from AELogParser::errorFromAELEI()</returns>
	/// @see AELogParser::errorFromAELEI()
	inline cint nextEntryType(AELogEntry& entry, const cint severity, const bool strictSeverity = false) {
		return this->nextEntry(entry, severity, strictSeverity, AELP_NO_MODULENAME);
	}

	/// <summary>
	/// Read the next indexed *valid* log entry in the file of the given severity and module name filter, and return it's file cursors in the file
	/// @note The severity value just changes the lowest limit of the log severity (lowest by default is debug). If a higher severity is encountered, it's read as well.
	/// @note AELOG_TYPE_INVALID works the same as AELOG_TYPE_DEBUG. This function parses only *valid* entries.
	/// </summary>
	/// <param name="severity">The lowest severity of the log to find</param>
	/// <param name="strictSeverity">The flag to indicate whether the search for severity should be strict (exact)</param>
	/// <param name="mname">The module name of the log entry to search for</param>
	/// <returns>The file cursors of the next valid entry (in the currently-opened log file); error codes from AELogParser::errorFromAELEI()</returns>
	/// @see AELogParser::errorFromAELEI()
	inline llint nextEntryCursor(const cint severity = AELOG_TYPE_DEBUG, const bool strictSeverity = false, const std::string_view mname = AELP_NO_MODULENAME) {

		const AELogEntryInfo leInfo = this->findNextEntry(severity, mname, strictSeverity);
		const cint ret = AELogParser::errorFromAELEI(leInfo);
		if (ret != AELP_ERR_NOERROR) {
			return ret; // return the error code from the AELEI
		}

		return leInfo.cursorIndex;
	}

	/// <summary>
	/// Read the next indexed *valid* log entry in the file of the given module name filter, and return it's file cursors in the file
	/// @see AELogParser::nextEntryCursor()
	/// </summary>
	/// <param name="mname">The module name of the log entry to search for</param>
	/// <returns>The file cursors of the next valid entry (in the currently-opened log file); error codes from AELogParser::errorFromAELEI()</returns>
	/// @see AELogParser::errorFromAELEI()
	inline llint nextEntryCursorName(const std::string_view mname) { return this->nextEntryCursor(AELP_SEVERITY_ALL, false, mname); }

	/// <summary>
	/// Read the next indexed *valid* log entry in the file of the given severity filter and return it's file cursors in the file
	/// @note AELOG_TYPE_INVALID works the same as AELOG_TYPE_DEBUG. This function parses only *valid* entries.
	/// @see AELogParser::nextEntryCursor()
	/// </summary>
	/// <param name="severity">The lowest severity of the log to find</param>
	/// <param name="strictSeverity">The flag to indicate whether the search for severity should be strict (exact)</param>
	/// <returns>The file cursors of the next valid entry (in the currently-opened log file); error codes from AELogParser::errorFromAELEI()</returns>
	/// @see AELogParser::errorFromAELEI()
	inline llint nextEntryCursorType(const cint severity, const bool strictSeverity = false) { return this->nextEntryCursor(severity, strictSeverity, AELP_NO_MODULENAME); }

	/// <summary>
	/// Get the file cursors of the current valid entry
	/// This entry is the one that was read by a previous call to AELogParser::nextEntry() or AELogParser::getNextEntryIndex()
	/// </summary>
	/// <returns>The file cursors of the current valid entry (in the currently-opened log file); AEFR_ERR_FILE_NOT_OPEN if the file isn't open; AEFR_ERR_READ_EOF if the entry number is invalid (past the end of the log file)</returns>
	inline llint currentEntryCursor(void) const noexcept {
		_AELP_CHECK_IF_FILE_OPEN;
		if (this->m_ullCurrentEntry >= this->m_vecEntryIndices.size()) {
			return AEFR_ERR_READ_EOF;
		}
		return this->m_vecEntryIndices[this->m_ullCurrentEntry].cursorIndex;
	}

	/// <summary>
	/// Return the order number (index) of the current log entry (that was attempted to be read)
	/// </summary>
	/// <returns>The index of the current entry as std::size_t</returns>
	inline std::size_t getCurrentEntryIndex(void) const noexcept { return this->m_ullCurrentEntry.load(); }

	/// <summary>
	/// Set the order number (index) of the next log entry to be parsed
	/// </summary>
	/// <param name="entryIndex">The order number of the next entry</param>
	/// <returns>AELP_ERR_NOERROR on success; AEFR_ERR_READ_EOF if the value was larger than the (amount of valid parsed entries - 1); AEFR_ERR_FILE_NOT_OPEN if file wasn't open</returns>
	inline cint setNextEntryIndex(const std::size_t entryIndex) noexcept {
		_AELP_CHECK_IF_FILE_OPEN;
		if (entryIndex >= this->m_vecEntryIndices.size()) {
			return AEFR_ERR_READ_EOF;
		}
		this->m_ullCurrentEntry = entryIndex;
		return AELP_ERR_NOERROR; 
	}

	/// <summary>
	/// Get the list of file cursors of the valid entries, separated by type.
	/// @note If the file is not open, the returned vector is empty
	/// </summary>
	/// <returns>(by value) The vector of AELogEntryInfo, each having the (cursor) index, index of the module name, and type of each valid entry</returns>
	inline std::vector<AELogEntryInfo> getEntryCursorAll(void) const noexcept {
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
	/// @see AELogEntry::makeQueue()
	/// The parsed queue is filtered with the given severity level and "strict severity search" flag
	/// @warning If this queue isn't deallocated (deleted) before dropping the queue pointer, this **WILL** lead to memory leaks!
	/// </summary>
	/// <param name="begin">The pointer to which the queue will be allocated</param>
	/// <param name="severity">The lowest severity of the log to find</param>
	/// <param name="strictSeverity">The flag to indicate whether the search for severity should be strict (exact)</param>
	/// <param name="mname">The module name of the log entry to search for</param>
	/// <returns>AELP_ERR_NOERROR (0) on success; or AEFR_ERR_* (-1 to -8) or AELE_ERR_* (-11 to -15), AELP_ERR_*(-20 to -24) flags on error </returns>
	cint logToQueue(AELogEntry*& begin, const cint severity = AELOG_TYPE_DEBUG, const bool strictSeverity = false, const std::string_view mname = AELP_NO_MODULENAME);

	/// <summary>
	/// A shortcut to AELogParser::logToQueue() to filter only by type/severity of the log entries
	/// @see AELogParser::logToQueue()
	/// @warning If this queue isn't deallocated (deleted) before dropping the queue pointer, this **WILL** lead to memory leaks!
	/// </summary>
	/// <param name="begin">The pointer to which the queue will be allocated</param>
	/// <param name="severity">The lowest severity of the log to find</param>
	/// <param name="strictSeverity">The flag to indicate whether the search for severity should be strict (exact)</param>
	/// <returns>AELP_ERR_NOERROR (0) on success; or AEFR_ERR_* (-1 to -8) or AELE_ERR_* (-11 to -15), AELP_ERR_*(-20 to -24) flags on error </returns>
	inline cint logToQueueType(AELogEntry*& begin, const cint severity, const bool strictSeverity = false) {
		return this->logToQueue(begin, severity, strictSeverity, AELP_NO_MODULENAME);
	}

	/// <summary>
	/// A shortcut to AELogParser::logToQueue() to filter only by the module name of the log entries
	/// @see AELogParser::logToQueue()
	/// @warning If this queue isn't deallocated (deleted) before dropping the queue pointer, this **WILL** lead to memory leaks!
	/// </summary>
	/// <param name="begin">The pointer to which the queue will be allocated</param>
	/// <param name="mname">The module name of the log entry to search for</param>
	/// <returns>AELP_ERR_NOERROR (0) on success; or AEFR_ERR_* (-1 to -8) or AELE_ERR_* (-11 to -15), AELP_ERR_*(-20 to -24) flags on error </returns>
	inline cint logToQueueName(AELogEntry*& begin, const std::string_view mname) {
		return this->logToQueue(begin, AELP_SEVERITY_ALL, false, mname);
	}

	/// <summary>
	/// Filters the passed queue according to the given severity and modulename filters.
	/// @warning Make sure the queue is not circled (the "next node" pointer of the last entry in the queue is nullptr). Otherwise this will result in an infinite loop in the function
	/// @todo Implement a container for the queue to prevent this from happening
	/// </summary>
	/// <param name="ptr">The pointer to the existing queue</param>
	/// <param name="severity">The lowest severity of the log to filter for</param>
	/// <param name="strictSeverity">The flag to indicate whether the filtering for severity should be strict (exact)</param>
	/// <param name="mname">The module name of the log entry to filter for</param>
	/// <param name="reallocQueue">Flag to reallocate the filtered queue (reduces the final memory usage)</param>
	/// @warning Use reallocQueue flag only if the queue was allocated on the heap (like with AELogParser::logToQueue(). Otherwise (if it's stack-allocated) it will try to delete[] the stack memory
	/// <returns>AELP_ERR_NOERROR (0) on success; On invalid arguments: AELP_ERR_INVALID_QUEUE if ptr is nullptr, AELP_ERR_INVALID_FILTER if mname is empty and severity is AELP_SEVERITY_ALL; On filtering results: AELP_ERR_INVALID_SEVERITY if severity wasn't found in the queue (severity was the only filter), AELP_ERR_INVALID_MODULE_NAME if module name wasn't found in the queue(module name was the only filter), AELP_ERR_FILTER_NO_MATCHES if nothing was found with both passed and valid severity and module name filters</returns>
	static cint filterQueue(AELogEntry*& ptr, const cint severity, const bool strictSeverity, const std::string_view mname, const bool reallocQueue = true);

	/// <summary>
	/// Filters the passed queue according to the given severity
	/// @warning Make sure the queue is not circled (the "next node" pointer of the last entry in the queue is nullptr). Otherwise this will result in an infinite loop in the function
	/// @todo Implement a container for the queue to prevent this from happening
	/// </summary>
	/// <param name="ptr">The pointer to the existing queue</param>
	/// <param name="severity">The lowest severity of the log to filter for</param>
	/// <param name="strictSeverity">The flag to indicate whether the filtering for severity should be strict (exact)</param>
	/// <param name="reallocQueue">Flag to reallocate the filtered queue (reduces the final memory usage)</param>
	/// @warning Use reallocQueue flag only if the queue was allocated on the heap (like with AELogParser::logToQueue(). Otherwise (if it's stack-allocated) it will try to delete[] the stack memory
	/// <returns>Same values as AELogParser::filterQueue() but only relating to the severity filter</returns>
	static inline cint filterQueueType(AELogEntry*& ptr, const cint severity, const bool strictSeverity = false, const bool reallocQueue = true) {
		return AELogParser::filterQueue(ptr, severity, strictSeverity, AELP_NO_MODULENAME, reallocQueue);
	}

	/// <summary>
	/// Filters the passed queue according to the given modulename
	/// @warning Make sure the queue is not circled (the "next node" pointer of the last entry in the queue is nullptr). Otherwise this will result in an infinite loop in the function
	/// @todo Implement a container for the queue to prevent this from happening
	/// </summary>
	/// <param name="ptr">The pointer to the existing queue</param>
	/// <param name="mname">The module name of the log entry to filter for</param>
	/// <param name="reallocQueue">Flag to reallocate the filtered queue (reduces the final memory usage)</param>
	/// @warning Use reallocQueue flag only if the queue was allocated on the heap (like with AELogParser::logToQueue(). Otherwise (if it's stack-allocated) it will try to delete[] the stack memory
	/// <returns>Same values as AELogParser::filterQueue() but only relating to the module name filter</returns>
	static inline cint filterQueueName(AELogEntry*& ptr, const std::string_view mname, const bool reallocQueue = true) {
		return AELogParser::filterQueue(ptr, AELP_SEVERITY_ALL, false, mname, reallocQueue);
	}


	/// <summary>
	/// Get the amount of valid entries in the log (with optional lowest severity setting).
	/// Valid entries are entries that are not AELOG_TYPE_INVALID
	/// </summary>
	/// <param name="severity">The lowest severity of the log to find</param>
	/// <returns>ULLINT_MAX if the severity is outside of the AELOG_TYPE_* range; ullint amount of entries</returns>
	inline ullint amountEntriesValid(const cint severity = AELOG_TYPE_DEBUG) const noexcept {
		if (!ace::utils::isInRange(AELOG_TYPE_INVALID, AELOG_TYPE_FATAL_ERROR, severity)) {
			return ULLINT_MAX;
		}
		ullint tempres = 0;
		for (int i = severity; i <= AELOG_TYPE_FATAL_ERROR; i++) {
			tempres += this->m_arrEntryAmount[i + 1];
		}
		return tempres;
	}

	/// <summary>
	/// Get the amount of invalid entries in the log. Invalid entries are of type AELOG_TYPE_INVALID
	/// </summary>
	/// <returns>ullint amount of invalid entries</returns>
	inline ullint amountEntriesInvalid(void) const noexcept {
		return m_arrEntryAmount[0]; // shortened version of AELOG_TYPE_INVALID (which is -1) + 1
	}

	/// <summary>
	/// Get the amount of entries of a certain type/severity
	/// </summary>
	/// <param name="severity">The severity/type of the log to find</param>
	/// <returns>ullint amount of entries on success; ULLINT_MAX if the severity is outside of the AELOG_TYPE_* range</returns>
	inline ullint amountEntriesType(const cint severity) const noexcept {
		if (!ace::utils::isInRange(AELOG_TYPE_INVALID, AELOG_TYPE_FATAL_ERROR, severity)) {
			return ULLINT_MAX;
		}
		return this->m_arrEntryAmount[severity + 1];
	}

	/// <summary>
	/// Get the amount of entries of a certain module name
	/// </summary>
	/// <param name="mname">The module name of the log entries to search for<</param>
	/// <returns>Amount of entries with a certain module name; ULLINT_MAX if the modulename wasn't found</returns>
	inline ullint amountEntriesName(const std::string_view mname) const noexcept {
		const decltype(this->m_mapModuleNames)::const_iterator iter = this->m_mapModuleNames.find(mname.data());
		if (iter == this->m_mapModuleNames.end()) {
			return ULLINT_MAX;
		}
		return iter->second.second; 
	}

	/// <summary>
	/// Checks if the current log file contains the given severity (or higher, depending on the "strict-severity" flag)
	/// </summary>
	/// <param name="severity">The severity to search for</param>
	/// <param name="strictSeverity">The flag to indicate whether the search for severity should be strict (exact)</param>
	/// <returns>True if the log contains the severity, false otherwise; false if the invalid severity was passed or file is closed</returns>
	inline bool containsSeverity(const cint severity, const bool strictSeverity = false) const noexcept {
		if (!ace::utils::isInRange(AELP_SEVERITY_ALL, AELOG_TYPE_FATAL_ERROR, severity) || this->isClosed()) {
			return false;
		}
		if (strictSeverity) {
			return this->m_arrEntryAmount[severity + 1];
		}
		for (int i = ((severity == AELP_SEVERITY_ALL) ? AELOG_TYPE_DEBUG : severity); i <= AELOG_TYPE_FATAL_ERROR; i++) {
			if (this->m_arrEntryAmount[i + 1]) {
				return true;
			}
		}
		return false;
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

	/// <summary>
	/// Finds the AELogEntryInfo corresponding to the next log entry with the specified severity and module name
	/// </summary>
	/// <param name="severity">The severity of the log entry to look for</param>
	/// <param name="mname">The module name of the log entry to search for</param>
	/// <param name="strictSeverity">The flag to indicate whether the search for severity should be strict (exact)</param>
	/// <returns>The AELogEntryInfo instance that corresponds to that log entry</returns>
	AELogEntryInfo findNextEntry(const cint severity, const std::string_view mname, const bool strictSeverity);

	/// <summary>
	/// Determines the error codes from the invalid values in the passed AELogEntryInfo instance
	/// </summary>
	/// <param name="leInfo">The passed AELogEntryInfo instance to check</param>
	/// <returns>AELP_ERR_NOERROR if everything is okay; AEFR_ERR_FILE_NOT_OPEN if it's fully invalid; AEFR_ERR_READ_EOF on invalid cursor; AELP_ERR_INVALID_MODULE_NAME on invalid module name; AELP_ERR_INVALID_SEVERITY on invalid type</returns>
	static constexpr cint errorFromAELEI(const AELogEntryInfo& leInfo) noexcept {
		if (leInfo.isInvalid()) {
			return AEFR_ERR_FILE_NOT_OPEN;
		}
		if (leInfo.isBroken()) {
			if (leInfo.isInvalidMName() && leInfo.isInvalidType()) {
				return AELP_ERR_FILTER_NO_MATCHES;
			}
			if (leInfo.isInvalidCursor()) {
				return AEFR_ERR_READ_EOF;
			}
			if (leInfo.isInvalidMName()) {
				return AELP_ERR_INVALID_MODULE_NAME;
			}
			if (leInfo.isInvalidType()) {
				return AELP_ERR_INVALID_SEVERITY;
			}
		}
		// everything is okay
		return AELP_ERR_NOERROR;
	}

	/// <summary>
	/// Checks if the passed severity is valid compared to the filtering severity.
	/// Moved from a lambda, so it's easier to use with other parts of the log parser.
	/// @note If strictSeverity is true, performs exact check. Otherwise if the passed entrySeverity is more or equal to the filtering filterSeverity.
	/// @note If filterSeverity is AELP_SEVERITY_ALL, the function always returns true
	/// </summary>
	/// <typeparam name="strictSeverity">Whether to check if both severity levels *match exactly*. Default: false</typeparam>
	/// <param name="entrySeverity">The severity to check/filter</param>
	/// <param name="filterSeverity">The filtering severity</param>
	/// <param name="strictSeverity">The flag to indicate whether the severity check should be strict (exact)</param>
	/// <returns>True if the filtering conditions are satisfied; false otherwise</returns>
	static constexpr bool checkSeverity(const cint entrySeverity, const cint filterSeverity, const bool strictSeverity = false) noexcept {
		if (filterSeverity == AELP_SEVERITY_ALL) {
			return true;
		}
		if (strictSeverity) {
			return entrySeverity == filterSeverity;
		}
		return entrySeverity >= filterSeverity;
	}

	/// <summary>
	/// Checks if the passed entry's and filter's module name indices match
	/// Moved from a lambda, so it's easier to use with other parts of the log parser.
	/// @note If filterNameIndex is 0, then it will always return true (we aren't checking for it)
	/// </summary>
	/// <param name="entryNameIndex">The module name index of the entry</param>
	/// <param name="filterNameIndex">The module name index filter</param>
	/// <returns></returns>
	static constexpr bool checkMName(const short entryNameIndex, const short filterNameIndex) noexcept {
		if (!filterNameIndex) {
			return true; // if the module name is empty -- return true always (we aren't checking for it)
		}
		return entryNameIndex == filterNameIndex;
	}

	/// The file reader of the opened log file.
	AEFileReader m_frLogReader;
	/// The list of all indexed *valid* entries in the log file.
	/// Each item contains their corresponding cursor position in the file and their type/severity.
	std::vector<AELogEntryInfo> m_vecEntryIndices;
	/// The list of all indexed *invalid* entries in the log file
	/// Each item contains their corresponding cursor position in the file.
	std::vector<llint> m_vecInvalidEntryIndices;
	/// The map of the all module names parsed in the log file
	std::unordered_map < std::string, std::pair <ullint, short>> m_mapModuleNames;
	/// The amount of log entries read in the file, separated by type/severity.
	std::array<ullint, 9> m_arrEntryAmount;
	/// The number corresponding to the currently-read *valid* entry in the log file.
	/// The maximum value corresponds to the size of m_vecEntryIndices
	std::atomic<std::size_t> m_ullCurrentEntry;

	//aaaand register it
	REGISTER_MODULE(AELogParser);
};




#endif // !ENGINE_LOG_PARSER_HPP
