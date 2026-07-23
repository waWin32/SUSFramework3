// susfwk-utils-base_math.hpp
//
/*
* =====---------------- base_math.hpp - basic mathematical operations ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====-------------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_UTILS_BASE_MATH_
#define _SUSFWK_UTILS_BASE_MATH_

#include "susfwk/utils/core.hpp"
#include "susfwk/utils/mathdef.hpp"
#include "susfwk/utils/numeric.hpp"

namespace sus {

	// -------------------------------------------------------------------

	//Checking a number for infinity
	template<sus::floating_t T>
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr bool is_inf(const T& x) {
		union { T f; sus::make_unsigned_t<sus::float_to_int_t<T>> i; } u = { x };
		return (u.i & sus::numeric_limits<T>::EXP_MASK) == sus::numeric_limits<T>::EXP_MASK && (u.i & sus::numeric_limits<T>::MANTISSA_MASK) == 0;
	}
	// Checking the number that the number is not valid
	template<sus::floating_t T>
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr bool is_nan(const T& x) {
		union { T f; sus::make_unsigned_t<sus::float_to_int_t<T>> i; } u = { x };
		return (u.i & sus::numeric_limits<T>::EXP_MASK) == sus::numeric_limits<T>::EXP_MASK && (u.i & sus::numeric_limits<T>::MANTISSA_MASK) != 0;
	}
	// Checking the number that the number is valid
	template<sus::floating_t T>
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr bool is_finite(const T& x) {
		union { T f; sus::make_unsigned_t<sus::float_to_int_t<T>> i; } u = { x };
		return (u.i & sus::numeric_limits<T>::EXP_MASK) != sus::numeric_limits<T>::EXP_MASK;
	}
	// check if the number is negative
	template<sus::arithmetic_t T> requires sus::is_signed_v<T>
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr bool is_negative(T x) {
		if constexpr (sus::is_integral_v<T>) {
			return (x >> sus::numeric_limits<T>::bits) & 1;
		}
		else {
			union { T f; sus::make_unsigned_t<sus::float_to_int_t<T>> i; } u = { x };
			return (u.i >> sus::numeric_limits<T>::bits) & 1;
		}
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

	// -------------------------------------------------------------------

	// The number module
	template<sus::arithmetic_t T> requires sus::is_signed_v<T>
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr T abs(const T& x) {
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
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr T trunc(const T& x) {
		if constexpr (sus::is_same_v<T, sus::f32>)
			return static_cast<sus::f32>(static_cast<sus::i32>(x));
		else
			return static_cast<sus::f64>(static_cast<sus::i64>(x));
	}
	// Truncating the whole part
	template<sus::floating_t T>
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr T fract(const T& x) {
		return x - sus::trunc(x);
	}
	// The remainder of the division
	template<sus::floating_t T>
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr T fmod(const T& x, const T& y) {
		if constexpr (sus::is_same_v<T, sus::f32>)
			return __builtin_fmodf(x, y);
		else return __builtin_fmod(x, y);
	}

	// -------------------------------------------------------------------

	// Get the root of a number
	template<sus::floating_t T>
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr T sqrt(T x) {
		if constexpr (sus::is_same_v<T, sus::f32>)
			return __builtin_sqrtf(x);
		else return __builtin_sqrt(x);
	}
	// Check for a power of two
	template<sus::integral_t T> requires (sus::is_unsigned_v<T>)
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr bool is_power2(T x) { return x && !(x & (x - 1)); }
	// Round up to the power of two
	template<sus::integral_t T> requires (sus::is_unsigned_v<T>)
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr T round_to_power2(T x) {
		--x; x |= x >> 1; x |= x >> 2; x |= x >> 4;
		if constexpr (sizeof(T) >= 2) x |= x >> 8;
		if constexpr (sizeof(T) >= 4) x |= x >> 16;
		if constexpr (sizeof(T) >= 8) x |= x >> 32;
		return ++x;
	}
	// Exponentiation of a number to an integer power
	template<sus::arithmetic_t B, sus::integral_t T>
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST inline B pow(B base, T exp) {
		bool n = false;
		if constexpr (sus::is_signed_v<T>) {
			if (sus::is_negative(exp)) {
				if constexpr (sus::is_integral_v<B>) return B(0);
				else { exp = -exp; n = true; }
			}
		}
		T x = T(1);
		while (exp > T(0)) {
			if (exp & T(1)) x *= base;
			base *= base;
			exp >>= T(1);
		}
		if constexpr (sus::is_integral_v<B>) return x;
		else return n ? 1 / x : x;
	}

	// -------------------------------------------------------------------

	// convert degrees to radians
	template<floating_t T>
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr T to_radians(T x) {
		return x * math::pi<T> / T(180.0);
	}
	// convert radians to degrees
	template<floating_t T>
	SUS_ATTRIB_NODISCARD constexpr T to_degrees(T x) {
		return x * T(180.0) / math::pi<T>;
	}

	// -------------------------------------------------------------------

	// Linear interpolation
	template<sus::arithmetic_t T>
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr T lerp(T a, T b, T t) {
		return a + (b - a) * t;
	}
	// Smoothstep (Hermite interpolation)
	template<sus::floating_t T>
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr T smoothstep(T a, T b, T t) {
		T x = sus::clamp((t - a) / (b - a), T(0), T(1));
		return x * x * (T(3) - T(2) * x);
	}
	// Smootherstep (better Hermite)
	template<sus::floating_t T>
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr T smootherstep(T a, T b, T t) {
		T x = sus::clamp((t - a) / (b - a), T(0), T(1));
		return x * x * x * (x * (x * T(6) - T(15)) + T(10));
	}
	// Inverse lerp:
	template<sus::floating_t T>
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr T inverse_lerp(T a, T b, T v) {
		return (v - a) / (b - a);
	}
	// Remap value from one range to another
	template<sus::arithmetic_t T>
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr T remap(T v, T a1, T b1, T a2, T b2) {
		return a2 + (v - a1) * (b2 - a2) / (b1 - a1);
	}

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_UTILS_BASE_MATH_ */
