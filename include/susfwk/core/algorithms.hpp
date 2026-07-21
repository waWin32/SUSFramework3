// susfwk-core-algorithms.hpp
//
/*
* =====---------------- algorithms.hpp - a set of basic algorithms ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====----------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_CORE_ALGORITHMS_
#define _SUSFWK_CORE_ALGORITHMS_

#include "susfwk/utils/core.hpp"
#include "susfwk/utils/numeric.hpp"
#include "susfwk/utils/iterator.hpp"
#include "susfwk/utils/container.hpp"
#include "susfwk/core/pair.hpp"
#include "susfwk/arch/memops.h"

namespace sus {

	// -------------------------------------------------------------------

	template<typename F, typename... Args>
	struct is_invocable : sus::bool_constant < requires(F&& f, Args&&... args) {
		sus::forward<F>(f)(sus::forward<Args>(args)...);
	} > {};
	template<typename F, typename... Args>
	inline constexpr bool is_invocable_v = is_invocable<F, Args...>::v;
	template<typename F, typename... Args>
	concept invocable_t = is_invocable_v<F, Args...>;

	template<typename F, typename... Args>
	struct is_regular_invocable : sus::bool_constant<sus::is_invocable_v<F, Args...>&& sus::is_invocable_v<const sus::remove_reference_t<F>&, Args...>> {};
	template<typename F, typename... Args>
	inline constexpr bool is_regular_invocable_v = is_regular_invocable<F, Args...>::v;
	template<typename F, typename... Args>
	concept regular_invocable_t = is_regular_invocable_v<F, Args...>;

	template<typename F, typename... Args>
	struct is_predicate : sus::bool_constant < is_regular_invocable_v<F, Args...>&& requires(F f, Args... args) {
		{ f(args...) } -> sus::same_t<bool>;
	} > {};
	template<typename F, typename... Args>
	inline constexpr bool is_predicate_v = is_predicate<F, Args...>::v;
	template<typename F, typename... Args>
	concept predicate_t = is_predicate_v<F, Args...>;

	template<typename F, typename T>
	struct is_comparator : is_predicate<F, T, T> {};
	template<typename F, typename T>
	inline constexpr bool is_comparator_v = is_comparator<F, T>::v;
	template<typename F, typename T>
	concept comparator_t = is_comparator_v<F, T>;

	// -------------------------------------------------------------------

	template<typename T>
	struct equal_to { constexpr bool operator()(const T& lhs, const T& rhs) const { return lhs == rhs; } };
	template<typename T>
	struct greater { constexpr bool operator()(const T& lhs, const T& rhs) const { return lhs > rhs; } };
	template<typename T>
	struct greater_equal { constexpr bool operator()(const T& lhs, const T& rhs) const { return lhs >= rhs; } };
	template<typename T>
	struct less { constexpr bool operator()(const T& lhs, const T& rhs) const { return lhs < rhs; } };
	template<typename T>
	struct less_equal { constexpr bool operator()(const T& lhs, const T& rhs) const { return lhs <= rhs; } };

	// -------------------------------------------------------------------

	// Get the minimum from the range
	template<sus::container_t C> requires (sus::is_iterator_forward_v<sus::iterator_t<C>>)
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_PURE constexpr auto min_element(const C& c) {
		auto i = *begin(c);
		for (auto first = ++begin(c); first != end(c); ++first) if (*first < *i) i = first;
		return i;
	}
	// Get the most out of the range
	template<sus::container_t C> requires (sus::is_iterator_forward_v<sus::iterator_t<C>>)
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_PURE constexpr auto max_element(const C& c) {
		auto i = *begin(c);
		for (auto first = ++begin(c); first != end(c); ++first) if (*first > i) i = *first;
		return i;
	}

	// -------------------------------------------------------------------

	// Check that everything matches the condition
	template<sus::container_t C, sus::predicate_t<sus::add_const_t<sus::iterator_reference_t<sus::iterator_t<C>>>> Pred> requires (sus::is_iterator_forward_v<sus::iterator_t<C>>)
	SUS_ATTRIB_NODISCARD bool all_of(const C& c, Pred pred) {
		for (auto first = begin(c); first != end(c); ++first) if (!pred(*first)) return false;
		return true;
	}
	// Check that at least one matches the condition
	template<sus::container_t C, sus::predicate_t<sus::add_const_t<sus::iterator_reference_t<sus::iterator_t<C>>>> Pred> requires (sus::is_iterator_forward_v<sus::iterator_t<C>>)
	SUS_ATTRIB_NODISCARD bool any_of(const C& c, Pred pred) {
		for (auto first = begin(c); first != end(c); ++first) if (pred(*first)) return true;
		return false;
	}
	// Check that everything does not match the condition
	template<sus::container_t C, sus::predicate_t<sus::add_const_t<sus::iterator_reference_t<sus::iterator_t<C>>>> Pred> requires (sus::is_iterator_forward_v<sus::iterator_t<C>>)
	SUS_ATTRIB_NODISCARD bool none_of(const C& c, Pred pred) {
		for (auto first = begin(c); first != end(c); ++first) if (pred(*first)) return false;
		return true;
	}

	// Find an element in the range
	template<sus::container_t C, typename T> requires (sus::is_iterator_forward_v<sus::iterator_t<C>>)
	SUS_ATTRIB_NODISCARD auto find(const C& c, const T& value) {
		auto first = begin(c);
		for (; first != end(c); ++first) if (*first == value) break;
		return first;
	}
	// Find an element matching the condition in the range
	template<sus::container_t C, sus::predicate_t<sus::add_const_t<sus::iterator_reference_t<sus::iterator_t<C>>>> Pred> requires (sus::is_iterator_forward_v<sus::iterator_t<C>>)
	SUS_ATTRIB_NODISCARD auto find_if(const C& c, Pred pred) {
		auto first = begin(c);
		for (; first != end(c); ++first) if (pred(*first)) break;
		return first;
	}

	// -------------------------------------------------------------------

	// Copying a block without deleting it, forward
	template<iterator_forward_t I1, iterator_forward_t I2>
	constexpr I2 uninitialized_copy_forward(I1 first, I1 last, I2 dest) {
		if constexpr (sus::is_trivially_copyable_v<sus::iterator_value_t<I1>> && sus::is_pointer_v<I1> && sus::is_pointer_v<I2>) {
			return (I2)sus::memcpy((void*)dest, (void*)first, (last - first) * sizeof(sus::iterator_value_t<I1>));
		}
		else {
			for (; first != last; ++first, ++dest) ::new (&*dest) sus::iterator_value_t<I2>(*first);
			return dest;
		}
	}
	// Copying a block without deleting it, backword
	template<iterator_bidirectional_t I1, iterator_bidirectional_t I2>
	constexpr I2 uninitialized_copy_backward(I1 first, I1 last, I2 dest_end) {
		if constexpr (sus::is_trivially_copyable_v<sus::iterator_value_t<I1>> && sus::is_pointer_v<I1> && sus::is_pointer_v<I2>) {
			return (I2)sus::memcpybkw((void*)dest_end, (void*)last, (last - first) * sizeof(sus::iterator_value_t<I1>));
		}
		else {
			while (first != last) ::new (&*--dest_end) sus::iterator_value_t<I2>(*--last);
			return dest_end;
		}
	}
	// Copying a block without deleting or overlapping it
	template<iterator_random_access_t I1, iterator_random_access_t I2>
	constexpr I2 uninitialized_copy(I1 first, I1 last, I2 dest) {
		if constexpr (sus::is_trivially_copyable_v<sus::iterator_value_t<I1>> && sus::is_pointer_v<I1> && sus::is_pointer_v<I2>) {
			return (I2)sus::memmove((void*)dest, (void*)first, (last - first) * sizeof(sus::iterator_value_t<I1>));
		}
		else {
			if (dest > first && dest <= last) return sus::uninitialized_copy_backward(first, last, dest + (last - first));
			return sus::uninitialized_copy_forward(first, last, dest);
		}
	}

	// Copy forward with initialization
	template<iterator_forward_t I1, iterator_forward_t I2>
	constexpr I2 copy_forward(I1 first, I1 last, I2 dest) {
		if constexpr (sus::is_trivially_copyable_v<sus::iterator_value_t<I1>> && sus::is_pointer_v<I1> && sus::is_pointer_v<I2>) {
			return (I2)sus::memcpy((void*)dest, (void*)first, (last - first) * sizeof(sus::iterator_value_t<I1>));
		}
		else {
			for (; first != last; ++first, ++dest) *dest = *first;
			return dest;
		}
	}
	// Copy back with initialization
	template<iterator_bidirectional_t I1, iterator_bidirectional_t I2>
	constexpr I2 copy_backward(I1 first, I1 last, I2 dest_end) {
		if constexpr (sus::is_trivially_copyable_v<sus::iterator_value_t<I1>> && sus::is_pointer_v<I1> && sus::is_pointer_v<I2>) {
			return (I2)sus::memcpybkw((void*)dest_end, (void*)last, (last - first) * sizeof(sus::iterator_value_t<I1>));
		}
		else {
			while (first != last) *--dest_end = *--last;
			return dest_end;
		}
	}
	// Copy with initialization
	template<iterator_random_access_t I1, iterator_random_access_t I2>
	constexpr I2 copy(I1 first, I1 last, I2 dest) {
		if constexpr (sus::is_trivially_copyable_v<sus::iterator_value_t<I1>> && sus::is_pointer_v<I1> && sus::is_pointer_v<I2>) {
			return (I2)sus::memmove((void*)dest, (void*)first, (last - first) * sizeof(sus::iterator_value_t<I1>));
		}
		else {
			if (dest > first && dest <= last) return sus::copy_backward(first, last, dest + (last - first));
			return sus::copy_forward(first, last, dest);
		}
	}

	// Move forward without initialization
	template<iterator_forward_t I1, iterator_forward_t I2>
	constexpr I2 uninitialized_move_forward(I1 first, I1 last, I2 dest) {
		if constexpr (sus::is_trivially_copyable_v<sus::iterator_value_t<I1>> && sus::is_pointer_v<I1> && sus::is_pointer_v<I2>) {
			return (I2)sus::memcpy((void*)dest, (const void*)(first), (last - first) * sizeof(sus::iterator_value_t<I1>));
		}
		else {
			for (; first != last; ++first, ++dest) ::new (&*dest) sus::iterator_value_t<I2>(sus::move(*first));
			return dest;
		}
	}
	// Transfer backward without initialization
	template<iterator_bidirectional_t I1, iterator_bidirectional_t I2>
	constexpr I2 uninitialized_move_backward(I1 first, I1 last, I2 dest_end) {
		if constexpr (sus::is_trivially_copyable_v<sus::iterator_value_t<I1>> && sus::is_pointer_v<I1> && sus::is_pointer_v<I2>) {
			return (I2)sus::memcpybkw((void*)dest_end, (void*)last, (last - first) * sizeof(sus::iterator_value_t<I1>));
		}
		else {
			while (first != last) ::new (&*--dest_end) sus::iterator_value_t<I2>(sus::move(*--last));
			return dest_end;
		}
	}
	// Transfer without initialization
	template<iterator_random_access_t I1, iterator_random_access_t I2>
	constexpr I2 uninitialized_move(I1 first, I1 last, I2 dest) {
		if constexpr (sus::is_trivially_copyable_v<sus::iterator_value_t<I1>> && sus::is_pointer_v<I1> && sus::is_pointer_v<I2>) {
			return (I2)sus::memmove((void*)dest, (void*)first, (last - first) * sizeof(sus::iterator_value_t<I1>));
		}
		else {
			if (dest > first && dest <= last) return sus::uninitialized_move_backward(first, last, dest + (last - first));
			return sus::uninitialized_move_forward(first, last, dest);
		}
	}

	// Move objects forward
	template<iterator_forward_t I1, iterator_forward_t I2>
	constexpr I2 move_forward(I1 first, I1 last, I2 dest) {
		if constexpr (sus::is_trivially_copyable_v<sus::iterator_value_t<I1>> && sus::is_pointer_v<I1> && sus::is_pointer_v<I2>) {
			return (I2)sus::memcpy((void*)dest, (void*)first, (last - first) * sizeof(sus::iterator_value_t<I1>));
		}
		else {
			for (; first != last; ++first, ++dest) *dest = sus::move(*first);
			return dest;
		}
	}
	// Move objects backward
	template<iterator_bidirectional_t I1, iterator_bidirectional_t I2>
	constexpr I2 move_backward(I1 first, I1 last, I2 dest_end) {
		if constexpr (sus::is_trivially_copyable_v<sus::iterator_value_t<I1>> && sus::is_pointer_v<I1> && sus::is_pointer_v<I2>) {
			return (I2)sus::memcpybkw((void*)dest_end, (void*)last, (last - first) * sizeof(sus::iterator_value_t<I1>));
		}
		else {
			while (first != last) *--dest_end = sus::move(*--last);
			return dest_end;
		}
	}
	// Move objects
	template<iterator_random_access_t I1, iterator_random_access_t I2>
	constexpr I2 move(I1 first, I1 last, I2 dest) {
		if constexpr (sus::is_trivially_copyable_v<sus::iterator_value_t<I1>> && sus::is_pointer_v<I1> && sus::is_pointer_v<I2>) {
			return (I2)sus::memmove((void*)dest, (void*)first, (last - first) * sizeof(sus::iterator_value_t<I1>));
		}
		else {
			if (dest > first && dest <= last) return sus::move_backward(first, last, dest + (last - first));
			return sus::move_forward(first, last, dest);
		}
	}

	// -------------------------------------------------------------------

	// Fill memory with objects without destruction
	template<sus::iterator_forward_t I>
	constexpr void uninitialized_fill(I first, I last, sus::iterator_value_t<I> value) {
		if constexpr (sus::is_pointer_v<I> && sus::is_scalar_v<sus::iterator_value_t<I>>) {
			if constexpr (sizeof(sus::iterator_value_t<I>) == 8)
				sus::memset64((sus::i64*)first, (sus::i64)value, (sus::usize)(last - first));
			else if constexpr (sizeof(sus::iterator_value_t<I>) == 4)
				sus::memset32((sus::i32*)first, (sus::i32)value, (sus::usize)(last - first));
			else if constexpr (sizeof(sus::iterator_value_t<I>) == 2)
				sus::memset16((sus::i16*)first, (sus::i16)value, (sus::usize)(last - first));
			else
				sus::memset((void*)first, (sus::i32)value, (sus::usize)(last - first));
		}
		else while (first != last) { ::new(first) sus::iterator_value_t<I>(value); ++first; }
	}
	// Fill the memory with objects
	template<sus::iterator_forward_t I>
	constexpr void fill(I first, I last, sus::iterator_value_t<I> value) {
		if constexpr (sus::is_pointer_v<I> && sus::is_scalar_v<sus::iterator_value_t<I>>) uninitialized_fill(first, last, value);
		else while (first != last) { *first = value; ++first; }
	}

	// Delete an object
	template<sus::iterator_forward_t I>
	constexpr void destroy(I first) {
		using VT = sus::iterator_value_t<I>;
		if constexpr (!sus::is_trivially_destructible_v<VT>)
			first->~vt();
	}
	// Delete an array of objects
	template<sus::iterator_forward_t I>
	constexpr void destroy(I first, sus::usize n) {
		using VT = sus::iterator_value_t<I>;
		if constexpr (!sus::is_trivially_destructible_v<VT>)
			for (auto last = first + n; first != last; ++first) first->~VT();
	}
	// Delete a range
	template<sus::container_t C> requires (sus::is_iterator_forward_v<sus::iterator_t<C>>)
	constexpr void destroy(const C& c) {
		using T = sus::iterator_value_t<sus::iterator_t<C>>;
		if constexpr (!sus::is_trivially_destructible_v<T>) {
			for (auto first = begin(c); first != end(c); ++first) first->~T();
		}
	}

	// Compare two ranges
	template<sus::iterator_forward_t I1, sus::iterator_forward_t I2>
	constexpr sus::i32 compare(I1 s1, I2 s2, sus::usize n) {
		static_assert(sus::is_same_v<sus::iterator_value_t<I1>, sus::iterator_value_t<I2>>);
		if constexpr (sus::is_pointer_v<I1> && sus::is_pointer_v<I2> && sus::is_trivial_v<sus::iterator_value_t<I1>>) {
			if constexpr (sizeof(sus::iterator_value_t<I1>) == 2) return sus::memcmp16((const sus::i16*)s1, (const sus::i16*)s2, n);
			else if constexpr (sizeof(sus::iterator_value_t<I1>) == 4) return sus::memcmp32((const sus::i32*)s1, (const sus::i32*)s2, n);
			else if constexpr (sizeof(sus::iterator_value_t<I1>) == 8) return sus::memcmp64((const sus::i64*)s1, (const sus::i64*)s2, n);
			else return sus::memcmp((const void*)s1, (const void*)s2, n * sizeof(sus::iterator_value_t<I1>));
		}
		else {
			for (; n; --n, ++s1, ++s2) {
				if (*s1 != *s2) {
					if (*s1 > *s2) return 1; return -1;
				}
			}
			return 0;
		}
	}

	// -------------------------------------------------------------------

	// Exchange iterator data
	template<iterator_forward_t I1, iterator_forward_t I2>
	constexpr I2 swap_ranges(I1 first1, I1 last1, I2 first2) {
		for (; first1 != last1; ++first1, ++first2) sus::swap(*first1, *first2);
		return first2;
	}
	// Expand an array of objects
	template<iterator_bidirectional_t I>
	constexpr void reverse(I first, I last) {
		while (first != last && first != --last) sus::swap(*first++, *last);
	}
	// Transfer a memory block
	template<iterator_random_access_t I>
	constexpr I rotate(I first, I middle, I last) {
		if (middle == first) return last;
		if (middle == last) return first;
		sus::reverse(first, middle);
		sus::reverse(middle, last);
		sus::reverse(first, last);
		return first + (last - middle);
	}

	// -------------------------------------------------------------------

	// Define an element that is at least as large as the specified one
	template<sus::container_t C, sus::predicate_t<sus::add_const_t<sus::iterator_reference_t<sus::iterator_t<C>>>, sus::add_const_t<sus::iterator_reference_t<sus::iterator_t<C>>>> Compare = sus::less<sus::iterator_value_t<sus::iterator_t<C>>>>
		requires(sus::is_iterator_random_access_v<sus::iterator_t<C>>)
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_PURE constexpr sus::iterator_t<C> lower_bound(const C& x, const sus::iterator_reference_t<sus::iterator_t<C>> value, Compare cmp = Compare{}) {
		auto first = begin(x);
		auto n = size(x);
		while (n) {
			sus::usize step = n / 2;
			auto mid = first + step;
			if (cmp(*mid, value)) {
				first = mid + 1;
				n -= step + 1;
			}
			else n = step;
		}
		return first;
	}
	// Define an element strictly larger than the specified one
	template<sus::container_t C, sus::predicate_t<sus::add_const_t<sus::iterator_reference_t<sus::iterator_t<C>>>, sus::add_const_t<sus::iterator_reference_t<sus::iterator_t<C>>>> Compare = sus::less<sus::iterator_value_t<sus::iterator_t<C>>>>
		requires(sus::is_iterator_random_access_v<sus::iterator_t<C>>)
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_PURE constexpr sus::iterator_t<C> upper_bound(const C& x, const sus::iterator_reference_t<sus::iterator_t<C>> value, Compare cmp = Compare{}) {
		return lower_bound(x, value, [&cmp](const auto& a, const auto& b) { return !cmp(b, a); });
	}
	// Get a range of matches
	template<sus::container_t C, sus::predicate_t<sus::add_const_t<sus::iterator_reference_t<sus::iterator_t<C>>>, sus::add_const_t<sus::iterator_reference_t<sus::iterator_t<C>>>> Compare = sus::less<sus::iterator_value_t<sus::iterator_t<C>>>>
		requires(sus::is_iterator_random_access_v<sus::iterator_t<C>>)
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_PURE constexpr sus::pair<sus::iterator_t<C>, sus::iterator_t<C>> equal_range(const C& x, const sus::iterator_reference_t<sus::iterator_t<C>> value, Compare cmp = Compare{}) {
		return { lower_bound(x, value, cmp), upper_bound(x, value, cmp) };
	}
	// Binary search in a sorted array
	template<sus::container_t C, sus::predicate_t<sus::add_const_t<sus::iterator_reference_t<sus::iterator_t<C>>>, sus::add_const_t<sus::iterator_reference_t<sus::iterator_t<C>>>> Compare = sus::less<sus::iterator_value_t<sus::iterator_t<C>>>>
		requires(sus::is_iterator_random_access_v<sus::iterator_t<C>>)
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_PURE constexpr bool binary_search(const C& x, const sus::iterator_reference_t<sus::iterator_t<C>> value, Compare cmp = Compare{}) {
		auto it = sus::lower_bound(x, value, cmp);
		return it != end(x) && !cmp(value, *it) && !cmp(*it, value);
	}
	// Binary search in a sorted array
	template<sus::container_t C, sus::predicate_t<sus::add_const_t<sus::iterator_reference_t<sus::iterator_t<C>>>, sus::add_const_t<sus::iterator_reference_t<sus::iterator_t<C>>>> Compare = sus::less<sus::iterator_value_t<sus::iterator_t<C>>>, typename Equality = sus::equal_to<sus::iterator_value_t<sus::iterator_t<C>>>>
		requires(sus::is_iterator_random_access_v<sus::iterator_t<C>>)
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_PURE constexpr bool binary_search(const C& x, const sus::iterator_reference_t<sus::iterator_t<C>> value, Compare cmp = Compare{}, Equality eq = Equality{}) {
		auto it = sus::lower_bound(x, value, cmp);
		return it != end(x) && eq(value, *it);
	}

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_CORE_ALGORITHMS_ */