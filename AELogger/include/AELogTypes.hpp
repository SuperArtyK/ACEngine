
/** @file AELogger/include/AELogTypes.hpp
 *  This file contains the macros for the AELogger's log types.
 *  Please do not touch! Thank you.
 *
 *  Should not cause everything to break :)
 */

#pragma once

#ifndef ENGINE_LOG_TYPES_HPP
#define ENGINE_LOG_TYPES_HPP

//log types
/// Macro for the AELogger's debug entry.
#define AELOG_TYPE_DEBUG 0
/// Macro for the AELogger's informational entry.
#define AELOG_TYPE_INFO 1
/// Macro for the AELogger's warning entry (something isn't right).
#define AELOG_TYPE_WARN 2
/// Macro for the AELogger's severe warning entry (something is wrong but it's not an error).
#define AELOG_TYPE_SEVERE_WARN 3
/// Macro for the AELogger's OK entry (something is good, like a check).
#define AELOG_TYPE_OK 4
/// Macro for the AELogger's successful entry (something done is successfull..like a module start).
#define AELOG_TYPE_SUCCESS 5
/// Macro for the AELogger's errorneus entry (something is bad..it's an error).
#define AELOG_TYPE_ERROR 6
/// Macro for the AELogger's fatally-errorenous entry (something critical is fatally-bad).
#define AELOG_TYPE_FATAL_ERROR 7

#endif // !ENGINE_LOG_TYPES_HPP
