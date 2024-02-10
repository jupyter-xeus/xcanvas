############################################################################
# Copyright (c) 2021, Martin Renou                                         #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

cmake_minimum_required(VERSION 3.8)
project(xcanvas)

message(STATUS "Forcing tests build type to Release")
set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)

set(XCANVAS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(XCANVAS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Configuration
# =============

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(XCANVAS_INSTALL_LIBRARY_DIR "\"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}\"")

configure_file (
    "${XCANVAS_INCLUDE_DIR}/xcanvas/xcanvas_config_cling.hpp.in"
    "${XCANVAS_INCLUDE_DIR}/xcanvas/xcanvas_config_cling.hpp"
)

# Versionning
# ===========

file(STRINGS "${XCANVAS_INCLUDE_DIR}/xcanvas/xcanvas_config.hpp" xcanvas_version_defines
     REGEX "#define XCANVAS_VERSION_(MAJOR|MINOR|PATCH)")
foreach(ver ${xcanvas_version_defines})
    if(ver MATCHES "#define XCANVAS_VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
        set(XCANVAS_VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
    endif()
endforeach()
set(XCANVAS_VERSION
    ${XCANVAS_VERSION_MAJOR}.${XCANVAS_VERSION_MINOR}.${XCANVAS_VERSION_PATCH})
message(STATUS "xcanvas version: v${XCANVAS_VERSION}")

# Binary version
# See the following URL for explanations about the binary versionning
# https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html#Updating-version-info
file(STRINGS "${XCANVAS_INCLUDE_DIR}/xcanvas/xcanvas_config.hpp" xcanvas_version_defines
    REGEX "#define XCANVAS_BINARY_(CURRENT|REVISION|AGE)")
foreach(ver ${xcanvas_version_defines})
    if(ver MATCHES "#define XCANVAS_BINARY_(CURRENT|REVISION|AGE) +([^ ]+)$")
        set(XCANVAS_BINARY_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
    endif()
endforeach()
set(XCANVAS_BINARY_VERSION
    ${XCANVAS_BINARY_CURRENT}.${XCANVAS_BINARY_REVISION}.${XCANVAS_BINARY_AGE})
message(STATUS "xcanvas binary version: v${XCANVAS_BINARY_VERSION}")

# Build options
# =============

option(XCANVAS_BUILD_SHARED_LIBS "Build xcanvas shared library." ON)
option(XCANVAS_BUILD_STATIC_LIBS "Build xcanvas static library (default if BUILD_SHARED_LIBS is OFF)." ON)

# Dependencies
# ============

set(xtl_REQUIRED_VERSION 0.7.0)
set(xproperty_REQUIRED_VERSION 0.11.0)
set(xwidgets_REQUIRED_VERSION 0.27.1)

if (NOT TARGET xtl)
    find_package(xtl ${xtl_REQUIRED_VERSION} REQUIRED)
endif ()

if (NOT TARGET xproperty)
    find_package(xproperty ${xproperty_REQUIRED_VERSION} REQUIRED)
endif ()

if (NOT TARGET xwidgets)
    find_package(xwidgets ${xwidgets_REQUIRED_VERSION} REQUIRED)
endif ()

# Source files
# ============

set(XCANVAS_HEADERS
    ${XCANVAS_INCLUDE_DIR}/xcanvas/xcanvas_config_cling.hpp
    ${XCANVAS_INCLUDE_DIR}/xcanvas/xcanvas_config.hpp
    ${XCANVAS_INCLUDE_DIR}/xcanvas/xcanvas.hpp
)

set(XCANVAS_SOURCES
    ${XCANVAS_SOURCE_DIR}/xcanvas.cpp
)

# Targets and link
# ================

include(CheckCXXCompilerFlag)

macro(xcanvas_create_target target_name linkage output_name)
    string(TOUPPER "${linkage}" linkage_upper)

    if (NOT ${linkage_upper} MATCHES "^(SHARED|STATIC)$")
        message(FATAL_ERROR "Invalid library linkage: ${linkage}")
    endif ()

    if (${linkage_upper} STREQUAL "STATIC")
        set(XWIDGETS_TARGET_NAME xwidgets-static)
    else()
        set(XWIDGETS_TARGET_NAME xwidgets)
    endif()

    # Output
    # ======

    add_library(${target_name} ${linkage_upper} ${XCANVAS_SOURCES} ${XCANVAS_HEADERS})
    target_include_directories(${target_name} PUBLIC $<BUILD_INTERFACE:${XCANVAS_INCLUDE_DIR}>
                                               $<INSTALL_INTERFACE:include>)

    target_link_libraries(${target_name}
                          PUBLIC xtl
                          PUBLIC xproperty
                          PUBLIC ${XWIDGETS_TARGET_NAME})

    set_target_properties(${target_name} PROPERTIES
                          PUBLIC_HEADER "${XCANVAS_HEADERS}"
                          COMPILE_DEFINITIONS "XCANVAS_EXPORTS"
                          PREFIX ""
                          VERSION ${XCANVAS_BINARY_VERSION}
                          SOVERSION ${XCANVAS_BINARY_CURRENT}
                          OUTPUT_NAME "lib${output_name}")

    # Compilation flags
    # =================

    string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

    set_target_properties(${target_name} PROPERTIES
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED 17)

    target_compile_features(${target_name} PRIVATE cxx_std_17)

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Intel")
        target_compile_options(${target_name} PUBLIC -Wunused-parameter -Wextra -Wreorder)
    endif()

    if (${linkage_upper} STREQUAL "STATIC")
        target_compile_definitions(${target_name} PUBLIC XWIGDETS_STATIC_LIB)
    endif ()

    if(MSVC)
        target_compile_definitions(${target_name} PUBLIC -DNOMINMAX)
        target_compile_options(${target_name} PUBLIC /DGUID_WINDOWS /MP /bigobj)
        target_compile_options(${target_name} PUBLIC /wd4251 /wd4996)
        # Allows to compile in debug without requiring each dependencies to
        # be compiled in debug
        if(${U_CMAKE_BUILD_TYPE} MATCHES DEBUG)
            set(CMAKE_CXX_FLAGS_DEBUG "/DNDEBUG /MD /Zi /Ob0 /Od /RTC1")
            message(STATUS "Overriding CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
        endif()
    endif()
endmacro()

set(xcanvas_targets "")

if (XCANVAS_BUILD_SHARED_LIBS)
    xcanvas_create_target(xcanvas SHARED xcanvas)
    list(APPEND xcanvas_targets xcanvas)
endif()

if (XCANVAS_BUILD_STATIC_LIBS)
    # On Windows, a static library should use a different output name
    # to avoid the conflict with the import library of a shared one.
    if (CMAKE_HOST_WIN32)
        xcanvas_create_target(xcanvas-static STATIC xcanvas-static)
    else ()
        xcanvas_create_target(xcanvas-static STATIC xcanvas)
    endif ()

    list(APPEND xcanvas_targets xcanvas-static)
endif()

# Installation
# ============

set(XCANVAS_CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE STRING "install path for xcanvasConfig.cmake")

install(TARGETS ${xcanvas_targets}
        EXPORT ${PROJECT_NAME}-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xcanvas)

# Makes the project importable from the build directory
export(EXPORT ${PROJECT_NAME}-targets
       FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

# Configure 'xcanvasConfig.cmake' for a build tree
set(XCANVAS_CONFIG_CODE "####### Expanded from \@XCANVAS_CONFIG_CODE\@ #######\n")
set(XCANVAS_CONFIG_CODE "${XCANVAS_CONFIG_CODE}set(CMAKE_MODULE_PATH \"${CMAKE_CURRENT_SOURCE_DIR}/cmake;\${CMAKE_MODULE_PATH}\")\n")
set(XCANVAS_CONFIG_CODE "${XCANVAS_CONFIG_CODE}##################################################")
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${PROJECT_BINARY_DIR})

# Configure 'xcanvasConfig.cmake' for an install tree
set(XCANVAS_CONFIG_CODE "")
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${XCANVAS_CMAKECONFIG_INSTALL_DIR})
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
                                 VERSION ${XCANVAS_VERSION}
                                 COMPATIBILITY AnyNewerVersion)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
              DESTINATION ${XCANVAS_CMAKECONFIG_INSTALL_DIR})
install(EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}Targets.cmake
DESTINATION ${XCANVAS_CMAKECONFIG_INSTALL_DIR})
