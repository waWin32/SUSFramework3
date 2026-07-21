// susfwk-utils-array.hpp
//
/*
* =====---------------- array.hpp - structure for working with a static array ----------------=====
*
* Part of ths SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====---------------------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_CORE_ARRAY_
#define _SUSFWK_CORE_ARRAY_

#include "susfwk/utils/core.hpp"
#include "susfwk/utils/iterator.hpp"
#include "susfwk/utils/container.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// Static array
	template<typename T, sus::usize N>
	class array {
	public: using iterator = T*;
	private:
		sus::iterator_value_t<iterator> data_[N];
	public:
		SUS_ATTRIB_PURE constexpr friend auto data(const array& arr) { return (sus::iterator_pointer_t<iterator>)arr.data_; }
		SUS_ATTRIB_PURE constexpr friend sus::usize size(const array&) { return N; }
		SUS_ATTRIB_PURE constexpr friend iterator begin(const array& arr) { return data(arr); }
		SUS_ATTRIB_PURE constexpr friend iterator end(const array& arr) { return data(arr) + N; }
		constexpr sus::iterator_reference_t<iterator> operator[](sus::usize index) const { assert(index < N); return const_cast<T&>(data_[index]); }
	public:
		constexpr array() = default;
		template<sus::container_t C> requires (sus::is_iterator_forward_v<sus::iterator_t<C>> && sus::is_same_v<sus::iterator_value_t<sus::iterator_t<C>>, sus::remove_cvref_t<T>>)
		constexpr array(const C& c) { auto curr = begin(); for (auto first = begin(c); first != end(c); ++first, ++curr) ::new(&*first) T(*curr); }
	};

	// -------------------------------------------------------------------

	// Check if the type is an array
	template<typename T>
	struct is_array : sus::bool_constant<sus::is_container_v<T>&& sus::is_iterator_random_access_v<sus::iterator<T>>> {};
	template<typename T>
	inline constexpr bool is_array_v = is_array<T>::v;
	template<typename T>
	concept array_t = is_array_v<T>;

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_CORE_ARRAY_ */
