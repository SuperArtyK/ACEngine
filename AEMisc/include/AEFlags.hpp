#pragma once


#ifndef ENGINE_FLAGS_HPP
#define ENGINE_FLAGS_HPP

//Conveniently-placed flags of the modules for you to interact with

//AEFileWriter flags
/// Macro to enable the auto-flushing mechanism
/// Undefine to disable the feature
#define AEFW_AUTOFLUSH_ENABLE 
/// Macro for the default "use auto-flush" argument value
#define AEFW_DEFAULT_AUTOFLUSH true
/// Macro for the default auto-flush amount value
#define AEFW_DEFAULT_AUTOFLUSH_INTERVAL 1




#endif // !ENGINE_FLAGS_HPP
