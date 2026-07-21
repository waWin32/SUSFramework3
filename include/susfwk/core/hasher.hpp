// susfwk-core-hasher.hpp
//
/*
* =====---------------- hasher.hpp - common function to hashing ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====-------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_CORE_HASHER_
#define _SUSFWK_CORE_HASHER_

#include "susfwk/utils/core.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// Simple hashing fnv_1a
	class hash_fnv_1a {
	public:
		sus::usize operator()(const void* data, sus::usize n) const {
			constexpr sus::usize FNV_PRIME = 1099511628211ULL;
			sus::usize hash = 14695981039346656037ULL;
			if (n > 16) {
				for (; reinterpret_cast<sus::usize>(data) & 7; --n) {
					hash ^= *static_cast<const sus::u8*>(data);
					hash *= FNV_PRIME;
					data = static_cast<const sus::u8*>(data) + 1;
				}
				for (sus::usize i = n >> 3; i; --i) {
					sus::u64 word = *static_cast<const sus::u64*>(data);
					hash ^= (word >> 0) & 0xff;
					hash *= FNV_PRIME;
					hash ^= (word >> 8) & 0xff;
					hash *= FNV_PRIME;
					hash ^= (word >> 16) & 0xff;
					hash *= FNV_PRIME;
					hash ^= (word >> 24) & 0xff;
					hash *= FNV_PRIME;
					hash ^= (word >> 32) & 0xff;
					hash *= FNV_PRIME;
					hash ^= (word >> 40) & 0xff;
					hash *= FNV_PRIME;
					hash ^= (word >> 48) & 0xff;
					hash *= FNV_PRIME;
					hash ^= (word >> 56) & 0xff;
					hash *= FNV_PRIME;
					data = static_cast<const sus::u64*>(data) + 1;
				}
				n &= 7;
			}
			for (; n; --n) {
				hash ^= *static_cast<const sus::u8*>(data);
				hash *= FNV_PRIME;
				data = static_cast<const sus::u8*>(data) + 1;
			}
			return hash;
		}
	};

	// -------------------------------------------------------------------

	// Is the class a hasher
	template<class H, typename T>
	struct is_hasher : sus::bool_constant <requires(H h, T t) {
		{ h(t) } -> sus::same_t<sus::usize>;
	}> {};
	template<class H, typename T>
	inline constexpr bool is_hasher_v = is_hasher<H, T>::v;
	template<class H, typename T>
	concept hasher_t = is_hasher_v<H, T>;

	// The base case is when the hash function is not declared
	template<typename T>
	struct hasher {
		static_assert(false, "The hash for the current type is not supported");
		constexpr sus::usize operator()(const T&) const { return 0; }
	};
	
	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_CORE_HASHER_ */