
/*! @file AECustomTypes/include/AELogTypes.hpp
 *  This file contains the macros for the AELogEntry (and consequently AELogger's) log types.
 *  Please do not touch! (Unless really-really sure) Thank you.
 *
 *  Should not cause everything to break :)
 */

#pragma once

#ifndef ENGINE_LOG_TYPES_HPP
#define ENGINE_LOG_TYPES_HPP

//log types
/// Macro for the AELogger's entry of invalid type.
#define AELOG_TYPE_INVALID cint{-1}

/// Macro for the AELogger's debug entry.
#define AELOG_TYPE_DEBUG cint{0}

/// Macro for the AELogger's informational entry.
#define AELOG_TYPE_INFO cint{1}

/// Macro for the AELogger's warning entry (something isn't right).
#define AELOG_TYPE_WARN cint{2}

/// Macro for the AELogger's severe warning entry (something is wrong but it's not an error).
#define AELOG_TYPE_SEVERE_WARN cint{3}

/// Macro for the AELogger's OK entry (something is good, like a check).
#define AELOG_TYPE_OK cint{4}

/// Macro for the AELogger's successful entry (something done is successfull..like a module start).
#define AELOG_TYPE_SUCCESS cint{5}

/// Macro for the AELogger's errorneus entry (something is bad..it's an error).
#define AELOG_TYPE_ERROR cint{6}

/// Macro for the AELogger's fatally-errorenous entry (something critical is fatally-bad).
#define AELOG_TYPE_FATAL_ERROR cint{7}

#endif // !ENGINE_LOG_TYPES_HPP
