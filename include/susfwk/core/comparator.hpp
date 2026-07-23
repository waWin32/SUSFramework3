// sus-core-comparator.hpp
//
/*
* =====---------------- comparator.hpp - Definition of comparison ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====--------------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_CORE_COMPARATOR_
#define _SUSFWK_CORE_COMPARATOR_

#include "susfwk/utils/core.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// A comparison concept for types
	template<class T>
	struct is_comparable : sus::bool_constant<requires(const T & a, const T & b) {
		{ compare(a, b) } -> sus::same_t<sus::i32>;
	}> {};
	template<class T>
	inline constexpr bool is_comparable_v = is_comparable<T>::v;
	template<class T>
	concept comparable_t = is_comparable_v<T>;

	// -------------------------------------------------------------------

	template<comparable_t Derived>
	constexpr bool operator==(const Derived& lhs, const Derived& rhs) { return compare(lhs, rhs) == 0; }
	template<comparable_t Derived>
	constexpr bool operator!=(const Derived& lhs, const Derived& rhs) { return compare(lhs, rhs) != 0; }
	template<comparable_t Derived>
	constexpr bool operator>(const Derived& lhs, const Derived& rhs) { return compare(lhs, rhs) > 0; }
	template<comparable_t Derived>
	constexpr bool operator>=(const Derived& lhs, const Derived& rhs) { return compare(lhs, rhs) >= 0; }
	template<comparable_t Derived>
	constexpr bool operator<(const Derived& lhs, const Derived& rhs) { return compare(lhs, rhs) < 0; }
	template<comparable_t Derived>
	constexpr bool operator<=(const Derived& lhs, const Derived& rhs) { return compare(lhs, rhs) <= 0; }

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_CORE_COMPARATOR_ */