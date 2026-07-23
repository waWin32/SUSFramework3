// susfwk-core-typeops-compare.hpp
//
/*
* =====---------------- compare.hpp - comparison functions for types ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====------------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_CORE_TYPEOPS_COMPARE_
#define _SUSFWK_CORE_TYPEOPS_COMPARE_

#include "susfwk/utils/core.hpp"
#include "susfwk/core/algorithms.hpp"
#include "susfwk/core/comparator.hpp"

// -------------------------------------------------------------------

#include "susfwk/utils/numeric.hpp"

namespace sus {

	// Optimized comparison version for numbers
	template<sus::integral_t T> requires(sizeof(T) <= 4)
		constexpr sus::i32 compare(const T& lhs, const T& rhs) {
		return lhs - rhs;
	}
	// Comparisons of primitive types
	template<sus::scalar_t T>
	constexpr sus::i32 compare(const T& lhs, const T& rhs) {
		return lhs != rhs ? (lhs > rhs ? 1 : -1) : 0;
	}

}

// -------------------------------------------------------------------

#include "susfwk/core/pair.hpp"

namespace sus {

	// Comparing a pair of types
	template<typename T1, typename T2>
	constexpr sus::i32 compare(const sus::pair<T1, T2>& lhs, const sus::pair<T1, T2>& rhs) {
		sus::i32 cmp = sus::compare(lhs.first, rhs.first);
		if (cmp) return cmp;
		return sus::compare(lhs.second, rhs.second);
	}

}

// -------------------------------------------------------------------

#endif /* !_SUSFWK_CORE_TYPEOPS_COMPARE_ */
