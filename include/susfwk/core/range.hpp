// susfwk-core-range.hpp
//
/*
* =====---------------- range.hpp - abstraction over a range ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====----------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_CORE_RANGE_
#define _SUSFWK_CORE_RANGE_

#include "susfwk/utils/core.hpp"
#include "susfwk/utils/container.hpp"
#include "susfwk/utils/numeric.hpp"
#include "susfwk/utils/iterator.hpp"

namespace sus {

	// -------------------------------------------------------------------
	
	// Template for the range
	template<typename I, typename = void>
	class range { static_assert(false, "The range accepted an invalid iterator"); };
	// Range for non-random access iterators
	template<typename I>
	class range<I, sus::enable_if_t<sus::is_iterator_forward_v<I> && !sus::is_iterator_random_access_v<I> && !sus::is_pointer_v<I>, void>> {
	public: using iterator = I;
	private:
		iterator first_ = nullptr;
		iterator last_ = nullptr;
		sus::make_unsigned_t<sus::iterator_difference_t<iterator>> size_ = 0;
	public:
		// Get the number of range elements
		SUS_ATTRIB_PURE inline friend auto size(const range& r) { return r.size_; }
		// Get an pointer at the beginning of the range
		SUS_ATTRIB_PURE inline friend auto data(const range& r) { return r.first_; }
		// Get an iterator at the beginning of the range
		SUS_ATTRIB_PURE inline friend auto begin(const range& r) { return r.first_; }
		// Get the iterator at the end of the range
		SUS_ATTRIB_PURE inline friend auto end(const range& r) { return r.last_; }
	public:
		constexpr range() = default;
		constexpr range(iterator f, sus::make_unsigned_t<sus::iterator_difference_t<iterator>> n) : first_(f), last_(f), size_(n) { for (auto i = n; i; --i) ++last_; }
		constexpr range(iterator f, iterator l, sus::make_unsigned_t<sus::iterator_difference_t<iterator>> n) : first_(f), last_(l), size_(n) {}
		constexpr range(iterator f, iterator l) : first_(f), last_(l) { for (auto i = f; i != l; ++size_) ++i; }
		template<sus::container_t C> requires(sus::is_same_v<sus::iterator_value_t<I>, sus::iterator_value_t<sus::iterator_t<C>>>)
		constexpr range(const C& c) : first_(begin(c)), last_(end(c)), size_(size(c)) {}
	};
	// Range for random access iterators
	template<typename I>
	class range<I, sus::enable_if_t<sus::is_iterator_random_access_v<I> && !sus::is_pointer_v<I>, void>> {
	public: using iterator = I;
	private:
		iterator first_ = nullptr;
		iterator last_ = nullptr;
	public:
		// Get the number of range elements
		SUS_ATTRIB_PURE inline friend auto size(const range& r) { return r.last_ - r.first_; }
		// Get an pointer at the beginning of the range
		SUS_ATTRIB_PURE inline friend auto data(const range& r) { return r.first_; }
		// Get an iterator at the beginning of the range
		SUS_ATTRIB_PURE inline friend auto begin(const range& r) { return r.first_; }
		// Get the iterator at the end of the range
		SUS_ATTRIB_PURE inline friend auto end(const range& r) { return r.last_; }
	public:
		constexpr range() = default;
		constexpr range(iterator f, sus::make_unsigned_t<sus::iterator_difference_t<iterator>> n) : first_(f), last_(f + n) {}
		constexpr range(iterator f, iterator l, sus::make_unsigned_t<sus::iterator_difference_t<iterator>>) : first_(f), last_(l) {}
		constexpr range(iterator f, iterator l) : first_(f), last_(l) {}
		template<sus::container_t C> requires(sus::is_same_v<sus::iterator_value_t<I>, sus::iterator_value_t<sus::iterator_t<C>>>)
		constexpr range(const C& c) : first_(begin(c)), last_(end(c)) {}
	};
	// Range for pointer iterators
	template<typename I >
	class range<I, sus::enable_if_t<sus::is_pointer_v<I>, void>> {
	public: using iterator = I;
	private:
		iterator data_ = nullptr;
		sus::make_unsigned_t<sus::iterator_difference_t<iterator>> size_ = 0;
	public:
		// Get the number of range elements
		SUS_ATTRIB_PURE inline friend auto size(const range& r) { return r.size_; }
		// Get an pointer at the beginning of the range
		SUS_ATTRIB_PURE inline friend auto data(const range& r) { return r.data_; }
		// Get an iterator at the beginning of the range
		SUS_ATTRIB_PURE inline friend auto begin(const range& r) { return r.data_; }
		// Get the iterator at the end of the range
		SUS_ATTRIB_PURE inline friend auto end(const range& r) { return r.data_ + r.size_; }
	public:
		constexpr range() = default;
		constexpr range(iterator f, sus::make_unsigned_t<sus::iterator_difference_t<iterator>> n) : data_(f), size_(n) {}
		constexpr range(iterator f, iterator, sus::make_unsigned_t<sus::iterator_difference_t<iterator>> n) : data_(f), size_(n) {}
		constexpr range(iterator f, iterator l) : data_(f), size_(l - f) {}
		template<sus::container_t C> requires(sus::is_same_v<sus::iterator_value_t<I>, sus::iterator_value_t<sus::iterator_t<C>>>)
		constexpr range(const C& c) : data_(data(c)), size_(size(c)) {}
	};

	// -------------------------------------------------------------------

	// Get the range of the container
	template<sus::container_t C>
	range(const C& c) -> range<sus::iterator<C>>;
	// Get the range from iterators and size
	template<sus::iterator_forward_t I>
	range(I f, sus::make_unsigned_t<sus::iterator_difference_t<I>> n) -> range<I>;
	// Get the range from iterators
	template<sus::iterator_forward_t I>
	range(I f, I l, sus::make_unsigned_t<sus::iterator_difference_t<I>> n) -> range<I>;
	// Get the range from iterators
	template<sus::iterator_forward_t I>
	range(I f, I l) -> range<I>;

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_CORE_RANGE_ */
