// susfwk-core-typeops-hasher.hpp
//
/*
* =====---------------- hasher.hpp - the function of getting a hash from types ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====----------------------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_CORE_TYPEOPS_HASHER_
#define _SUSFWK_CORE_TYPEOPS_HASHER_

#include "susfwk/utils/core.hpp"
#include "susfwk/core/algorithms.hpp"
#include "susfwk/core/hasher.hpp"

// -------------------------------------------------------------------

#include "susfwk/utils/numeric.hpp"

namespace sus {

	// A hash of a simple type
	template<sus::scalar_t T>
	struct hasher<T> {
		inline sus::usize operator()(const T& x) const {
			hash_fnv_1a fnv{};
			return fnv(&x, sizeof(T));
		}
	};
	// Simple type hash - optimization for the hash type
	template<sus::scalar_t T> requires (sizeof(T) <= sizeof(sus::usize))
		struct hasher<T> {
		constexpr sus::usize operator()(const T& x) const {
			return static_cast<sus::usize>(x);
		}
	};

}

// -------------------------------------------------------------------

#include "susfwk/core/pair.hpp"

namespace sus {

	// Hash the pair of types
	template<typename T1, typename T2>
	struct hasher<sus::pair<T1, T2>> {
		inline sus::usize operator()(const sus::pair<T1, T2>& x) const {
			sus::usize h1 = hasher<T1>()(x.first);
			sus::usize h2 = hasher<T2>()(x.second);
			return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
		}
	};

}

// -------------------------------------------------------------------

#endif /* !_SUSFWK_CORE_TYPEOPS_HASHER_ */

