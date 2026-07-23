// susfwk-core-string.hpp
//
/*
* =====---------------- string.hpp - Dynamic string ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====------------------------------------------------------------=====
*/
#ifndef _SUSFWK_CORE_STRING_
#define _SUSFWK_CORE_STRING_

#include "susfwk/utils/core.hpp"
#include "susfwk/utils/container.hpp"
#include "susfwk/arch/memops.h"
#include "susfwk/utils/iterator.hpp"
#include "susfwk/utils/allocator.hpp"
#include "susfwk/arch/strops.hpp"
#include "susfwk/core/array.hpp"
#include "susfwk/core/system_allocator.hpp"
#include "susfwk/core/vector.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// An array of characters ending in zero
	template<sus::char_t T, auto Policy = optimization_policy{}, sus::allocator_base_t Alloc = sus::sysheap_allocator >
	class string {
	public: using iterator = T*;
	private:
		sus::vector<T, Policy, Alloc> base_;
	public:
		// Get the container capacity
		SUS_ATTRIB_CONST constexpr sus::usize capacity() const { return base_.capacity(); }
		// Get a pointer to the beginning of the data
		SUS_ATTRIB_PURE constexpr friend auto data(const string& str) { return data(str.base_); }
		// Get the length of the string
		SUS_ATTRIB_CONST constexpr friend auto size(const string& str) { return size(str.base_) - 1; }
		// Get an iterator for the first character of a string
		SUS_ATTRIB_PURE constexpr friend auto begin(const string& str) { return begin(str.base_); }
		// Get an iterator for the last character of a string
		SUS_ATTRIB_PURE constexpr friend auto end(const string& str) { return end(str.base_) - 1; }
		// Implicit cast to C-string
		SUS_ATTRIB_PURE constexpr operator auto () const { return data(*this); }
	public:
		// Reserve data size in the array
		inline void reserve(sus::usize size) { base_.reserve(size + 1); }
		// Compress the array - smart compess
		inline void compress() { base_.compress(); }
		// Compress the array before size
		inline void shrink_to_fit() { base_.shrink_to_fit(); }
	public:
		// Get a string character
		SUS_ATTRIB_PURE constexpr auto& operator[](sus::usize index) const { assert(index < size(*this)); return data(*this)[index]; }
		// Insert multiple characters
		template<bool auto_resize = true>
		inline iterator insert(iterator i, sus::add_const_t<sus::iterator_reference_t<iterator>> ch, sus::usize n) {
			assert(ch != sus::iterator_value_t<iterator>(0));
			return base_.template insert<auto_resize>(i, ch, n);
		}
		// Insert multiple null
		template<bool auto_resize = true>
		inline iterator insert_uninitialized(iterator i, sus::usize n) {
			return base_.template insert_uninitialized<auto_resize>(i, n);
		}
		// Insert one character
		template<bool auto_resize = true>
		inline iterator insert(iterator i, sus::add_const_t<sus::iterator_reference_t<iterator>> ch) {
			assert(ch != sus::iterator_value_t<iterator>(0));
			return base_.template insert<auto_resize>(i, ch);
		}
		// Insert a range in a string
		template<bool auto_resize = true, sus::container_t C> requires(sus::is_same_v<sus::iterator_value_t<sus::iterator_t<C>>, T>)
		inline iterator insert(iterator i, const C& c) {
			return base_.template insert<auto_resize>(i, c);
		}

		// Insert null at the end of the string
		template<bool auto_resize = true>
		inline void push_uninitialized(sus::usize n) { insert_uninitialized<auto_resize>(end(*this), n); }
		// Insert characters at the end of the string
		template<bool auto_resize = true>
		inline void push(sus::add_const_t<sus::iterator_reference_t<iterator>> c, sus::usize n) { insert<auto_resize>(end(*this), c, n); }
		// Insert one character at the end of the string
		template<bool auto_resize = true>
		inline void push(sus::add_const_t<sus::iterator_reference_t<iterator>> c) { insert<auto_resize>(end(*this), c); }
		// Insert the range at the end of the string
		template<bool auto_resize = true, sus::container_t C> requires(sus::is_same_v<sus::iterator_value_t<sus::iterator_t<C>>, T>)
		inline void push(const C& c) { insert<auto_resize>(end(*this), c); }

		// Delete character
		template<bool auto_resize = true>
		constexpr iterator erase(iterator i) {
			return base_.template erase<auto_resize>(i);
		}
		// Delete characters
		template<bool auto_resize = true>
		constexpr iterator erase(iterator i, sus::usize n) {
			return base_.template erase<auto_resize>(i, n);
		}
		// Delete a range of characters
		template<bool auto_resize = true, sus::container_t C> requires(sus::is_same_v<sus::iterator_value_t<sus::iterator_t<C>>, T>)
		constexpr iterator erase(const C& c) {
			return base_.template erase<auto_resize>(begin(c), end(c), size(c));
		}

		// Remove characters from the end of the string
		template<bool auto_resize = true>
		constexpr void pop(sus::usize n) {
			erase<auto_resize>(end(*this) - n, n);
		}
		// Delete a character from the end of a string
		template<bool auto_resize = true>
		constexpr void pop() {
			erase<auto_resize>(end(*this) - 1);
		}
		// Clear the entire line
		template<bool shrink = false>
		constexpr void clear() {
			base_.clear();
			base_.template push<false>(sus::iterator_value_t<iterator>(0));
			if constexpr (shrink) shrink_to_fit();
		}

		// Replace the string with characters
		template<bool auto_resize = true>
		constexpr void assign(sus::iterator_value_t<iterator> c, sus::usize n) {
			base_.template assign<auto_resize>(c, n);
			base_.template push<false>(sus::iterator_value_t<iterator>(0));
		}
		// Replace the string with one character
		template<bool auto_resize = true>
		constexpr void assign(sus::iterator_value_t<iterator> c) {
			base_.template assign<auto_resize>(c);
			base_.template push<false>(sus::iterator_value_t<iterator>(0));
		}
		// Replace a string with a range
		template<bool auto_resize = true, sus::container_t C> requires(sus::is_same_v<sus::iterator_value_t<sus::iterator_t<C>>, T>)
		constexpr void assign(const C& c) {
			base_.template assign<auto_resize>(c);
			base_.template push<false>(sus::iterator_value_t<iterator>(0));
		}
	public:
		template<sus::container_t C> requires(sus::is_same_v<sus::iterator_value_t<sus::iterator_t<C>>, T>)
		constexpr string& operator=(const C& c) { assign(c); return *this; }
		constexpr string& operator=(sus::chr c) { assign(c); return *this; }
		constexpr string(const string& other) = default;
		constexpr string& operator=(const string& other) = default;
		constexpr string(string&& other) = default;
		constexpr string& operator=(string&& other) = default;
		constexpr ~string() = default;
		constexpr string() : base_() { base_.template push<false>(sus::iterator_value_t<iterator>(0)); }
		constexpr string(sus::usize capacity) : base_(capacity + 1) { base_.template push<false>(sus::iterator_value_t<iterator>(0)); }
		template<sus::container_t C> requires(sus::is_same_v<sus::iterator_value_t<sus::iterator_t<C>>, T>)
		constexpr string(const C& c) : base_(c) { base_.template push<false>(sus::iterator_value_t<iterator>(0)); }
	};

	template<sus::container_t C>
	string(const C&) -> string<sus::iterator_value_t<sus::iterator_t<C>>>;

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_CORE_STRING_ */
