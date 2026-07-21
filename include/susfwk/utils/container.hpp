// susfwk-utils-container.hpp
//
/*
* =====---------------- container.hpp - the general concept of the container ----------------=====
*
* Part of ths SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====--------------------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_UTILS_CONTAINER_
#define _SUSFWK_UTILS_CONTAINER_

#include "susfwk/utils/core.hpp"
#include "susfwk/utils/iterator.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// Is the type a container
	template<typename T>
	struct is_container : sus::bool_constant<requires(T& t) {
		{ data(t) };
		{ begin(t) };
		{ end(t) };
		{ size(t) };
	}> {};
	template<typename T>
	inline constexpr bool is_container_v = is_container<T>::v;
	template<typename T>
	concept container_t = is_container_v<T>;

	// Check if the container is empty
	template<sus::container_t T>
	constexpr bool empty(const T& x) { return !size(x); }

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_UTILS_CONTAINER_ */
