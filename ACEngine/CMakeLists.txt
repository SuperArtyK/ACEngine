set(PROJECT_NAME ACEngine)

################################################################################
# Source groups
################################################################################
set(Header_Files__engine_modules
    "../AEFileWriter/include/AEFileWriter.hpp"
    "../AEFrame/include/AEFrame.hpp"
    "../AELogger/include/AELogger.hpp"
    "../AEMisc/include/AEFlags.hpp"
    "../AEMisc/include/AEMath.hpp"
    "../AEMisc/include/AETrigLookup.hpp"
    "../AEMisc/include/AETypedefs.hpp"
    "../AEMisc/include/AEUtils.hpp"
    "../AEModuleBase/include/AEModuleBase.hpp"
    "../AETimer/include/AETimer.hpp"
    "../AEVector/include/AEVector.hpp"
)
source_group("Header Files\\engine modules" FILES ${Header_Files__engine_modules})

set(Resource_Files
    "README.md"
)
source_group("Resource Files" FILES ${Resource_Files})

set(Resource_Files__Logo_resources
    "../resources/resource1.h"
)
source_group("Resource Files\\Logo resources" FILES ${Resource_Files__Logo_resources})

set(Source_Files
    "main_ACEngine.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(Source_Files__src__engine_modules
    "../AEFileReader/src/AEFileReader.cpp"
    "../AEFileWriter/src/AEFileWriter.cpp"
    "../AELogger/src/AELogger.cpp"
)
source_group("Source Files\\src\\engine modules" FILES ${Source_Files__src__engine_modules})

set(ALL_FILES
    ${Header_Files__engine_modules}
    ${Resource_Files}
    ${Resource_Files__Logo_resources}
    ${Source_Files}
    ${Source_Files__src__engine_modules}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE ACEngine)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# MSVC runtime library
################################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:Debug>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Release>:
            MultiThreaded
        >
        $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:Debug>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Release>:
            MultiThreaded
        >
        $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
"${CMAKE_CURRENT_SOURCE_DIR}/../AEMisc;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../AEModuleBase;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../AEFileWriter;"
        "${CMAKE_CURRENT_SOURCE_DIR}/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../AELogger;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../AEFrame;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../AETimer;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../AEVector;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../AEFileReader;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../AELogParser;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../AECustomTypes;"
	)
################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_FILE_OFFSET_BITS=64;"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_FILE_OFFSET_BITS=64;"
        "WIN32;"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Ox;
                /Ob2;
                /Oi;
                /GF;
                /Oy-;
                /Gy;
                /Ot
            >
            /permissive-;
            /MP;
            /sdl;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /wd26481;
            /wd4514;
            /wd5266;
            /wd5045;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Ox;
                /Ob2;
                /Oi;
                /GF;
                /Gy;
                /Ot
            >
            /permissive-;
            /MP;
            /sdl;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /wd26481;
            /wd4514;
            /wd5266;
            /wd5045;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
    AECustomTypes
    AEFileReader
    AEFileWriter
    AEFrame
    AELogParser
    AELogger
    AEMisc
    AEModuleBase
    AETimer
    AEVector
)

