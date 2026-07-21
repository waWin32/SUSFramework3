// susfwk-core-platform.h
//
/*
* =====---------------- platform.h - connecting system libraries ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====--------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_CORE_PLATFORM_
#define _SUSFWK_CORE_PLATFORM_

#include "susfwk/utils/types.hpp"

#if defined(SUS_SYSTEM_WINDOWS)

#define WIN32_EXTRA_LEAN
#define NOMINMAX
#define _WIN32_WINNT 0x0A00
#define NOCOMM
#include <windows.h>

#elif defined(SUS_SYSTEM_LINUX)

#elif defined(SUS_SYSTEM_MACOS)

#endif /* !SUS_SYSTEM */

#endif /* !_SUSFWK_CORE_PLATFORM_ */