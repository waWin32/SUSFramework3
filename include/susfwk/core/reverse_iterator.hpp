// susfwk-core-reverse_iterator.hpp
//
/*
* =====---------------- reverse_iterator.hpp - Iterator modifier - reverse ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====------------------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_CORE_REVERSE_ITERATOR_
#define _SUSFWK_CORE_REVERSE_ITERATOR_

#include "susfwk/utils/core.hpp"
#include "susfwk/utils/iterator.hpp"
#include "susfwk/utils/container.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// Reverse iterator for bidirectional iterators
	template<sus::iterator_bidirectional_t I>
	class reverse_iterator_bidirectional : public sus::iterator_bidirectional_father<reverse_iterator_bidirectional<I>> {
	private:
		I base_{};
	protected:
		constexpr decltype(auto) dereference() const {
			auto tmp = base_;
			--tmp;
			return *tmp;
		}
		constexpr void increment() { --base_; }
		constexpr void decrement() { ++base_; }
	public:
		SUS_ATTRIB_PURE constexpr bool operator==(const reverse_iterator_bidirectional& other) const { return base_ == other.base_; }
		SUS_ATTRIB_PURE constexpr bool operator!=(const reverse_iterator_bidirectional& other) const { return base_ != other.base_; }
	public:
		constexpr operator I() const { return base_; }
		reverse_iterator_bidirectional() = default;
		reverse_iterator_bidirectional(const I& i) : base_(i) {}
		reverse_iterator_bidirectional(I&& i) : base_(sus::move(i)) {}
	};
	// Reverse iterator for random-access iterators
	template<sus::iterator_random_access_t I>
	class reverse_iterator_random_access : public iterator_random_access_father<reverse_iterator_random_access<I>> {
	private:
		I base_{};
	protected:
		constexpr operator I() const { return base_; }
		constexpr decltype(auto) dereference() const {
			decltype(auto) tmp = base_;
			--tmp;
			return *tmp;
		}
		constexpr void increment() { --base_; }
		constexpr void decrement() { ++base_; }
		constexpr decltype(auto) distance_to(const reverse_iterator_random_access& other) const { return other.base_ - base_; }
		constexpr void advance(sus::isize n) { base_ -= n; }
		constexpr decltype(auto) dereference_at(sus::usize i) const { return base_[-static_cast<sus::isize>(i) - 1]; }
	public:
		SUS_ATTRIB_PURE constexpr bool operator==(const reverse_iterator_random_access& other) const { return base_ == other.base_; }
		SUS_ATTRIB_PURE constexpr bool operator!=(const reverse_iterator_random_access& other) const { return base_ != other.base_; }
		SUS_ATTRIB_PURE constexpr bool operator>(const reverse_iterator_random_access& other) const { return base_ < other.base_; }
		SUS_ATTRIB_PURE constexpr bool operator>=(const reverse_iterator_random_access& other) const { return base_ <= other.base_; }
		SUS_ATTRIB_PURE constexpr bool operator<(const reverse_iterator_random_access& other) const { return base_ > other.base_; }
		SUS_ATTRIB_PURE constexpr bool operator<=(const reverse_iterator_random_access& other) const { return base_ >= other.base_; }
	public:
		constexpr operator I() const { return base_; }
		reverse_iterator_random_access() = default;
		reverse_iterator_random_access(const I& i) : base_(i) {}
		reverse_iterator_random_access(I&& i) : base_(sus::move(i)) {}
	};

	// -------------------------------------------------------------------

	// Make an iterator modifier
	template<sus::iterator_bidirectional_t I>
	constexpr reverse_iterator_bidirectional<I> make_reverse_iterator(I it) { return reverse_iterator_bidirectional<I>(sus::move(it)); }
	// Make an iterator modifier
	template<sus::iterator_random_access_t I>
	constexpr reverse_iterator_random_access<I> make_reverse_iterator(I it) { return reverse_iterator_random_access<I>(sus::move(it)); }

	// -------------------------------------------------------------------

	// Get the reverse iterator to the beginning
	template<sus::container_t T>
	constexpr auto rbegin(const T& x) { return sus::make_reverse_iterator(end(x)); }
	// Get the reverse iterator at the end
	template<sus::container_t T>
	constexpr auto rend(const T& x) { return sus::make_reverse_iterator(begin(x)); }

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_CORE_REVERSE_ITERATOR_ */
