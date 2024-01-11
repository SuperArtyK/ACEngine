
/** @file AELogParser/include/AELogParser.hpp
 *  This file contains the [stuff]
 *
 *  Should not cause everything to break :)
 */

#pragma once

#ifndef ENGINE_LOG_PARSER_HPP
#define ENGINE_LOG_PARSER_HPP

#include "include/AEModuleBase.hpp"
#include "include/AETypedefs.hpp"
#include "include/AEUtils.hpp"
#include "include/AEFileReader.hpp"
#include "include/AETypedefs.hpp"
#include "include/AELogTypes.hpp"
#include "include/AEFlags.hpp"
#include "include/AELogEntry.hpp"
#include <vector>
#include <array>
#include <string_view>

#define AELP_ERR_NOERROR ENGINE_MODULE_ERR_NOERROR

//index for the "entry indices" array to get the std::vector corresponding to the AELogger's invalid type
#define _AELP_INVALID_ENTRY_INDEX 8


class AELogParser : public __AEModuleBase<AELogParser> {

public:
	explicit AELogParser(const std::string_view fname) : 
		m_vecEntryIndices({}), m_vecInvalidEntryIndices({}), m_arrEntryAmount({}), m_ullCurrentEntry(0) {
		m_vecEntryIndices.reserve(AELOG_DEFAULT_QUEUE_SIZE * 10);
		m_vecInvalidEntryIndices.reserve(AELOG_DEFAULT_QUEUE_SIZE);
		this->openLog(fname);
		
	}

	~AELogParser(void) {
		this->closeLog();
	}

	//opens log file and indexes it
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

	inline cint closeLog(void) {
		m_vecEntryIndices.clear();
		m_vecInvalidEntryIndices.clear();
		std::memset(this->m_arrEntryAmount.data(), NULL, sizeof(this->m_arrEntryAmount.data()));
		m_ullCurrentEntry = 0;
		return this->m_frLogReader.closeFile();
	}

	//parse the log entry and give data to the given entry
	cint nextEntry(AELogEntry& entry, const cint severity = AELOG_TYPE_DEBUG);
	
	

	cint logToQueue(AELogEntry*& begin, const cint severity = AELOG_TYPE_DEBUG);
	
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

	inline ullint amountInvalidEntries(void) const noexcept {
		return m_arrEntryAmount[0]; // shortened version of AELOG_TYPE_INVALID (which is -1) + 1
	}

	inline ullint amountTypeEntries(const cint severity) const noexcept {
		if (!ace::utils::isInRange<cint>(AELOG_TYPE_INVALID, AELOG_TYPE_FATAL_ERROR, severity)) {
			return ULLINT_MAX;
		}
		return this->m_arrEntryAmount[severity + 1];
	}

	inline bool isOpen(void) const noexcept {
		return this->m_frLogReader.isOpen();
	}

	inline bool isClosed(void) const noexcept {
		return this->m_frLogReader.isClosed();
	}


private:




	/// The reader of the opened log file
	AEFileReader m_frLogReader;
	/// The array of the arrays of log entry cursor indices in the file, corresponding to each log type (including "invalid entry" for invalid stuff)
	//std::array<std::vector<ullint>, 9> m_arrEntryIndices;

	std::vector<std::pair<ullint, cint>> m_vecEntryIndices;
	std::vector<ullint> m_vecInvalidEntryIndices;
	
	/// The amount of log entries read in the file;
	std::array<ullint, 9> m_arrEntryAmount;
	std::atomic<ullint> m_ullCurrentEntry;

};

//aaaand register it
REGISTER_MODULE(AELogParser)


#endif // !ENGINE_LOG_PARSER_HPP
