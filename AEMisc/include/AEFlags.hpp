
/** @file AEMisc/include/AEFlags.hpp
 *  This file contains engine flags that are used to alter it's code and behaviour.
 *
 *  Should not cause everything to break :)
 */

#pragma once


#ifndef ENGINE_FLAGS_HPP
#define ENGINE_FLAGS_HPP

//Conveniently-placed flags of the modules for you to interact with

//Engine flags
/// Macro to enable the debug-marked features of the engine. Independent from debug/release compiler builds.
#define ENGINE_DEBUG 1
/// Macro for the engine's default delay in frames/ticks per second (used in AEFrame/AETimer).
#define ENGINE_FPS 60.0
/// Macro to enable the engine's global modules (like timer, logger, etc)
/// @note Undefine or comment-out to disable this feature
#define ENGINE_ENABLE_GLOBAL_MODULES 1

//AEFileWriter flags
/// Macro to enable the auto-flushing mechanism.
/// @note Undefine or comment out to disable the feature.
#define AEFW_AUTOFLUSH_ENABLE 
/// Macro for the default "use auto-flush" argument value.
#define AEFW_DEFAULT_AUTOFLUSH true
/// Macro for the default auto-flush amount value.
#define AEFW_DEFAULT_AUTOFLUSH_INTERVAL (ullint)-1
/// Macro to enable the engine's trigonometry lookup code.
/// @see AETrigLookup.hpp
#define ENGINE_ENABLE_TRIG_LOOKUP

//AELogger flags
/// Macro for the AELogger's default queue size (if none was provided).
#define AELOG_DEFAULT_QUEUE_SIZE 1024
/// Macro for the AELogger's default module name (if none was provided when calling the write-to-log request).
#define AELOG_DEFAULT_MODULE_NAME "ACEngine"
/// Macro for the default reserve amount of the AELogger's allocation vector (stores all queue ptrs).
/// @warning I personally wouldn't advise to play with it.
#define AELOG_DEFAULT_ALLOC_VECTOR_RESERVE 32
#define AELOG_DEFAULT_LOG_PATH "logs/"

//AELogEntry flags
#define AELOG_ENTRY_MESSAGE_SIZE 511

#define AELOG_ENTRY_MODULENAME_SIZE 31


//AEVector
/// A macro for the AEVectors operator[] to wrap dimensions if the index is bigger than the dimension amount.
#define AEVEC_WRAP_DIMENSIONS 1

#endif // !ENGINE_FLAGS_HPP
