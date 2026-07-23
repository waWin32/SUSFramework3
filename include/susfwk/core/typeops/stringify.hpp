// susfwk-core-typeops-stringify.hpp
//
/*
* =====---------------- stringify.hpp - converting a type to a string ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====-------------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_CORE_TYPEOPS_STRINGIFY_
#define _SUSFWK_CORE_TYPEOPS_STRINGIFY_

#include "susfwk/utils/core.hpp"
#include "susfwk/core/algorithms.hpp"
#include "susfwk/core/range.hpp"
#include "susfwk/streams/stringify.hpp"

// -------------------------------------------------------------------

#include "susfwk/arch/math.hpp"

namespace sus {

	// Symbol output
	template<sus::char_t C = sus::chr, auto f = sus::format_options<C>{} >
	inline void stringify(sus::ostream<C>& out, const C& x, sus::u32 = 0) {
		out.write(f.symbol.open);
		out.write(x);
		out.write(f.symbol.close);
	}
	// Numeric output
	template<sus::char_t C = sus::chr, auto f = sus::format_options<C>{}, sus::arithmetic_t T>
	inline void stringify(sus::ostream<C>& out, const T& x, sus::u32 = 0) {
		if constexpr (sus::is_floating<T>::v) {
			if (sus::isnan(x)) {
				if constexpr (f.numeric.uppercase)
					out.write(SUS_STRING_LITERAL(C, "NAN"));
				else
					out.write(SUS_STRING_LITERAL(C, "nan"));
				return;
			}
			else if (sus::isinf(x)) {
				if constexpr (f.numeric.uppercase)
					out.write(SUS_STRING_LITERAL(C, "INF"));
				else
					out.write(SUS_STRING_LITERAL(C, "inf"));
				return;
			}
		}
		C buffer[80];
		C* curr = buffer;
		T value = x;
		if constexpr (sus::is_signed_v<T>) {
			if (value < 0) { value = -value; *curr++ = C('-'); }
			else if constexpr (f.numeric.force_sign) *curr++ = C('+');
		}
		if constexpr (f.numeric.show_base_prefix) {
			if constexpr (f.numeric.base == 16) {
				curr[0] = C('0');
				if constexpr (f.numeric.uppercase) curr[1] = C('X');
				else curr[1] = C('x');
				curr += 2;
			}
			else if constexpr (f.numeric.base == 2) {
				curr[0] = C('0');
				if constexpr (f.numeric.uppercase) curr[1] = C('B');
				else curr[1] = C('b');
				curr += 2;
			}
			else if constexpr (f.numeric.base == 8) {
				*curr++ = C('0');
			}
		}
		auto start = curr;
		sus::make_unsigned_t<sus::float_to_int_t<T>> int_part = value;
		do {
			*curr++ = sus::digit_to_char<C, f.numeric.uppercase>((sus::u8)(int_part % f.numeric.base));
			int_part /= f.numeric.base;
		} while (int_part);
		sus::strrev(start, curr);
		if constexpr (sus::is_floating_v<T> && f.numeric.precision > 0) {
			*curr++ = f.numeric.decimal_point;
			value = sus::fract(value);
			if (!value) *curr++ = C('0');
			else for (sus::usize i = 0, zero_count = 0; i < f.numeric.precision; i++) {
				value *= f.numeric.base;
				sus::u8 digit = (sus::u8)value;
				if constexpr (f.numeric.trim_zeros) {
					if (!digit) { ++zero_count; continue; }
					while (zero_count) { *curr++ = C('0'); --zero_count; }
				}
				*curr++ = sus::digit_to_char<C, f.numeric.uppercase>(digit);
				value = sus::fract(value);
			}
		}
		auto n = curr - buffer;
		if constexpr (f.numeric.min_width) {
			if (n < f.numeric.min_width) {
				if constexpr (!sus::is_digit(f.numeric.pad_char)) start = buffer;
				auto pad = start + (f.numeric.min_width - n);
				sus::uninitialized_move_backward(start, curr, pad + (curr - start));
				sus::uninitialized_fill(start, pad, f.numeric.pad_char);
				out.write(sus::range(buffer, f.numeric.min_width));
				return;
			}
		}
		out.write(sus::range(buffer, sus::make_unsigned_t<decltype(n)>(n)));
	}
	// Bool output
	template<typename C = sus::chr, auto f = sus::format_options<C>{} >
	inline void stringify(sus::ostream<C>& out, const bool& x, sus::u32 = 0) {
		out.write((C*)(x ? SUS_STRING_LITERAL(C, "true") : SUS_STRING_LITERAL(C, "false")));
	}

}

// -------------------------------------------------------------------

#include "susfwk/core/pair.hpp"

namespace sus {

	// Pair output
	template<typename C = sus::chr, auto f = sus::format_options<C>{}, typename T1, typename T2>
	constexpr void stringify(sus::ostream<C>& out, const sus::pair<T1, T2>& x, sus::u32 d = 0) {
		out.write(f.list.open);
		f.indent(out, d + 1);
		sus::stringify(out, x.first, d + 1);
		f.separate(out, d + 1);
		sus::stringify(out, x.second, d + 1);
		f.indent(out, d);
		out.write(f.list.close);
	}

}

// -------------------------------------------------------------------

#include "susfwk/core/string.hpp"

namespace sus {

	// Str format output
	template<typename C = sus::chr, auto f = sus::format_options<C>{}, sus::container_t R> requires(sus::is_same_v<sus::remove_cvref_t<C>, sus::iterator_value_t<sus::iterator_t<R>>>)
	constexpr void stringify(sus::ostream<C>& out, const R& x, sus::u32 = 0) {
		out.write(f.string.open);
		for (auto it = begin(x); it != end(x); ++it) {
			C c = *it;
			switch (c) {
			case C('\n'): out.write(f.string.escape); out.write(C('n')); break;
			case C('\t'): out.write(f.string.escape); out.write(C('t')); break;
			case C('\r'): out.write(f.string.escape); out.write(C('r')); break;
			case C('\b'): out.write(f.string.escape); out.write(C('b')); break;
			case C('\v'): out.write(f.string.escape); out.write(C('v')); break;
			case C('\a'): out.write(f.string.escape); out.write(C('a')); break;
			case C('\0'): out.write(f.string.escape); out.write(C('0')); break;
			case C('\\'): out.write(f.string.escape); out.write(C('\\')); break;
			case C('\"'): out.write(f.string.escape); out.write(C('\"')); break;
			case C('\''): out.write(f.string.escape); out.write(C('\'')); break;
			default:
				if (c < 32 || c > 126) {
					out.write(f.string.escape);
					out.write(C('x'));
					sus::stringify(out, static_cast<sus::char_to_int_t<C>>(c));
				}
				else out.write(c);
			}
		}
		out.write(f.string.close);
	}

}

// -------------------------------------------------------------------

#endif /* !_SUSFWK_CORE_TYPEOPS_STRINGIFY_ */
