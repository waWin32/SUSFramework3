// susfwk-arch-incasimd.hpp
//
/*
* =====---------------- incasimd.hpp - connecting a shared SIMD for the architecture ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====-----------------------------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_ARCH_INCASIMD_
#define _SUSFWK_ARCH_INCASIMD_

#include "susfwk/utils/types.hpp"

// -------------------------------------------------------------------

#if defined(SUS_ARCH_X86)
#include "emmintrin.h"
#include <smmintrin.h>
#elif defined(SUS_ARCH_ARM)
#include "arm_neon.h"
#elif defined(SUS_ARCH_RISCV)
#include "riscv_vector.h"
#endif /* !SUS_ARCH */

// -------------------------------------------------------------------

#endif /* !_SUSFWK_ARCH_INCASIMD_ */
