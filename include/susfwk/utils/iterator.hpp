// susfwk-utils-iterator.hpp
//
/*
* =====---------------- iterator.hpp - iterator template ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_UTILS_ITERATOR_
#define _SUSFWK_UTILS_ITERATOR_

#include "susfwk/utils/core.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// Iterator with forward direction
	template<typename I>
	struct is_iterator_forward : sus::bool_constant<requires(I i, const I i_other) {
		{ ++i } -> sus::same_t<I&>;
		{ i++ } -> sus::same_t<I>;
		{ *i };
		{ i == i_other } -> sus::same_t<bool>;
		{ i != i_other } -> sus::same_t<bool>;
	}>{};
	template<typename I>
	inline constexpr bool is_iterator_forward_v = is_iterator_forward<I>::v;
	template<typename I>
	concept iterator_forward_t = is_iterator_forward_v<I>;
	// Bidirectional iterator
	template<typename I>
	struct is_iterator_bidirectional : sus::bool_constant<sus::is_iterator_forward_v<I>&& requires(I i, const I i_other) {
		{ --i } -> sus::same_t<I&>;
		{ i-- } -> sus::same_t<I>;
	}> {};
	template<typename I>
	inline constexpr bool is_iterator_bidirectional_v = is_iterator_bidirectional<I>::v;
	template<typename I>
	concept iterator_bidirectional_t = is_iterator_bidirectional_v<I>;
	// Random Access Iterator
	template<typename I>
	struct is_iterator_random_access : sus::bool_constant<sus::is_iterator_bidirectional_v<I>&& requires(I i, const I i_other, sus::isize n, sus::isize ii) {
		{ i += n } -> sus::same_t<I&>;
		{ i + n } -> sus::same_t<I>;
		{ i -= n } -> sus::same_t<I&>;
		{ i - n } -> sus::same_t<I>;
		{ i - i_other };
		{ i[ii] };
		{ i > i_other } -> sus::same_t<bool>;
		{ i >= i_other } -> sus::same_t<bool>;
		{ i < i_other } -> sus::same_t<bool>;
		{ i <= i_other } -> sus::same_t<bool>;
	}> {};
	template<typename I>
	inline constexpr bool is_iterator_random_access_v = is_iterator_random_access<I>::v;
	template<typename I>
	concept iterator_random_access_t = is_iterator_random_access_v<I>;

	// -------------------------------------------------------------------

	// Get the iterator value type
	template<sus::iterator_forward_t I>
	struct iterator_value {
		using t = typename decltype([] {
			if constexpr (requires { typename I::value_type; })
				return sus::type_identity<typename I::value_type>{};
			else
				return sus::type_identity<sus::remove_cv_t<sus::remove_reference_t<decltype(*sus::declval<I>())>>>{};
			}())::t;
	};
	template<sus::iterator_forward_t I>
	using iterator_value_t = typename iterator_value<I>::t;
	// Get the reference type of the iterator value
	template<sus::iterator_forward_t I>
	struct iterator_reference {
		using t = typename decltype([] {
			if constexpr (requires { typename I::reference; })
				return sus::type_identity<typename I::reference>{};
			else
				return sus::type_identity<decltype(*sus::declval<I>())>{};
			}())::t;
	};
	template<sus::iterator_forward_t I>
	using iterator_reference_t = typename iterator_reference<I>::t;
	// Get the pointer type of the iterator value
	template<sus::iterator_forward_t I>
	struct iterator_pointer {
		using t = typename decltype([] {
			if constexpr (requires { typename I::pointer; })
				return sus::type_identity<typename I::pointer>{};
			else if constexpr (sus::is_pointer_v<I>)
				return sus::type_identity<I>{};
			else
				return sus::type_identity<decltype(sus::declval<I>().operator->())>{};
			}())::t;
	};
	template<sus::iterator_forward_t I>
	using iterator_pointer_t = typename iterator_pointer<I>::t;
	// Get the type of iterator distance difference
	template<sus::iterator_forward_t I>
	struct iterator_difference {
		using t = typename decltype([] {
			if constexpr (requires { typename I::difference_type; })
				return sus::type_identity<typename I::difference_type>{};
			else if constexpr (sus::is_iterator_random_access_v<I>)
				return sus::type_identity<decltype(sus::declval<I>() - sus::declval<I>())>{};
			else
				return sus::type_identity<sus::isize>{};
			}())::t;
	};
	template<sus::iterator_forward_t I>
	using iterator_difference_t = typename iterator_difference<I>::t;
	// Get the type of iterator from the type
	template<typename T>
	struct iterator {
		using t = typename decltype([] {
			if constexpr (requires { typename T::iterator; })
				return sus::type_identity<typename T::iterator>{};
			else
				return sus::type_identity<sus::array_to_pointer_t<sus::remove_reference_t<T>>>{};
			}())::t;
	};
	template<typename T>
	using iterator_t = typename iterator<T>::t;

	// -------------------------------------------------------------------

	// Is the backend iterator a forward iterator?
	template<typename I>
	struct is_iterator_backend_forward : sus::bool_constant <requires(I i, I i_other) {
		{ i.dereference() };
		{ i.increment() } -> sus::same_t<void>;
		{ i == i_other } -> sus::same_t<bool>;
		{ i != i_other } -> sus::same_t<bool>;
	} > {};
	template<typename I>
	inline constexpr bool is_iterator_backend_forward_v = is_iterator_backend_forward<I>::v;
	template<typename I>
	concept iterator_backend_forward_t = is_iterator_backend_forward_v<I>;
	// Is the backend iterator a bidirectional iterator
	template<class I>
	struct is_iterator_backend_bidirectional : sus::bool_constant <sus::is_iterator_backend_forward_v<I> && requires(I i) {
		{ i.decrement() } -> sus::same_t<void>;
	} > {};
	template<class I>
	inline constexpr bool is_iterator_backend_bidirectional_v = is_iterator_backend_bidirectional<I>::v;
	template<class I>
	concept iterator_backend_bidirectional_t = is_iterator_backend_bidirectional_v<I>;
	// Is the backend iterator a random access iterator
	template<class I>
	struct is_iterator_backend_random_access : sus::bool_constant <sus::is_iterator_backend_bidirectional_v<I> && requires(I i, const I i_other, sus::isize n, sus::usize s) {
		{ i.distance_to(i_other) };
		{ i.advance(n) } -> sus::same_t<void>;
		{ i.dereference_at(s) };
		{ i > i_other } -> sus::same_t<bool>;
		{ i >= i_other } -> sus::same_t<bool>;
		{ i < i_other } -> sus::same_t<bool>;
		{ i <= i_other } -> sus::same_t<bool>;
	} > {};
	template<class I>
	inline constexpr bool is_iterator_backend_random_access_v = is_iterator_backend_random_access<I>::v;
	template<class I>
	concept iterator_backend_random_access_t = is_iterator_backend_random_access_v<I>;

	// -------------------------------------------------------------------

	// The iterator is forward-only
	template<typename Derived>
	class iterator_forward_father {
	public:
		constexpr decltype(auto) operator*() const { return static_cast<const Derived*>(this)->dereference(); }
		constexpr decltype(auto) operator->() {
			if constexpr (requires { static_cast<const Derived*>(this)->arrow(); }) return static_cast<const Derived*>(this)->arrow();
			else return &static_cast<const Derived*>(this)->dereference();
		}
		constexpr Derived& operator++() { static_cast<Derived*>(this)->increment(); return *static_cast<Derived*>(this); }
		constexpr Derived operator++(int) { Derived old = *static_cast<const Derived*>(this); static_cast<Derived*>(this)->increment(); return old; }
	};
	// The direction iterator only goes forward and backward
	template<typename Derived>
	class iterator_bidirectional_father : public sus::iterator_forward_father<Derived> {
	public:
		constexpr Derived& operator--() { static_cast<Derived*>(this)->decrement(); return *static_cast<Derived*>(this); }
		constexpr Derived operator--(int) { Derived old = *static_cast<const Derived*>(this); static_cast<Derived*>(this)->decrement(); return old; }
	};
	// Random Access Iterator
	template<typename Derived>
	class iterator_random_access_father : public sus::iterator_bidirectional_father<Derived> {
	public:
		constexpr decltype(auto) operator-(const Derived& other) const { return static_cast<const Derived*>(this)->distance_to(other); }
		constexpr Derived& operator+=(sus::isize n) { static_cast<Derived*>(this)->advance(n); return *static_cast<const Derived*>(this); }
		constexpr Derived& operator-=(sus::isize n) { static_cast<Derived*>(this)->advance(-n); return *static_cast<const Derived*>(this); }
		constexpr Derived operator+(sus::isize n) const { Derived res = *static_cast<const Derived*>(this); res.advance(n); return res; }
		constexpr Derived operator-(sus::isize n) const { Derived res = *static_cast<const Derived*>(this); res.advance(-n); return res; }
		constexpr decltype(auto) operator[](sus::usize i) const { return static_cast<const Derived*>(this)->dereference_at(i); }
	};

	// -------------------------------------------------------------------

}

// -------------------------------------------------------------------

template<typename T, sus::usize N>
constexpr sus::usize size(const T(&)[N]) { return N; }
template<typename T, sus::usize N>
constexpr T* data(const T(&arr)[N]) { return const_cast<T*>(arr); }
template<typename T, sus::usize N>
constexpr T* begin(const T(&arr)[N]) { return const_cast<T*>(arr); }
template<typename T, sus::usize N>
constexpr T* end(const T(&arr)[N]) { return const_cast<T*>(arr + N); }

// -------------------------------------------------------------------

#endif /* !_SUSFWK_UTILS_ITERATOR_ */
