cmake_minimum_required(VERSION 3.24)
project(Ascent VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable vcpkg manifest mode if vcpkg.json is present
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
    # Enable manifest mode and point to the correct vcpkg.json location
    if(EXISTS "${CMAKE_SOURCE_DIR}/vcpkg.json")
        set(VCPKG_MANIFEST_DIR "${CMAKE_SOURCE_DIR}" CACHE STRING "Vcpkg manifest directory")
        set(VCPKG_MANIFEST_MODE ON CACHE BOOL "Enable vcpkg manifest mode")
        # Use only local vcpkg installation to avoid conflicts
        set(VCPKG_INSTALLED_DIR "${CMAKE_SOURCE_DIR}/vcpkg_installed" CACHE STRING "Vcpkg installed directory")
        # For Visual Studio generator, automatically enable vcpkg manifest in MSBuild
        if(CMAKE_GENERATOR MATCHES "Visual Studio")
            set(CMAKE_VS_GLOBALS "VcpkgEnableManifest=true")
        endif()
    endif()
endif()

# Enable modern CMake policies
cmake_policy(SET CMP0074 NEW)  # find_package uses <PackageName>_ROOT variables

# Project directories with separate debug/release outputs
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin)
set(BIN_DEBUG_DIR ${BIN_DIR}/debug)
set(BIN_RELEASE_DIR ${BIN_DIR}/release)
set(BUILD_OUTPUT_DIR ${CMAKE_BINARY_DIR})

# Create necessary directories
file(MAKE_DIRECTORY ${BIN_DEBUG_DIR})
file(MAKE_DIRECTORY ${BIN_RELEASE_DIR})

# Find packages using local vcpkg only
# Remove system vcpkg to avoid debug/release library conflicts
if(EXISTS "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/share")
    set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/share")
endif()

# Find Ogre3D
find_package(OGRE CONFIG REQUIRED COMPONENTS Bites Overlay)

# Find Boost
find_package(Boost REQUIRED COMPONENTS serialization)



# Compiler-specific flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wconversion -Wsign-conversion")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wconversion -Wsign-conversion")
    # Suppress specific warnings that are too noisy
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-utf8 -Wno-deprecated-declarations")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /EHsc /FS")  # /FS enables parallel compilation with PDB files
    # Disable specific MSVC warnings that are too noisy
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4996 /wd4201")  # Disable deprecation and nameless struct/union warnings
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od /MDd /DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /MD /DNDEBUG")
endif()

# Collect all source files automatically from src directory and subdirectories
file(GLOB_RECURSE SOURCES
    ${SRC_DIR}/*.cpp
)
list(FILTER SOURCES EXCLUDE REGEX ".*stdafx\\.cpp$")

# Collect all header files (for IDE integration)
file(GLOB_RECURSE HEADERS
    ${SRC_DIR}/*.hpp
)

# Print discovered files for debugging
message(STATUS "Discovered Ascent source files:")
foreach(src_file ${SOURCES})
    message(STATUS "  ${src_file}")
endforeach()

message(STATUS "Discovered Ascent header files:")
foreach(header_file ${HEADERS})
    message(STATUS "  ${header_file}")
endforeach()

# Create executable
add_executable(Ascent ${SOURCES} ${HEADERS})

# Set target properties with separate debug/release output directories
set_target_properties(Ascent PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_DEBUG_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_RELEASE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${BIN_RELEASE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${BIN_RELEASE_DIR}
)

# Add include directories
target_include_directories(Ascent PRIVATE 
    ${SRC_DIR}
    ${SRC_DIR}/Entities
    ${SRC_DIR}/Entities/Enemies
    ${SRC_DIR}/Entities/Player
    ${SRC_DIR}/Entities/Projectiles
    ${SRC_DIR}/Game
    ${SRC_DIR}/Map
    ${OGRE_INCLUDE_DIRS}
)

# Link Ogre3D libraries (OgreBites includes input handling via SDL2)
target_link_libraries(Ascent PRIVATE 
    OgreMain
    OgreOverlay
    OgreBites
    Boost::serialization
)

# Link Boost libraries
target_link_libraries(Ascent PRIVATE 
    Boost::serialization
)

# Set additional properties for Visual Studio vcpkg integration
set_target_properties(Ascent PROPERTIES
    # Intermediate files go to build directory  
    ARCHIVE_OUTPUT_DIRECTORY ${BUILD_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${BUILD_OUTPUT_DIR}
    # Let vcpkg automatically copy required DLLs
    VS_GLOBAL_VcpkgEnabled true
    VS_GLOBAL_VcpkgEnableManifest true
)

# Enable automatic DLL copying for vcpkg dependencies
if(DEFINED ENV{VCPKG_ROOT})
    set_target_properties(Ascent PROPERTIES
        VCPKG_APPLOCAL_DEPS ON
    )
endif()

# Copy res folder to output directories
if(EXISTS "${CMAKE_SOURCE_DIR}/res")
    # Copy res to appropriate directory based on configuration
    add_custom_command(TARGET Ascent POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/res"
        "$<IF:$<CONFIG:Debug>,${BIN_DEBUG_DIR}/res,${BIN_RELEASE_DIR}/res>"
        COMMENT "Copying resources to output directory"
    )
else()
    message(WARNING "Resources directory not found at ${CMAKE_SOURCE_DIR}/res")
endif()

# Copy Ogre configuration files to output directories
add_custom_command(TARGET Ascent POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/cfg/plugins.cfg"
    "${CMAKE_SOURCE_DIR}/cfg/plugins_d.cfg"
    "${CMAKE_SOURCE_DIR}/cfg/resources.cfg"
    "${CMAKE_SOURCE_DIR}/cfg/resources_d.cfg"
    "$<IF:$<CONFIG:Debug>,${BIN_DEBUG_DIR},${BIN_RELEASE_DIR}>"
    COMMENT "Copying Ogre configuration files to output directory"
)

# Copy Ogre plugin DLLs and dependencies to output directory
if(WIN32)
    add_custom_command(TARGET Ascent POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Copying Ogre plugins and DLLs..."
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/x64-windows/$<IF:$<CONFIG:Debug>,debug/,>plugins/ogre"
        "$<IF:$<CONFIG:Debug>,${BIN_DEBUG_DIR},${BIN_RELEASE_DIR}>"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/x64-windows/$<IF:$<CONFIG:Debug>,debug/,>bin"
        "$<IF:$<CONFIG:Debug>,${BIN_DEBUG_DIR},${BIN_RELEASE_DIR}>"
        COMMENT "Copying Ogre plugins and dependency DLLs"
    )
endif()

# Print build information
message(STATUS "Ascent v${PROJECT_VERSION} Build Configuration:")
message(STATUS "  Platform:     ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Compiler:     ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Build Type:   ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Source Dir:   ${SRC_DIR}")
message(STATUS "  Debug Dir:    ${BIN_DEBUG_DIR}")
message(STATUS "  Release Dir:  ${BIN_RELEASE_DIR}")
message(STATUS "  Build Dir:    ${CMAKE_BINARY_DIR}")
message(STATUS "  Flags:        ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}")
