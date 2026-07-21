// susfwk-arch-exit.hpp
//
/*
* =====---------------- exit.hpp - program exit functions ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====-------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_ARCH_EXIT_
#define _SUSFWK_ARCH_EXIT_

#include "susfwk/utils/types.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// Emergency exit from the program
	SUS_ATTRIB_NORETURN void abort(const sus::chr* msg = nullptr);
	// Safe exit from the program
	SUS_ATTRIB_NORETURN void exit(sus::i32 code = 1);
	#if defined(SUS_DEBUG)
	// Assert is enabled
	#define assert(expression) if (!(expression)) sus::abort("assert in " __FILE__ " : " SUS_TOSTRING(__LINE__) "\n")
	#else
	// Assert is disabled
	#define assert(expression)
	#endif // !SUS_DEBUG

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_ARCH_EXIT_ */