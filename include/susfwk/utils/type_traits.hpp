// susfwk-utils-type_traits.hpp
//
/*
* =====---------------- type_traits.hpp - working with types at the compilation stage ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====-----------------------------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_UTILS_TYPE_TRAITS_
#define _SUSFWK_UTILS_TYPE_TRAITS_

#include "susfwk/utils/types.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// Compilation time type - true
	struct true_type { static constexpr bool v = true; };
	inline constexpr bool true_type_v = true_type::v;
	template<typename T>
	concept true_type_t = true_type_v;
	// Compilation time type - false
	struct false_type { static constexpr bool v = false; };
	inline constexpr bool false_type_v = false_type::v;
	template<typename T>
	concept false_type_t = false_type_v;
	// Determine the type of compilation time
	template<typename T>
	struct type_identity { using t = T; };
	template<typename T>
	using type_identity_t = typename type_identity<T>::t;
	// The compilation time condition is false
	template<bool C, typename T, typename F>
	struct conditional : type_identity<F> {};
	// The compilation time condition is true
	template<typename T, typename F>
	struct conditional<true, T, F> : type_identity<T> {};
	template<bool C, typename T, typename F>
	using conditional_t = typename conditional<C, T, F>::t;
	// A Boolean type constant
	template<bool C>
	struct bool_constant : sus::conditional_t<C, sus::true_type, sus::false_type> {};
	template<bool C>
	inline constexpr bool bool_constant_v = bool_constant<C>::v;
	template<bool C>
	concept bool_constant_t = bool_constant_v<C>;
	// Compilation time type disabled
	template<bool C, typename T = void>
	struct enable_if {};
	// Compilation time type enabled
	template<typename T>
	struct enable_if<true, T> : type_identity<T> {};
	template<bool C, typename T>
	using enable_if_t = typename enable_if<C, T>::t;
	// Converting to an rvalue reference
	template<typename T>
	constexpr T&& declval();
	// Compile-time verification that the types match
	template<typename T, typename U>
	struct is_same : sus::false_type {};
	template<typename T>
	struct is_same<T, T> : sus::true_type {};
	template<typename T, typename U>
	inline constexpr bool is_same_v = is_same<T, U>::v;
	template<typename T, typename U>
	concept same_t = is_same_v<T, U>;
	// Checking at compile time that types can be cast
	template<typename T, typename U>
	struct is_convertible : sus::bool_constant < requires { static_cast<U>(sus::declval<T>()); } > {};
	template<typename T, typename U>
	inline constexpr bool is_convertible_v = sus::is_convertible<T, U>::v;
	template<typename T, typename U>
	concept convertible_t = is_convertible_v<T, U>;

	// -------------------------------------------------------------------

	template<typename T>
	struct is_void : sus::bool_constant<__is_void(T)> {};
	template<typename T>
	inline constexpr bool is_void_v = is_void<T>::v;
	template<typename T>
	concept void_t = is_void_v<T>;
	template<typename T>
	struct is_pointer : sus::bool_constant<__is_pointer(T)> {};
	template<typename T>
	inline constexpr bool is_pointer_v = is_pointer<T>::v;
	template<typename T>
	concept pointer_t = is_pointer_v<T>;
	template<typename T>
	struct is_lvalue_reference : sus::bool_constant<__is_lvalue_reference(T)> {};
	template<typename T>
	inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::v;
	template<typename T>
	concept lvalue_reference_t = is_lvalue_reference_v<T>;
	template<typename T>
	struct is_rvalue_reference : sus::bool_constant<__is_rvalue_reference(T)> {};
	template<typename T>
	inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::v;
	template<typename T>
	concept rvalue_reference_t = is_rvalue_reference_v<T>;
	template<typename T>
	struct is_reference : bool_constant<is_lvalue_reference_v<T> || is_rvalue_reference_v<T>> {};
	template<typename T>
	inline constexpr bool is_reference_v = is_reference<T>::v;
	template<typename T>
	concept reference_t = is_reference_v<T>;
	template<typename T>
	struct is_const : sus::false_type {};
	template<typename T>
	struct is_const<const T> : sus::true_type {};
	template<typename T>
	inline constexpr bool is_const_v = is_const<T>::v;
	template<typename T>
	concept const_t = is_const_v<T>;
	template<typename T>
	struct is_volatile : sus::false_type {};
	template<typename T>
	struct is_volatile<volatile T> : sus::true_type {};
	template<typename T>
	inline constexpr bool is_volatile_v = is_volatile<T>::v;
	template<typename T>
	concept volatile_t = is_volatile_v<T>;

	template<typename T>
	struct is_trivially_copyable : sus::bool_constant<__is_trivially_copyable(T)> {};
	template<typename T>
	inline constexpr bool is_trivially_copyable_v = is_trivially_copyable<T>::v;
	template<typename T>
	concept trivially_copyable_t = is_trivially_copyable_v<T>;
	template<typename T>
	struct is_trivial : sus::bool_constant<__is_trivial(T)> {};
	template<typename T>
	inline constexpr bool is_trivial_v = is_trivial<T>::v;
	template<typename T>
	concept trivial_t = is_trivial_v<T>;
	template<typename T>
	struct is_trivially_destructible : sus::bool_constant<__is_trivially_destructible(T)> {};
	template<typename T>
	inline constexpr bool is_trivially_destructible_v = is_trivially_destructible<T>::v;
	template<typename T>
	concept trivially_destructible_t = is_trivially_destructible_v<T>;

	template<typename T>
	struct remove_reference : sus::type_identity<T> {};
	template<typename T>
	struct remove_reference<T&> : sus::type_identity<T> {};
	template<typename T>
	struct remove_reference<T&&> : sus::type_identity<T> {};
	template<typename T>
	using remove_reference_t = typename remove_reference<T>::t;
	template<typename T>
	struct remove_const : sus::type_identity<T> {};
	template<typename T>
	struct remove_const<const T> : sus::type_identity<T> {};
	template<typename T>
	using remove_const_t = typename remove_const<T>::t;
	template<typename T>
	struct remove_volatile : sus::type_identity<T> {};
	template<typename T>
	struct remove_volatile<volatile T> : sus::type_identity<T> {};
	template<typename T>
	using remove_volatile_t = typename remove_volatile<T>::t;
	template<typename T>
	struct remove_cv : sus::remove_const<remove_volatile_t<T>> {};
	template<typename T>
	using remove_cv_t = typename remove_cv<T>::t;
	template<typename T>
	struct remove_cvref : remove_cv<remove_reference_t<T>> {};
	template<typename T>
	using remove_cvref_t = typename remove_cvref<T>::t;
	template<typename T>
	struct remove_pointer : sus::type_identity<T> {};
	template<typename T>
	struct remove_pointer<T*> : sus::type_identity<T> {};
	template<typename T>
	struct remove_pointer<T* const> : sus::type_identity<T> {};
	template<typename T>
	struct remove_pointer<T* volatile> : sus::type_identity<T> {};
	template<typename T>
	struct remove_pointer<T* const volatile> : sus::type_identity<T> {};
	template<typename T>
	using remove_pointer_t = typename remove_pointer<T>::t;
	template<typename T>
	struct remove_extent : sus::type_identity<T> {};
	template<typename T, sus::usize N>
	struct remove_extent<T[N]> : sus::type_identity<T> {};
	template<typename T>
	struct remove_extent<T[]> : sus::type_identity<T> {};
	template<typename T>
	using remove_extent_t = typename remove_extent<T>::t;

	template<typename T>
	struct add_lvalue_reference : sus::type_identity<T&> {};
	template<>
	struct add_lvalue_reference<void> : sus::type_identity<void> {};
	template<typename T>
	using add_lvalue_reference_t = typename add_lvalue_reference<T>::t;
	template<typename T>
	struct add_rvalue_reference : sus::type_identity<T&&> {};
	template<>
	struct add_rvalue_reference<void> : sus::type_identity<void> {};
	template<typename T>
	using add_rvalue_reference_t = typename add_rvalue_reference<T>::t;
	template<typename T>
	struct add_pointer : sus::type_identity<T*> {};
	template<typename T>
	using add_pointer_t = typename add_pointer<T>::t;
	template<typename T>
	struct add_const : sus::type_identity<const T> {};
	template<typename T>
	struct add_const<T&> : sus::type_identity<const T&> {};
	template<typename T>
	struct add_const<T&&> : sus::type_identity<const T&&> {};
	template<typename T>
	using add_const_t = typename add_const<T>::t;
	template<typename T>
	struct add_volatile : sus::type_identity<volatile T> {};
	template<typename T>
	using add_volatile_t = typename add_volatile<T>::t;
	template<typename T>
	struct add_cv : sus::type_identity<const volatile T> {};
	template<typename T>
	using add_cv_t = typename add_cv<T>::t;

	template<typename T>
	struct array_to_pointer : sus::type_identity<T> {};
	template<typename T, sus::usize N>
	struct array_to_pointer<T[N]> : sus::type_identity<T*> {};
	template<typename T>
	struct array_to_pointer<T[]> : sus::type_identity<T*> {};
	template<typename T>
	using array_to_pointer_t = typename array_to_pointer<T>::t;
	template<typename T>
	struct decay : type_identity<T> {};
	template<typename T>
	struct decay<T[]> : type_identity<T*> {};
	template<typename T, size_t N>
	struct decay<T[N]> : type_identity<T*> {};
	template<typename T>
	struct decay<T&> : type_identity<T> {};
	template<typename T>
	struct decay<T&&> : type_identity<T> {};
	template<typename T>
	struct decay<const T> : type_identity<T> {};
	template<typename T>
	struct decay<volatile T> : type_identity<T> {};
	template<typename T>
	using decay_t = typename decay<T>::t;

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_UTILS_TYPE_TRAITS_ */
