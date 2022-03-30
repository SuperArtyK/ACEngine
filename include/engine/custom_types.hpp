
#pragma once

#ifndef ENGINE_CUSTOM_TYPES_HPP
#define ENGINE_CUSTOM_TYPES_HPP

//custom-defined types go here
#include <ctime>
#include <string>
#include <atomic>
#include "AELog_types.hpp"
#include "typedefs.hpp"

/// macro for entry order-number to be ignored by log writer
#define AELE_INVALID_ENTRY_ORDERNUM 0

/// Log entry struct for the, well, logger.
/// Hungarian notation is "le".
/// Flags start with AELE_
struct AELogEntry
{

	/// message of the log
	std::string m_sLogMessage;
	/// module that issued the log entry
	std::string m_sModuleName;
	/// time when log entry created
	std::time_t m_tmLogTime;

	/// next log entry (node)
	AELogEntry *m_lepNextNode = nullptr;

	/// order number of the entry, required for consequent writing
	/// and thread-safety of the log
	/// -1 ignores node
	/// checked first before waiting for status
	std::atomic<biguint> m_ullOrderNum;

	/// type of the log
	/// refer to aelog_types.hpp
	smalluint m_ucLogType;

	/// atomic flag to show that AELogEntry is ready to be written
	/// or not
	std::atomic<bool> m_bStatus;

	const static AELogEntry clear()
	{
		return {"", "", 0, nullptr, AELE_INVALID_ENTRY_ORDERNUM, AELOG_TYPE_INFO, false};
	}
};

#endif // !ENGINE_CUSTOM_TYPES_HPP
