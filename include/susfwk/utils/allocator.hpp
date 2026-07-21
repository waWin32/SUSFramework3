// susfwk-utils-allocator.hpp
//
/*
* =====---------------- allocator.hpp - memory allocator template ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_UTILS_ALLOCATOR_
#define _SUSFWK_UTILS_ALLOCATOR_

#include "susfwk/utils/core.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// A primitive allocator - only memory allocation
	template<typename Derived>
	class allocator_simple_father {
	public:
		SUS_ATTRIB_NODISCARD static constexpr void* malloc(sus::usize size) {
			return Derived::allocate(size);
		}
	};
	// Basic allocator - memory allocation and deallocation
	template<typename Derived>
	class allocator_base_father : public sus::allocator_simple_father<Derived> {
	public:
		static constexpr void free(void* block) {
			Derived::deallocate(block);
		}
	};
	// Full allocator - allocation, deallocation, and implementation of memory
	template<typename Derived>
	class allocator_full_father : public sus::allocator_base_father<Derived> {
	public:
		SUS_ATTRIB_NODISCARD static constexpr void* realloc(void* block, sus::usize size) {
			return Derived::reallocate(block, size);
		}
	};

	// -------------------------------------------------------------------

	// Allocator - memory allocation only
	template<typename A>
	struct is_allocator_simple : sus::bool_constant<requires(sus::usize s) {
		{ A::malloc(s) } -> sus::same_t<void*>;
	}> {};
	template<typename A>
	inline constexpr bool is_allocator_simple_v = is_allocator_simple<A>::v;
	template<typename A>
	concept allocator_simple_t = is_allocator_simple_v<A>;
	// Allocator - allocation and deallocation of memory
	template<typename A>
	struct is_allocator_base : sus::bool_constant<allocator_simple_t<A> && requires(void* p) {
		{ A::free(p) } -> sus::same_t<void>;
	}> {};
	template<typename A>
	inline constexpr bool is_allocator_base_v = is_allocator_base<A>::v;
	template<typename A>
	concept allocator_base_t = is_allocator_base_v<A>;
	// Allocator - allocation, deallocation, and implementation of memory
	template<typename A>
	struct is_allocator_full : sus::bool_constant <allocator_base_t<A> && requires(void* p, sus::usize s) {
		{ A::realloc(p, s) } -> sus::same_t<void*>;
	}> {};
	template<typename A>
	inline constexpr bool is_allocator_full_v = is_allocator_full<A>::v;
	template<typename A>
	concept allocator_full_t = is_allocator_full_v<A>;

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_UTILS_ALLOCATOR_ */
