// susfwk-arch-math.hpp
//
/*
* =====---------------- math.hpp - basic mathematical operations ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====--------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_ARCH_MATH_
#define _SUSFWK_ARCH_MATH_

#include "susfwk/utils/core.hpp"
#include "susfwk/utils/numeric.hpp"

namespace sus {

	// -------------------------------------------------------------------

	//Checking a number for infinity
	template<sus::floating_t T>
	constexpr bool isinf(const T& value) {
		union { T f; sus::make_unsigned_t<sus::float_to_int_t<T>> i; } u = { value };
		return (u.i & sus::numeric_limits<T>::EXP_MASK) == sus::numeric_limits<T>::EXP_MASK && (u.i & sus::numeric_limits<T>::MANTISSA_MASK) == 0;
	}
	// Checking the number that the number is not valid
	template<sus::floating_t T>
	constexpr bool isnan(const T& value) {
		union { T f; sus::make_unsigned_t<sus::float_to_int_t<T>> i; } u = { value };
		return (u.i & sus::numeric_limits<T>::EXP_MASK) == sus::numeric_limits<T>::EXP_MASK && (u.i & sus::numeric_limits<T>::MANTISSA_MASK) != 0;
	}
	// Checking the number that the number is valid
	template<sus::floating_t T>
	constexpr bool isfinite(const T& value) {
		union { T f; sus::make_unsigned_t<sus::float_to_int_t<T>> i; } u = { value };
		return (u.i & sus::numeric_limits<T>::EXP_MASK) != sus::numeric_limits<T>::EXP_MASK;
	}

	// -------------------------------------------------------------------

	// Get a minimum of two values
	template<sus::arithmetic_t T>
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr auto min(T a, T b) { 
		if constexpr (sus::is_integral_v<T>) {
			if constexpr (sus::is_signed_v<T>) {
				using U = sus::make_unsigned_t<T>;
				U mask = (static_cast<U>(a) - static_cast<U>(b)) >> (sizeof(T) * 8 - 1);
				return static_cast<T>((static_cast<U>(a) & -mask) | (static_cast<U>(b) & ~- mask));
			}
			else {
				T mask = (a - b) >> (sizeof(T) * 8 - 1);
				return (a & -mask) | (b & ~- mask);
			}
		}
		else return (a < b) ? a : b;
	}
	// Get the maximum of two values
	template<sus::arithmetic_t T>
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr auto max(T a, T b) {
		if constexpr (sus::is_integral_v<T>) {
			if constexpr (sus::is_signed_v<T>) {
				using U = sus::make_unsigned_t<T>;
				U mask = (static_cast<U>(a) - static_cast<U>(b)) >> (sizeof(T) * 8 - 1);
				return static_cast<T>((static_cast<U>(b) & -mask) | (static_cast<U>(a) & ~- mask));
			}
			else {
				T mask = (a - b) >> (sizeof(T) * 8 - 1);
				return (b & -mask) | (a & ~- mask);
			}
		}
		else return (a > b) ? a : b;
	}
	// Get the value between the minimum and the maximum
	template<sus::arithmetic_t T>
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr auto clamp(T v, T l, T h) { return sus::min(sus::max(v, l), h); }

	// The number module
	template<sus::arithmetic_t T> requires sus::is_signed_v<T>
	constexpr T abs(const T& x) {
		if constexpr (sus::is_integral_v<T>) {
			T mask = x >> (sizeof(T) * 8 - 1);
			return (x + mask) ^ mask;
		}
		else {
			union { T f; sus::make_unsigned_t<sus::float_to_int_t<T>> i; } u = { x };
			u.i &= ~(sus::make_unsigned_t<sus::float_to_int_t<T>>(1) << (sizeof(T) * 8 - 1));
			return u.f;
		}
	}
	// Truncating the fractional part
	template<sus::floating_t T>
	constexpr T trunc(const T& x) {
		if constexpr (sus::is_same_v<T, sus::f32>)
			return static_cast<sus::f32>(static_cast<sus::i32>(x));
		else
			return static_cast<sus::f64>(static_cast<sus::i64>(x));
	}
	// Truncating the whole part
	template<sus::floating_t T>
	constexpr T fract(const T& x) {
		return x - sus::trunc(x);
	}
	// The remainder of the division
	template<sus::floating_t T>
	constexpr T fmod(const T& x, const T& y) {
		if constexpr (sus::is_same_v<T, sus::f32>)
			return __builtin_fmodf(x, y);
		else return __builtin_fmod(x, y);
	}

	// Rounding up
	template<sus::floating_t T>
	constexpr T ceil(const T& x) {
		if constexpr (sus::is_same_v<T, sus::f32>)
			return __builtin_ceilf(x);
		else return __builtin_ceil(x);
	}
	// Rounding down
	template<sus::floating_t T>
	constexpr T floor(const T& x) {
		if constexpr (sus::is_same_v<T, sus::f32>)
			return __builtin_floorf(x);
		else return __builtin_floor(x);
	}
	// Rounding according to the rules
	template<sus::floating_t T>
	constexpr T round(const T& x) {
		if constexpr (sus::is_same_v<T, sus::f32>)
			return __builtin_roundf(x);
		else return __builtin_round(x);
	}

	// Get the root of a number
	template<sus::floating_t T>
	constexpr T sqrt(const T& x) {
		if constexpr (sus::is_same_v<T, sus::f32>)
			return __builtin_sqrtf(x);
		else return __builtin_sqrt(x);
	}

	// Check for a power of two
	template<sus::integral_t T> requires (sus::is_unsigned_v<T>)
	SUS_ATTRIB_CONST constexpr bool is_power2(T value) { return value && !(value & (value - 1)); }
	// Round up to the power of two
	template<sus::integral_t T> requires (sus::is_unsigned_v<T>)
	SUS_ATTRIB_CONST constexpr T round_to_power2(T x) {
		--x; x |= x >> 1; x |= x >> 2; x |= x >> 4;
		if constexpr (sizeof(T) >= 2) x |= x >> 8;
		if constexpr (sizeof(T) >= 4) x |= x >> 16;
		if constexpr (sizeof(T) >= 8) x |= x >> 32;
		return ++x;
	}

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_ARCH_MATH_ */
