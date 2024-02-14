
/*! @file AECustomTypes/include/AELogEntryInfo.hpp
 *  This file contains the macros for the AELogEntryInfo (and consequently AELogEntry's) log types.
 *
 *  Should not cause everything to break :)
 */


#pragma once

#ifndef ENGINE_AELOGENTRYINFO_HPP
#define ENGINE_AELOGENTRYINFO_HPP

#include "include/AETypedefs.hpp"

/// Macro for the AELogEntryInfo's value for the invalid cursor index
#define AELEI_INVALID_CURSOR -1
/// Macro for the AELogEntryInfo's value for the invalid module name index
#define AELEI_INVLAID_MNAME -1
/// Macro for the AELogEntryInfo's value for the invalid log type
#define AELEI_INVALID_TYPE -1


/// <summary>
/// The struct that holds the information about the log entry, used by AELogParser
/// @see AELogEntry
/// </summary>
struct AELogEntryInfo {

	/// The cursor number of the log entry in the opened log file
	llint cursorIndex = 0;
	/// The index of the module name of the log entry
	/// The module names are enumerated when initially parsing the log file
	/// @see AELogParser::openLog()
	short mnameIndex = 0;
	/// The type of the log entry
	cint logType = 0;

	/// <summary>
	/// Returns the invalid instance of the AELogEntryInfo
	/// </summary>
	/// <returns>Invalid instance of the AELogEntryInfo</returns>
	static constexpr AELogEntryInfo invalidEntry() noexcept { return AELogEntryInfo{ AELEI_INVALID_CURSOR, AELEI_INVLAID_MNAME, AELEI_INVALID_TYPE }; }

	/// <summary>
	/// Checks if the current instance is invalid
	/// @see AELogEntryInfo::isInvalidCursor()
	/// @see AELogEntryInfo::isInvalidMName()
	/// @see AELogEntryInfo::isInvalidType()
	/// </summary>
	/// <returns>True if the instance is invalid, false otherwise</returns>
	constexpr bool isInvalid() const noexcept {
		return (this->isInvalidCursor() && this->isInvalidMName() && this->isInvalidType());
	}

	/// <summary>
	/// Checks if the current instance is broken (partially invalid)
	/// @see AELogEntryInfo::isInvalidCursor()
	/// @see AELogEntryInfo::isInvalidMName()
	/// @see AELogEntryInfo::isInvalidType()
	/// </summary>
	/// <returns>True if the instance is broken, false otherwise</returns>
	constexpr bool isBroken() const noexcept {
		return (this->isInvalidCursor() || this->isInvalidMName() || this->isInvalidType());
	}

	/// <summary>
	/// Checks if the cursor index of the current instance is invalid
	/// </summary>
	/// <returns>True if it's invalid, false otherwise</returns>
	constexpr bool isInvalidCursor() const noexcept {
		return (this->cursorIndex == AELEI_INVALID_CURSOR);
	}

	/// <summary>
	/// Checks if the cursor index of the current instance is invalid
	/// </summary>
	/// <returns>True if it's invalid, false otherwise</returns>
	constexpr bool isInvalidMName() const noexcept {
		return (this->mnameIndex == AELEI_INVLAID_MNAME);
	}

	/// <summary>
	/// Checks if the cursor index of the current instance is invalid
	/// </summary>
	/// <returns>True if it's invalid, false otherwise</returns>
	constexpr bool isInvalidType() const noexcept {
		return (this->logType == AELEI_INVALID_TYPE);
	}


};

#endif // !ENGINE_AELOGENTRYINFO_HPP


