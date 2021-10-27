/******************************************************************************
* Copyright (c) 2021, Martin Renou                                            *
*                                                                             *
* Distributed under the terms of the BSD 3-Clause License.                    *
*                                                                             *
* The full license is in the file LICENSE, distributed with this software.    *
*******************************************************************************/

#ifndef XCANVAS_CONFIG_HPP
#define XCANVAS_CONFIG_HPP

#include <string>

// Visual C++ declspec macros
#ifdef _WIN32
    #ifdef XCANVAS_EXPORTS
        #define XCANVAS_API __declspec(dllexport)
    #else
        #define XCANVAS_API __declspec(dllimport)
    #endif
#else
    #define XCANVAS_API
#endif

// Project version
#define XCANVAS_VERSION_MAJOR 0
#define XCANVAS_VERSION_MINOR 2
#define XCANVAS_VERSION_PATCH 2

// Binary version
#define XCANVAS_BINARY_CURRENT 1
#define XCANVAS_BINARY_REVISION 0
#define XCANVAS_BINARY_AGE 0

// Semver requirement for jupyter-canvas
#define XJUPYTER_CANVAS_VERSION_MAJOR 0
#define XJUPYTER_CANVAS_VERSION_MINOR 9
#define XJUPYTER_CANVAS_VERSION_PATCH 1

// Composing the version strings from major, minor and patch
#define XCANVAS_CONCATENATE(A, B) XCANVAS_CONCATENATE_IMPL(A, B)
#define XCANVAS_CONCATENATE_IMPL(A, B) A##B
#define XCANVAS_STRINGIFY(a) XCANVAS_STRINGIFY_IMPL(a)
#define XCANVAS_STRINGIFY_IMPL(a) #a

#define XJUPYTER_CANVAS_VERSION XCANVAS_STRINGIFY(XCANVAS_CONCATENATE(XJUPYTER_CANVAS_VERSION_MAJOR,   \
                             XCANVAS_CONCATENATE(.,XCANVAS_CONCATENATE(XJUPYTER_CANVAS_VERSION_MINOR,   \
                                                  XCANVAS_CONCATENATE(.,XJUPYTER_CANVAS_VERSION_PATCH)))))

inline std::string jupyter_canvas_semver()
{
    return std::string("^") + XJUPYTER_CANVAS_VERSION;
}

#ifdef __CLING__
#include "xcanvas_config_cling.hpp"
#endif

#endif
