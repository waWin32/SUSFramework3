// susfwk-utils-core.hpp
//
/*
* =====---------------- core.hpp - basic utilities for C++ ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====--------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_UTILS_CORE_
#define _SUSFWK_UTILS_CORE_

#include "susfwk/utils/types.hpp"
#include "susfwk/utils/type_traits.hpp"

namespace sus {
	
	// -------------------------------------------------------------------

	template<typename T>
	SUS_ATTRIB_NODISCARD constexpr sus::remove_reference<T>::t&& move(T&& t) { return static_cast<sus::remove_reference<T>::t&&>(t); }
	template<typename T>
	SUS_ATTRIB_NODISCARD constexpr T&& forward(typename sus::remove_reference<T>::t& t) { return static_cast<T&&>(t); }
	template<typename T>
	SUS_ATTRIB_NODISCARD constexpr T&& forward(typename sus::remove_reference<T>::t&& t) { return static_cast<T&&>(t); }
	template<typename T>
	constexpr void swap(T& a, T& b) { T t = sus::move(a); a = sus::move(b); b = sus::move(t); }

	// -------------------------------------------------------------------

}

constexpr void* operator new(sus::usize, void* ptr) noexcept { return ptr; }
constexpr void* operator new[](sus::usize, void* ptr) noexcept { return ptr; }
constexpr void operator delete(void*, void*) noexcept {}
constexpr void operator delete[](void*, void*) noexcept {}

#endif /* !_SUSFWK_UTILS_CORE_ */
