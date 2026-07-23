// susfwk-utils-mathdef.hpp
//
/*
* =====---------------- mathdef.hpp - basic math constant ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====-------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_UTILS_MATHDEF_
#define _SUSFWK_UTILS_MATHDEF_

#include "susfwk/utils/types.hpp"
#include "susfwk/utils/numeric.hpp"

namespace sus::math {

	// -------------------------------------------------------------------

	// The constant is the number PI
	template<sus::floating_t T>
	inline constexpr T pi = static_cast<T>(3.141592653589793238462643383279502884);
	// The constant is the Euler number
	template<sus::floating_t T>
	inline constexpr T e = static_cast<T>(2.718281828459045235360287471352662498);
	// The sqrt of two
	template<sus::floating_t T>
	inline constexpr T sqrt2 = static_cast<T>(1.414213562373095048801688724209698079);
	// The sqrt of three
	template<sus::floating_t T>
	inline constexpr T sqrt3 = static_cast<T>(1.732050807568877293527446341505872367);
	// The sqrt of pi
	template<sus::floating_t T>
	inline constexpr T sqrt_pi = static_cast<T>(1.772453850905516027298167483341145183);
	// The magic rounding constant
	template<sus::floating_t T>
	inline constexpr T reduction_magic_x86 = sus::is_same_v<T, sus::f32> ? 12582912.0f : 6755399441055744.0;

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_UTILS_MATHDEF_ */
