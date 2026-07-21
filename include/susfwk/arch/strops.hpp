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
#include "susfwk/arch/memops.h"
#include "susfwk/arch/exit.hpp"
#include "susfwk/utils/numeric.hpp"
#include "susfwk/utils/container.hpp"
#include "susfwk/utils/algorithms.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// Is the type a symbol
	template<typename C>
	struct is_char : sus::bool_constant<
		sus::is_same_v<C, sus::chr> ||
		sus::is_same_v<C, sus::wchr> ||
		sus::is_same_v<C, sus::chr16> ||
		sus::is_same_v<C, sus::chr32>
	> {};
	template<typename T>
	inline constexpr bool is_char_v = is_char<T>::v;
	template<typename T>
	concept char_t = is_char_v<T>;

	#define SUS_STRING_LITERAL(C, str) \
		_Generic((C*)nullptr, \
			char*: str, \
			wchar_t*: L##str, \
			char8_t*: u8##str, \
			char16_t*: u##str, \
			char32_t*: U##str \
		)

	// -------------------------------------------------------------------

	inline sus::usize size(const sus::chr* s) { return sus::strlen(s); }
	inline sus::usize size(const sus::chr16* s) { return sus::strlen16(s); }
	inline sus::usize size(const sus::chr32* s) { return sus::strlen32(s); }
	inline sus::usize size(const sus::wchr* s) { return sus::wcslen(s); }

	// -------------------------------------------------------------------

	template<sus::char_t C>
	constexpr bool is_space(C c) { return (c == C(' ') || (c >= C('\t') && c <= C('\r'))); }
	template<sus::char_t C>
	constexpr bool is_conn(C c) { return (c == C('-') || c == C('~') || c == C('_') || c == C('.')); }
	template<sus::char_t C>
	constexpr bool is_alpha(C c) { return (c >= C('A') && c <= C('Z')) || (c >= C('a') && c <= C('z')); }
	template<sus::char_t C>
	constexpr bool is_digit(C c) { return (c >= C('0') && c <= C('9')); }
	template<sus::char_t C>
	constexpr bool is_alnum(C c) { return sus::is_alpha(c) || sus::is_digit(c); }
	template<sus::char_t C>
	constexpr bool is_calnum(C c) { return sus::is_alpha(c) || sus::is_digit(c) || sus::is_conn(c); }
	template<sus::char_t C>
	constexpr bool is_lbracket(C c) { return (c == C('{') || c == C('[') || c == C('(')); }
	template<sus::char_t C>
	constexpr bool is_rbracket(C c) { return (c == C('}') || c == C(']') || c == C(')')); }
	template<sus::char_t C>
	constexpr bool is_bracket(C c) { return is_lbracket<C>(c) || is_rbracket<C>(c); }

	// -------------------------------------------------------------------

	template<sus::char_t C>
	constexpr C toupper(C x) {
		if (x >= C('a') && x <= C('z')) return x - (C('a') - C('A'));
		if constexpr (!sus::is_same_v<C, sus::chr>) {
			if (x >= C('à') && x <= C('ÿ')) return x - (C('à') - C('À'));
		}
		return x;
	}
	template<sus::char_t C>
	constexpr C tolower(C x) {
		if (x >= C('A') && x <= C('Z')) return x + (C('a') - C('A'));
		if constexpr (!sus::is_same_v<C, sus::chr>) {
			if (x >= C('À') && x <= C('ß')) return x + (C('à') - C('À'));
		}
		return x;
	}

	template<sus::container_t R>
	constexpr void toupper(R& x) {
		for (auto first = begin(x); first != end(x); ++first) {
			*first = sus::toupper(*first);
		}
	}
	template<sus::container_t R>
	constexpr void tolower(R& x) {
		for (auto first = begin(x); first != end(x); ++first) {
			*first = sus::tolower(*first);
		}
	}

	// -------------------------------------------------------------------

	template<sus::char_t C>
	constexpr void strrev(C* first, C* last) {
		for (--last; first < last; ++first, --last) {
			sus::swap(*first, *last);
		}
	}
	template<sus::char_t C, bool uppercase = true>
	constexpr C digit_to_char(sus::u8 x) {
		if (x > 9) return C(uppercase ? 'A' : 'a') + C(x - 10);
		else return C('0') + C(x);
	}
	template<sus::usize base = 10, sus::char_t C>
	constexpr sus::u8 char_to_digit(C c) {
		if constexpr (base <= 10) {
			if (c >= C('0') && c <= C('0') + (base - 1)) return c - C('0');
			else return -1;
		}
		else {
			if (c >= C('0') && c <= C('0') + (base - 1)) return c - C('0');
			else if (c >= C('A') && c <= C('A') + (base - 10)) return c - C('A') + 10;
			else if (c >= C('a') && c <= C('a') + (base - 10)) return c - C('a') + 10;
			else return -1;
		}
	}
	template<sus::char_t C>
	constexpr bool char_to_digit(C c, sus::usize base, sus::u8* digit) {
		if (sus::is_digit(c)) *digit = c - C('0');
		else if (c >= C('A') && c <= C('Z')) *digit = c - C('A') + 10;
		else if (c >= C('a') && c <= C('z')) *digit = c - C('a') + 10;
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

	template<sus::char_t C>
	inline C* striml(const C* s) {
		if constexpr (sus::is_same_v<C, sus::chr>) {
			return sus::striml_(s);
		}
		else if constexpr (sus::is_same_v<C, sus::chr16>) {
			return sus::striml16_(s);
		}
		else if constexpr (sus::is_same_v<C, sus::chr32>) {
			return sus::striml32_(s);
		}
		else if constexpr (sus::is_same_v<C, sus::wchr>) {
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