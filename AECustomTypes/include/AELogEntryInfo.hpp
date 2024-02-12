
/*! @file AECustomTypes/include/AELogEntryInfo.hpp
 *  This file contains the macros for the AELogEntryInfo (and consequently AELogEntry's) log types.
 *
 *  Should not cause everything to break :)
 */


#pragma once

#ifndef ENGINE_AELOGENTRYINFO_HPP
#define ENGINE_AELOGENTRYINFO_HPP

#include "include/AETypedefs.hpp"

/// <summary>
/// The struct that holds the information about the log entry, used by AELogParser
/// @see AELogEntry
/// </summary>
struct AELogEntryInfo {
	/// The cursor number of the log entry in the opened log file
	llint cursorIndex;
	/// The index of the module name of the log entry
	/// The module names are enumerated when initially parsing the log file
	/// @see AELogEntry::openLog()
	short mnameIndex;
	/// The type of the log entry
	cint logType;
};

#endif // !ENGINE_AELOGENTRYINFO_HPP


