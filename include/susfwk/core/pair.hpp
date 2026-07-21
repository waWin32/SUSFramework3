// susfwk-core-pair.hpp
//
/*
* =====---------------- pair.hpp - pair of value structure ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====--------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_CORE_PAIR_
#define _SUSFWK_CORE_PAIR_

#include "susfwk/utils/core.hpp"

namespace sus {

	// -------------------------------------------------------------------

	template<typename T1, typename T2>
	struct pair {
		using first_type = T1;
		using second_type = T2;
		T1 first;
		T2 second;
	public:
		template<typename U1, typename U2>
		constexpr pair(U1&& f, U2&& s) : first(sus::forward<U1>(f)), second(sus::forward<U2>(s)) {};
		constexpr pair() = default;
		constexpr ~pair() = default;
		template<typename U1, typename U2>
		constexpr pair(const pair<U1, U2>& other) : first(other.first), second(other.second) {}
		template<typename U1, typename U2>
		constexpr pair(pair<U1, U2>&& other) : first(sus::move(other.first)), second(sus::move(other.second)) {}

		constexpr friend bool operator==(const pair& lhs, const pair& rhs) {
			return lhs.first == rhs.first && lhs.second == rhs.second;
		}
		constexpr friend bool operator!=(const pair& lhs, const pair& rhs) {
			return lhs.first != rhs.first || lhs.second != rhs.second;
		}
		constexpr friend bool operator>(const pair& lhs, const pair& rhs) {
			return lhs.first > rhs.first || (lhs.first == rhs.first && lhs.second > rhs.second);
		}
		constexpr friend bool operator>=(const pair& lhs, const pair& rhs) {
			return !(lhs < rhs);
		}
		constexpr friend bool operator<(const pair& lhs, const pair& rhs) {
			return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second);
		}
		constexpr friend bool operator<=(const pair& lhs, const pair& rhs) {
			return !(lhs > rhs);
		}
	};

	template<typename T1, typename T2>
	pair(const T1&, const T2&) -> pair<T1, T2>;

	// -------------------------------------------------------------------

	// Is the type a pair
	template<typename T>
	struct is_pair : sus::bool_constant <requires(T& t) {
		{ t.first };
		{ t.second };
	} > {};
	template<typename T>
	inline constexpr bool is_pair_v = is_pair<T>::v;
	template<typename T>
	concept pair_t = is_pair_v<T>;

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_CORE_PAIR_ */
