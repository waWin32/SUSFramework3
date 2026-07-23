// susfwk-arch-strops.hpp
//
/*
* =====---------------- strops.hpp - Basic string operations ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====----------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_ARCH_STROPS_
#define _SUSFWK_ARCH_STROPS_

#include "susfwk/utils/core.hpp"
#include "susfwk/utils/numeric.hpp"
#include "susfwk/utils/container.hpp"
#include "susfwk/arch/memops.h"
#include "susfwk/arch/exit.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// Is the type a symbol
	template<typename T>
	struct is_char : sus::bool_constant<
		sus::is_same_v<T, sus::chr> ||
		sus::is_same_v<T, sus::wchr> ||
		sus::is_same_v<T, sus::chr16> ||
		sus::is_same_v<T, sus::chr32>
	> {};
	template<typename T>
	inline constexpr bool is_char_v = is_char<T>::v;
	template<typename T>
	concept char_t = is_char_v<T>;

	#define SUS_STRING_LITERAL(T, str) \
		_Generic((T*)nullptr, \
			char*: str, \
			wchar_t*: L##str, \
			char8_t*: u8##str, \
			char16_t*: u##str, \
			char32_t*: U##str \
		)

	// -------------------------------------------------------------------

	template<sus::char_t T>
	constexpr bool is_space(T c) { return (c == T(' ') || (c >= T('\t') && c <= T('\r'))); }
	template<sus::char_t T>
	constexpr bool is_conn(T c) { return (c == T('-') || c == T('~') || c == T('_') || c == T('.')); }
	template<sus::char_t T>
	constexpr bool is_alpha(T c) { return (c >= T('A') && c <= T('Z')) || (c >= T('a') && c <= T('z')); }
	template<sus::char_t T>
	constexpr bool is_digit(T c) { return (c >= T('0') && c <= T('9')); }
	template<sus::char_t T>
	constexpr bool is_alnum(T c) { return sus::is_alpha(c) || sus::is_digit(c); }
	template<sus::char_t T>
	constexpr bool is_calnum(T c) { return sus::is_alpha(c) || sus::is_digit(c) || sus::is_conn(c); }
	template<sus::char_t T>
	constexpr bool is_lbracket(T c) { return (c == T('{') || c == T('[') || c == T('(')); }
	template<sus::char_t T>
	constexpr bool is_rbracket(T c) { return (c == T('}') || c == T(']') || c == T(')')); }
	template<sus::char_t T>
	constexpr bool is_bracket(T c) { return is_lbracket<T>(c) || is_rbracket<T>(c); }

	// -------------------------------------------------------------------

	template<sus::char_t T>
	constexpr T toupper(T x) {
		if (x >= T('a') && x <= T('z')) return x - (T('a') - T('A'));
		if constexpr (!sus::is_same_v<T, sus::chr>) {
			if (x >= T('à') && x <= T('ÿ')) return x - (T('à') - T('À'));
		}
		return x;
	}
	template<sus::char_t T>
	constexpr T tolower(T x) {
		if (x >= T('A') && x <= T('Z')) return x + (T('a') - T('A'));
		if constexpr (!sus::is_same_v<T, sus::chr>) {
			if (x >= T('À') && x <= T('ß')) return x + (T('à') - T('À'));
		}
		return x;
	}

	template<sus::container_t C>
	constexpr void toupper(C& x) {
		for (auto first = begin(x); first != end(x); ++first) {
			*first = sus::toupper(*first);
		}
	}
	template<sus::container_t C>
	constexpr void tolower(C& x) {
		for (auto first = begin(x); first != end(x); ++first) {
			*first = sus::tolower(*first);
		}
	}

	// -------------------------------------------------------------------

	template<sus::char_t T>
	constexpr void strrev(T* first, T* last) {
		for (--last; first < last; ++first, --last) {
			sus::swap(*first, *last);
		}
	}
	template<sus::char_t T, bool uppercase = true>
	constexpr T digit_to_char(sus::u8 x) {
		if (x > 9) return T(uppercase ? 'A' : 'a') + T(x - 10);
		else return T('0') + T(x);
	}
	template<sus::usize base = 10, sus::char_t T>
	constexpr sus::u8 char_to_digit(T c) {
		if constexpr (base <= 10) {
			if (c >= T('0') && c <= T('0') + (base - 1)) return c - T('0');
			else return -1;
		}
		else {
			if (c >= T('0') && c <= T('0') + (base - 1)) return c - T('0');
			else if (c >= T('A') && c <= T('A') + (base - 10)) return c - T('A') + 10;
			else if (c >= T('a') && c <= T('a') + (base - 10)) return c - T('a') + 10;
			else return -1;
		}
	}
	template<sus::char_t T>
	constexpr bool char_to_digit(T c, sus::usize base, sus::u8* digit) {
		if (sus::is_digit(c)) *digit = c - T('0');
		else if (c >= T('A') && c <= T('Z')) *digit = c - T('A') + 10;
		else if (c >= T('a') && c <= T('z')) *digit = c - T('a') + 10;
		else return false;
		return base > *digit;
	}
	extern "C" {

		// -------------------------------------------------------------------

		sus::chr* striml_(const sus::chr* s);
		sus::chr16* striml16_(const sus::chr16* s);
		sus::chr32* striml32_(const sus::chr32* s);

		// -------------------------------------------------------------------

	}

	template<sus::char_t T>
	inline T* striml(const T* s) {
		if constexpr (sus::is_same_v<T, sus::chr>) {
			return sus::striml_(s);
		}
		else if constexpr (sus::is_same_v<T, sus::chr16>) {
			return sus::striml16_(s);
		}
		else if constexpr (sus::is_same_v<T, sus::chr32>) {
			return sus::striml32_(s);
		}
		else if constexpr (sus::is_same_v<T, sus::wchr>) {
			#if defined(SUS_SYSTEM_WINDOWS)
			return sus::striml16_(s);
			#else
			return sus::striml32_(s);
			#endif /* !SUS_SYSTEM */
		}
	}
}

// -------------------------------------------------------------------

template<sus::char_t T, sus::usize N>
constexpr sus::usize size(const T(&)[N]) { return N - 1; }
template<sus::char_t T, sus::usize N>
constexpr T* end(const T(&arr)[N]) { return const_cast<T*>(arr + N - 1); }

// -------------------------------------------------------------------

#endif /* !_SUSFWK_ARCH_STROPS_ */