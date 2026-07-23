// susfwk-core-typeops-parse.hpp
//
/*
* =====---------------- parse.hpp - converting a string to a type ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====---------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_CORE_TYPEOPS_PARSE_
#define _SUSFWK_CORE_TYPEOPS_PARSE_

#include "susfwk/utils/core.hpp"
#include "susfwk/core/algorithms.hpp"
#include "susfwk/core/range.hpp"
#include "susfwk/streams/parse.hpp"

// -------------------------------------------------------------------

#include "susfwk/utils/base_math.hpp"

namespace sus {

	template<sus::char_t C = sus::chr, auto f = sus::parse_options<C>{} >
	constexpr bool parse(sus::istream<C>& in, C& x) {
		if (!f.skip_spaces(in)) return false;
		if (!f.expect(in, f.symbol.open)) return false;
		if (!in.read(x)) return false;
		if (!f.expect(in, f.symbol.close)) return false;
		return true;
	}
	template<sus::char_t C = sus::chr, auto f = sus::parse_options<C>{}, sus::arithmetic_t T>
	constexpr bool parse(sus::istream<C>& in, T& x) {
		if (!f.skip_spaces(in)) return false;
		bool negative = false;
		auto cc = in.peek();
		if (cc == sus::numeric_limits<sus::u64>::max()) return false;
		if (cc == C('+')) { if (!in.discard()) return false; }
		else if (cc == C('-')) {
			negative = true;
			if (!in.discard()) return false;
		}
		sus::usize base;
		if ((cc = in.peek()) == sus::numeric_limits<sus::u64>::max()) return false;
		if (cc == C('0')) {
			if (!in.discard()) return false;
			if constexpr (!f.numeric.base) {
				if ((cc = in.peek()) == sus::numeric_limits<sus::u64>::max()) return false;
				if (cc == C('0')) {
					x = 0;
					if (!in.discard()) return false;
					return true;
				}
				else if (cc == C('X') || cc == C('x')) {
					base = 16;
					if (!in.discard()) return false;
				}
				else if (cc == C('B') || cc == C('b')) {
					base = 2;
					if (!in.discard()) return false;
				}
				else {
					base = 8;
				}
			}
			else {
				x = 0;
				return true;
			}
		}
		else base = f.numeric.base;
		if ((cc = in.peek()) == sus::numeric_limits<sus::u64>::max() || !sus::is_digit((C)cc)) return false;
		sus::float_to_int_t<T> int_part = 0;
		C c;
		for (sus::u8 digit;;) {
			if (!in.read(c)) goto int_part_end;
			if (!sus::char_to_digit(c, base, &digit)) break;
			int_part = int_part * base + digit;
		}
		if constexpr (sus::is_floating_v<T>) {
			if (!in.read(c)) return false;
			if constexpr (f.handle_syntax_errors) if (c != f.numeric.decimal_point) goto int_part_end;
			T value = int_part;
			sus::usize ibase = base;
			for (sus::u8 digit;;ibase *= base) {
				if (!in.read(c)) goto float_part_end;
				if (!sus::char_to_digit(c, base, &digit)) break;
				value += T(digit) / ibase;
			}
		float_part_end:
			x = negative ? -value : value;
		}
	int_part_end:
		x = negative ? -int_part : int_part;
		return true;
	}
	template<sus::char_t C = sus::chr, auto f = sus::parse_options<C>{}>
	constexpr bool parse(sus::istream<C>& in, bool& x) {
		if (!f.skip_spaces(in)) return false;
		auto c = in.peek();
		if (c == sus::numeric_limits<sus::u64>::max()) return false;
		if (c == C('t') || c == C('T')) {
			if (!f.expect_sequence(in, C("rue"))) return false;
			x = true;
		}
		else if (c == C('f') || c == C('F')) {
			if (!f.expect_sequence(in, C("alse"))) return false;
			x = false;
		}
		return false;
	}

}

// -------------------------------------------------------------------

#include "susfwk/core/pair.hpp"

namespace sus {

	template<sus::char_t C = sus::chr, auto f = sus::parse_options<C>{}, typename T1, typename T2>
	constexpr bool parse(sus::istream<C>& in, sus::pair<T1, T2>& x) {
		if (!f.skip_spaces(in)) return false;
		if (!f.expect(in, f.list.open)) return false;
		if (!sus::parse<C, f>(in, x.first)) return false;
		if (!f.skip_spaces(in)) return false;
		if (!f.expect(in, f.list.separator)) return false;
		if (!sus::parse<C, f>(in, x.second)) return false;
		if (!f.skip_spaces(in)) return false;
		if (!f.expect(in, f.list.close)) return false;
		return true;
	}

}

// -------------------------------------------------------------------

#include "susfwk/core/string.hpp"

namespace sus {

	template<sus::char_t C = sus::chr, auto f = sus::parse_options<C>{}>
	constexpr bool parse(sus::istream<C>& in, sus::string<C>& x) {
		if (!f.skip_spaces(in)) return false;
		C c;
		if (!in.read(c)) return false;
		if (c != f.string.open) {
			while (sus::is_calnum(c)) { x.push(c); if (!in.read(c)) break; }
			return true;
		}
		bool escape = false;
		while (true) {
			if (!in.read(c)) return false;
			if (c == f.string.escape) {
				if (escape) x.push(f.string.escape);
				escape = !escape;
			}
			else {
				if (escape) {
					switch (c) {
					case C('n'): x.push(C('\n')); break;
					case C('t'): x.push(C('\t')); break;
					case C('r'): x.push(C('\r')); break;
					case C('b'): x.push(C('\b')); break;
					case C('v'): x.push(C('\v')); break;
					case C('a'): x.push(C('\a')); break;
					case C('0'): x.push(C('\0')); break;
					case C('\"'): x.push(C('\"')); break;
					case C('\''): x.push(C('\'')); break;
					case C('x'): {
						C d1;
						if (!in.read(d1)) return false;
						sus::u8 digit1;
						if (!sus::char_to_digit(static_cast<C>(d1), 16, &digit1)) return false;
						C d2;
						if (!in.read(d2)) return false;
						sus::u8 digit2;
						if (!sus::char_to_digit(static_cast<C>(d2), 16, &digit2)) return false;
						x.push(static_cast<C>(digit1 * 16 + digit2));
					} break;
					default: x.push(c);
					}
					escape = false;
				}
				else {
					if (c == f.string.close) return true;
					else x.push(c);
				}
			}
		}
	}
}

// -------------------------------------------------------------------

#endif /* !_SUSFWK_CORE_TYPEOPS_PARSE_ */
