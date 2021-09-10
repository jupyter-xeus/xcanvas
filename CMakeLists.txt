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
foreach(ver ${xCANVAS_version_defines})
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

# Dependencies
# ============

find_package(xtl 0.6.5 REQUIRED)
find_package(xeus 0.23.3 REQUIRED)
find_package(xwidgets 0.20.0 REQUIRED)

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

# ======

add_library(xcanvas SHARED ${XCANVAS_SOURCES} ${XCANVAS_HEADERS})
target_include_directories(xcanvas PUBLIC $<BUILD_INTERFACE:${XCANVAS_INCLUDE_DIR}>
                                           $<INSTALL_INTERFACE:include>)

target_link_libraries(xcanvas
    PUBLIC xtl
    PUBLIC xeus
    PUBLIC xwidgets)

set_target_properties(xcanvas PROPERTIES
                      PUBLIC_HEADER "${XCANVAS_HEADERS}"
                      COMPILE_DEFINITIONS "XCANVAS_EXPORTS"
                      PREFIX ""
                      VERSION ${XCANVAS_BINARY_VERSION}
                      SOVERSION ${XCANVAS_BINARY_CURRENT}
                      OUTPUT_NAME "libxcanvas")

# Compilation flags
# =================

include(CheckCXXCompilerFlag)
string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)
OPTION(DISABLE_ARCH_NATIVE "disable -march=native flag" OFF)

set_target_properties(xcanvas PROPERTIES
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED 14)

target_compile_features(xcanvas PRIVATE cxx_std_14)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    target_compile_options(xcanvas PUBLIC -Wunused-parameter -Wextra -Wreorder)
    if (DISABLE_ARCH_NATIVE)
        target_compile_options(xcanvas PUBLIC -mtune=generic)
    else()
        target_compile_options(xcanvas PUBLIC -march=native)
    endif()

    # Enable link time optimization and set the default symbol
    # visibility to hidden (very important to obtain small binaries)
    if (NOT ${U_CMAKE_BUILD_TYPE} MATCHES DEBUG)
        # Check for Link Time Optimization support
        # (GCC/Clang)
        CHECK_CXX_COMPILER_FLAG("-flto" HAS_LTO_FLAG)
        if (HAS_LTO_FLAG)
            target_compile_options(xcanvas PUBLIC -flto)
        endif()

        # Intel equivalent to LTO is called IPO
        if (CMAKE_CXX_COMPILER_ID MATCHES "Intel")
            CHECK_CXX_COMPILER_FLAG("-ipo" HAS_IPO_FLAG)
            if (HAS_IPO_FLAG)
                target_compile_options(xcanvas PUBLIC -ipo)
            endif()
        endif()
    endif()
    message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
endif()

# if(MSVC)
#     target_compile_definitions(xcanvas PUBLIC -DNOMINMAX)
#     target_compile_options(xcanvas PUBLIC /DGUID_WINDOWS /MP /bigobj)
#     target_compile_options(xcanvas PUBLIC /wd4251 /wd 4996)
#     # Allows to compile in debug without requiring each dependencies to
#     # be compiled in debug
#     if(${U_CMAKE_BUILD_TYPE} MATCHES DEBUG)
#         set(CMAKE_CXX_FLAGS_DEBUG "/DNDEBUG /MD /Zi /Ob0 /Od /RTC1")
#         message(STATUS "Overriding CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
#     endif()
# endif()

# Installation
# ============

set(XCANVAS_CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE STRING "install path for xcanvasConfig.cmake")

install(TARGETS xcanvas
        EXPORT ${PROJECT_NAME}-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xcanvas)

# Makes the project importable from the build directory
export(EXPORT ${PROJECT_NAME}-targets
       FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

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
