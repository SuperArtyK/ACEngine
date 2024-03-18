
/*! @file AEMisc/include/AEFlags.hpp
 *
 *  @brief This file contains the **flags to manipulate engine's funcitonality**.
 *
 *  This is the only file, where you, the programmer, are welcome to modify stuff to your needs,
 *  unless you know your way around the engine and stuff.
 *
 *  But if you do, **you're welcome to help me in its development too ;)**
 *
 *	@date 2023/09/21
 *	
 *	@author	Artemii Kozhemiak (SuperArtyK)
 *	
 *	@copyright MIT License
 *	
 *  *Should not cause everything to break :)*
 */

#pragma once


#ifndef ENGINE_FLAGS_HPP
#define ENGINE_FLAGS_HPP

//Conveniently-placed flags of the modules for you to interact with

//Engine flags
/// Macro to enable the **debug-marked features** of the engine. 
/// It's an engine thing, ***separate*** from debug/release compiler builds.
/// @note Set to 1 to enable, and 0 to disable this feature
#define ENGINE_DEBUG 1
/// Macro for the engine's **default delay** in frames/ticks per second (used in AEFrame and AETimer).
/// @note Set to any positive value to enable, and any negative value or 0 to disable this feature
/// @attention Disabling this disables the sleep functionality in instances of AEFrame and AETimer that use the default delay.
/// @warning Undefining this macro will produce a compilation error!
/// @see AEFrame
/// @see AETimer
#define ENGINE_FPS 60.0
/// Macro to enable the engine's **global modules** (like AETimer, AELogger, etc).
/// @note Set to 1 to enable, and 0 to disable this feature
/// @remark Disabling this code *may* break your code (from compiling), as it removes the global module declarations completely
#define ENGINE_ENABLE_GLOBAL_MODULES 1
/// Macro to enable the engine's **assert code**
/// @note This only works if the debug features are enabled (#ENGINE_DEBUG).
/// @note Set to 1 to enable, and 0 to disable this feature
/// @see #aceAssert()
#define ENGINE_ENABLE_ASSERT 1
/// Macro to enable the engine's **trigonometry lookup tables and code**.
/// @note Set to 1 to enable, and 0 to disable this feature
/// @see AETrigLookup.hpp
#define ENGINE_ENABLE_MATH_LOOKUP 1

//AEFileWriter flags
/// Macro to enable the AEFileWriter's **auto-flushing mechanism**.
/// @note Set to 1 to enable, and 0 to disable this feature
/// @see AEFileWriter::autoFlush()
#define AEFW_AUTOFLUSH_ENABLE 1
/// Macro for the AEFileWriter's **default auto-flushing action** (default argument in file-writing functions).	
/// @note This only works/matters if the auto-flushing feature is enabled (#AEFW_AUTOFLUSH_ENABLE)
/// @see AEFileWriter::autoFlush()
#define AEFW_DEFAULT_AUTOFLUSH true
/// Macro for the AEFileWriter's **default value to trigger auto-flushing**.
/// @remark Default value (ullint)-1 means **never**. Or almost never, once about every 2^64 write operations.
/// @note This only works/matters if the auto-flushing feature is enabled (#AEFW_AUTOFLUSH_ENABLE)
/// @see AEFileWriter::autoFlush()
#define AEFW_DEFAULT_AUTOFLUSH_INTERVAL (ullint)-1

//AELogger flags
/// Macro for the AELogger's **default queue size**
/// @warning Setting this to a very large or a negative number can crash the engine on AELogger's creation.
/// @warning All because the queue too is large to be allocated.
#define AELOG_DEFAULT_QUEUE_SIZE (ullint)2048
/// Macro for the AELogger's **default module/log invoker name**
/// @warning This must follow the rules for the module name: Be alphanumeric+underscore and be no more than #AELE_MODULENAME_SIZE.
/// @warning Otherwise any logs invoked with a default module name will fail.
/// @see AELogger::writeToLog()
/// @see AELogger::writeToLogDebug()
/// @see AELogger::writeToLogDirectly()
#define AELOG_DEFAULT_MODULE_NAME "ACEngine"
/// Macro for the AELogger's **default reserve amount of the allocation vector** (which stores all queue pointers).
/// @attention I personally *wouldn't* advise to play with it (but you can).
/// @remark 32 allocations, 1.5 queue size factor -- 884 million entries. If each node is 568 bytes -- it all will eat up 468 gb of ram.
/// @remark I think that's enough :)
#define AELOG_DEFAULT_ALLOC_VECTOR_RESERVE 32
/// Macro for the AELogger's **default log file path**
/// @see AELogger::openLog()
#define AELOG_DEFAULT_LOG_PATH "./logs/"

//AELogEntry flags
/// Macro for the size of the AELogEntry's **message in characters** (excluding null termination)
/// @note Changing this -- changes the size of the AELogEntry structure
/// @attention Mind the size here:
///		* Too little will make the messages cut off
/// 	* Too large will increase the memory usage by a lot
/// @see AELogEntry::m_sLogMessage[]
#define AELE_MESSAGE_SIZE 511
/// Macro for the size of the AELogEntry's **module name in characters** (excluding null termination)
/// @note Changing this -- changes the size of the AELogEntry structure
/// @attention Mind the size here:
///		* Too little will make the messages cut off
/// 	* Too large will increase the memory usage by a lot
/// @see AELogEntry::m_sModuleName[]
#define AELE_MODULENAME_SIZE 31

//AEVector
/// A macro for the AEVector::operator[] to **wrap dimension index around** if the index is bigger than the dimension amount.
/// Example: 2d vector a, possible values to the [] is 0 and 1. If you input 2, this will be equivalent of [0]; 3 -> [1], and so on.
/// @note Set to 1 to enable, and 0 to disable this feature
/// @attention If this is disabled, and you try exceeding the dimension amount -- **sigsegv** will smack you in the face.
/// @see AEVector::operator[]()
#define AEVEC_WRAP_DIMENSIONS 1

//AEFrame
/// Macro for the AEFrame to **reset the timepoint when behind X milliseconds**.
/// This makes the AEFrame to perform a lateness check when calling the AEFrame::sleep() function. <br>
/// It checks if the difference between the current time and the timepoint stored in the AEFrame instance
///		is larger than the time in milliseconds indicated by this macro
///	@attention Setting this to a low value can decrease the program performance
///		as well as the accuracy of the AEFrame
/// @note Set to positive integer value to enable, and 0 to disable this feature
/// @see AEFrame::sleep()
#define AEF_RESET_TIME_WHEN_BEHIND_MS 100
/// Macro for the clock for the AEFrame to use for counting internally
/// @note This clock needs to be a chrono-compatible clock
/// @see SysClock
/// @see HighResClock
/// @see SteadyClock
#define AEF_CLOCK HighResClock

#endif // !ENGINE_FLAGS_HPP
