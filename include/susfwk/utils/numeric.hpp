// susfwk-utils-numeric.hpp
//
/*
* =====---------------- numeric.hpp - all information for working with numbers ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====----------------------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_UTILS_NUMERIC_
#define _SUSFWK_UTILS_NUMERIC_

#include "susfwk/utils/core.hpp"

namespace sus {

	// -------------------------------------------------------------------

	template<typename T>
	struct is_signed : sus::bool_constant<(T(0) > T(-1))> {};
	template<typename T>
	inline constexpr bool is_signed_v = is_signed<T>::v;
	template<typename T>
	concept signed_t = is_signed_v<T>;
	template<typename T>
	struct is_unsigned : sus::bool_constant<(T(0) < T(-1))> {};
	template<typename T>
	inline constexpr bool is_unsigned_v = is_unsigned<T>::v;
	template<typename T>
	concept unsigned_t = is_unsigned_v<T>;
	template<typename T>
	struct is_scalar : sus::bool_constant<__is_scalar(T)> {};
	template<typename T>
	inline constexpr bool is_scalar_v = is_scalar<T>::v;
	template<typename T>
	concept scalar_t = is_scalar_v<T>;
	template<typename T>
	struct is_integral : bool_constant<__is_integral(T)> {};
	template<typename T>
	inline constexpr bool is_integral_v = is_integral<T>::v;
	template<typename T>
	concept integral_t = is_integral_v<T>;
	template<typename T>
	struct is_floating : bool_constant<__is_floating_point(T)> {};
	template<typename T>
	inline constexpr bool is_floating_v = is_floating<T>::v;
	template<typename T>
	concept floating_t = is_floating_v<T>;
	template<typename T>
	struct is_arithmetic : bool_constant<sus::is_integral<T>::v || sus::is_floating<T>::v> {};
	template<typename T>
	inline constexpr bool is_arithmetic_v = is_arithmetic<T>::v;
	template<typename T>
	concept arithmetic_t = is_arithmetic_v<T>;

	template<sus::integral_t T>
	struct make_unsigned : type_identity<T> {};
	template<>
	struct make_unsigned<sus::i8> : type_identity<sus::u8> {};
	template<>
	struct make_unsigned<sus::i16> : type_identity<sus::u16> {};
	template<>
	struct make_unsigned<sus::i32> : type_identity<sus::u32> {};
	template<>
	struct make_unsigned<sus::i64> : type_identity<sus::u64> {};
	template<typename T>
	using make_unsigned_t = typename make_unsigned<T>::t;

	template<sus::integral_t T>
	struct make_signed : type_identity<T> {};
	template<>
	struct make_signed<sus::u8> : type_identity<sus::i8> {};
	template<>
	struct make_signed<sus::u16> : type_identity<sus::i16> {};
	template<>
	struct make_signed<sus::u32> : type_identity<sus::i32> {};
	template<>
	struct make_signed<sus::u64> : type_identity<sus::i64> {};
	template<typename T>
	using make_signed_t = typename make_signed<T>::t;

	template<sus::arithmetic_t T>
	struct float_to_int : sus::type_identity<T> {};
	template<>
	struct float_to_int<sus::f32> : sus::type_identity<sus::i32> {};
	template<>
	struct float_to_int<sus::f64> : sus::type_identity<sus::i64> {};
	template<sus::arithmetic_t T>
	using float_to_int_t = typename float_to_int<T>::t;

	template<sus::arithmetic_t T>
	struct char_to_int : sus::type_identity<T> {};
	template<>
	struct char_to_int<sus::chr> : sus::type_identity<sus::u8> {};
	template<>
	struct char_to_int<sus::chr16> : sus::type_identity<sus::u16> {};
	template<>
	struct char_to_int<sus::chr32> : sus::type_identity<sus::u32> {};
	template<>
	struct char_to_int<sus::wchr> : sus::type_identity<sus::conditional_t<sizeof(sus::wchr) == 16, sus::u16, sus::u32>> {};
	template<sus::arithmetic_t T>
	using char_to_int_t = typename char_to_int<T>::t;

	// -------------------------------------------------------------------

	// 
	template<typename T>
	struct numeric_limits { static_assert(false, "The type does not have numeric_limits overload."); };
	// 
	template<>
	struct numeric_limits<bool> {
		static constexpr sus::u32 bits = 1;
		static constexpr sus::u32 digits = 1;
		static constexpr sus::u32 digits10 = 0;

		static constexpr bool min() { return false; }
		static constexpr bool max() { return true; }
	};
	template<sus::integral_t T>
	struct numeric_limits<T> {
		static constexpr sus::u32 bits = sizeof(T) * 8;
		static constexpr sus::u32 digits = bits - (sus::is_signed_v<T> ? 1 : 0);
		static constexpr sus::u32 digits10 = digits * 30103 / 100000;

		static constexpr T min() { return sus::is_signed_v<T> ? T(1) << (bits - 1) : T(0); }
		static constexpr T max() { return min() - 1; }
		static constexpr T lowest() { return min(); }
		static constexpr T epsilon() { return 0; }
		static constexpr T infinity() { return max(); }
	};
	// 
	template<sus::floating_t T>
	struct numeric_limits<T> {
		static constexpr sus::u32 bits = sizeof(T) * 8;
		static constexpr sus::u32 digits = bits == 64 ? 53 : 24;
		static constexpr sus::u32 digits10 = bits == 64 ? 15 : 6;
		static constexpr sus::u32 max_exponent = bits == 64 ? 1024 : 128;
		static constexpr sus::u32 min_exponent = bits == 64 ? -1021 : -125;
		static constexpr sus::make_unsigned_t<sus::float_to_int_t<T>> SIGN_MASK = bits == 64 ? 0x8000000000000000ULL : 0x80000000;
		static constexpr sus::make_unsigned_t<sus::float_to_int_t<T>> EXP_MASK = bits == 64 ? 0x7FF0000000000000ULL : 0x7F800000;
		static constexpr sus::make_unsigned_t<sus::float_to_int_t<T>> MANTISSA_MASK = bits == 64 ? 0xFFFFFFFFFFFFFULL : 0x7FFFFF;
		static constexpr sus::make_unsigned_t<sus::float_to_int_t<T>> INFINITY_BITS = bits == 64 ? 0x7FF0000000000000ULL : 0x7F800000;
		static constexpr sus::make_unsigned_t<sus::float_to_int_t<T>> NEG_INFINITY_BITS = bits == 64 ? 0xFFF0000000000000ULL : 0xFF800000;
		static constexpr sus::make_unsigned_t<sus::float_to_int_t<T>> QNAN_BITS = bits == 64 ? 0x7FF8000000000000ULL : 0x7FC00000;
		static constexpr sus::make_unsigned_t<sus::float_to_int_t<T>> SNAN_BITS = bits == 64 ? 0x7FF4000000000000ULL : 0x7FA00000;
		static constexpr sus::make_unsigned_t<sus::float_to_int_t<T>> ONE_BITS = bits == 64 ? 0x3FF0000000000000ULL : 0x3F800000;

		static constexpr T min() { return bits == 64 ? 2.2250738585072014e-308 : 1.17549435e-38F; }
		static constexpr T max() { return bits == 64 ? 1.7976931348623157e+308 : 3.40282347e+38F; }
		static constexpr T lowest() { return -max(); }
		static constexpr T epsilon() { return bits == 64 ? 2.2204460492503131e-16 : 1.19209290e-07F; }
		static constexpr T infinity() {
			union { T f; sus::make_unsigned_t<sus::float_to_int_t<T>> i; } u;
			u.i = INFINITY_BITS;
			return u.f;
		}
		static constexpr T neg_infinity() {
			union { T f; sus::make_unsigned_t<sus::float_to_int_t<T>> i; } u;
			u.i = NEG_INFINITY_BITS;
			return u.f;
		}
		static constexpr T quiet_NaN() {
			union { T f; sus::make_unsigned_t<sus::float_to_int_t<T>> i; } u;
			u.i = QNAN_BITS;
			return u.f;
		}
		static constexpr T signaling_NaN() {
			union { T f; sus::make_unsigned_t<sus::float_to_int_t<T>> i; } u;
			u.i = SNAN_BITS;
			return u.f;
		}
	};

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_UTILS_NUMERIC_ */
