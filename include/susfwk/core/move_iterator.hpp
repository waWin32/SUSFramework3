// susfwk-core-move_iterator.hpp
//
/*
* =====---------------- move_iterator.hpp - Iterator modifier - move ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====------------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_CORE_MOVE_ITERATOR_
#define _SUSFWK_CORE_MOVE_ITERATOR_

#include "susfwk/utils/core.hpp"
#include "susfwk/utils/iterator.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// The iterator modifier is always a transfer
	template<sus::iterator_forward_t I>
	class move_iterator : public sus::iterator_forward_father<move_iterator<I>> {
	private:
		I base{};
	protected:
		constexpr decltype(auto) dereference() const { return sus::move(*base); }
		constexpr void increment() { ++base; }
	public:
		SUS_ATTRIB_PURE constexpr bool operator==(const move_iterator& other) const { return base == other.base; }
		SUS_ATTRIB_PURE constexpr bool operator!=(const move_iterator& other) const { return base != other.base; }
	public:
		constexpr operator I() const { return base; }
		move_iterator() = default;
		move_iterator(const I& i) : base(i) {}
		move_iterator(I&& i) : base(sus::move(i)) {}
	};

	// -------------------------------------------------------------------

	// Make an iterator modifier
	template<sus::iterator_forward_t I>
	move_iterator(I&& it) -> move_iterator<I>;

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_CORE_MOVE_ITERATOR_ */
