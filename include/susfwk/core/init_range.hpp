// susfwk-core-init_range.hpp
//
/*
* =====---------------- init_range.hpp - Initializing array for containers ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====------------------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_CORE_INIT_RANGE_
#define _SUSFWK_CORE_INIT_RANGE_

#include "susfwk/utils/core.hpp"
#include "susfwk/utils/container.hpp"
#include "susfwk/utils/iterator.hpp"
#include "susfwk/utils/initializer_list.hpp"
#include "susfwk/core/move_iterator.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// Initializing a range with values that it owns
	template<typename T>
	class init_range {
	public: using iterator = T*;
	private:
		const iterator first_ = nullptr;
		const iterator last_ = nullptr;
	public:
		// Get the number of range elements
		SUS_ATTRIB_PURE inline friend auto size(const init_range& r) { return r.last_ - r.first_; }
		// Get an iterator at the beginning of the range
		SUS_ATTRIB_PURE inline friend auto begin(const init_range& r) { return sus::move_iterator(r.first_); }
		// Get the iterator at the end of the range
		SUS_ATTRIB_PURE inline friend auto end(const init_range& r) { return sus::move_iterator(r.last_); }
	public:
		constexpr init_range() = default;
		constexpr init_range(std::initializer_list<T> il) : first_(const_cast<iterator>(il.begin())), last_(const_cast<iterator>(il.end())) {}
	};

	// -------------------------------------------------------------------

	template<typename T>
	init_range(std::initializer_list<T>) -> init_range<T>;

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_CORE_INIT_RANGE_ */
