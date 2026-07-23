// susfwk-streams-stringify.hpp
//
/*
* =====---------------- stringify.hpp - the concept of converting a type to a string ----------------===== 
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====----------------------------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_STREAMS_STRINGIFY_
#define _SUSFWK_STREAMS_STRINGIFY_

#include "susfwk/utils/core.hpp"
#include "susfwk/core/string.hpp"
#include "susfwk/streams/ostream.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// Formatting Options
	template<sus::char_t C>
	struct format_options {
		bool margins = true;
		bool line_breaks = false;
		bool measuring_depth = true;
		C separator = C(',');
		C indentation_symbol = C(' ');
		C deepening_symbol = C('\t');
		struct {
			C open = C('[');
			C close = C(']');
		} array{};
		struct {
			C open = C('(');
			C close = C(')');
		} list{};
		struct {
			C open = C('{');
			C close = C('}');
			C separator = C(':');
		} map{};
		struct {
			C open = C('\'');
			C close = C('\'');
		} symbol{};
		struct {
			C open = C('\"');
			C close = C('\"');
			C escape = C('\\');
		} string{};
		struct {
			sus::u8 base = 10;
			sus::usize min_width = 0;
			C pad_char = C(' ');
			bool uppercase = false;
			bool show_base_prefix = true;
			bool force_sign = false;
			bool trim_zeros = true;
			sus::u8 precision = 6;
			C decimal_point = C('.');
		} numeric{};

		inline void indent(sus::ostream<C>& out, sus::u32 depth = 0) const {
			if (line_breaks) { out.write(C('\n')); if (measuring_depth) out.write(deepening_symbol, depth); }
			else if (margins) out.write(indentation_symbol);
		}
		inline void separate(sus::ostream<C>& out, sus::u32 depth = 0) const {
			out.write(separator);
			indent(out, depth);
		}
	};

	// -------------------------------------------------------------------

	// Fallback on an unknown type
	/*template<sus::char_t C = sus::chr, auto f = sus::format_options<C>{}, typename T>
	inline void stringify(sus::ostream<C>& out, const T& x, sus::u32 = 0) {
		out.write(SUS_STRING_LITERAL(C, "unknown"));
	}*/
	// Write a variable to a stream with formatting
	template<sus::char_t C, auto f = sus::format_options<C>{}, typename T >
	inline sus::ostream<C>& operator<<(sus::ostream<C>& out, const T& x) {
		stringify<C, f>(out, x); return out;
	}
	// Write a string to a stream with formatting
	template<sus::char_t C, auto f = sus::format_options<C>{}, sus::container_t R> requires(sus::is_same_v<sus::remove_cvref_t<C>, sus::iterator_value_t<sus::iterator_t<R>>>)
	inline sus::ostream<C>& operator<<(sus::ostream<C>& out, const R& x) {
		out.write(x); return out;
	}
	// Write a char to a stream with formatting
	template<sus::char_t C, auto f = sus::format_options<C>{}>
	inline sus::ostream<C>& operator<<(sus::ostream<C>& out, const C& x) {
		out.write(x); return out;
	}
	// Output operator through a manipulator to the output stream
	template<sus::char_t C>
	inline sus::ostream<C>& operator<<(sus::ostream<C>& out, sus::ostream<C>& (*pf)(sus::ostream<C>&)) {
		return pf(out);
	}
	// Output stream manipulator - line termination and flush output
	template<sus::char_t C>
	inline sus::ostream<C>& endl(sus::ostream<C>& out) {
		out.write(C('\n'));
		out.flush();
		return out;
	}
	// Output Stream Manipulator - flush output
	template<sus::char_t C>
	inline sus::ostream<C>& flush(sus::ostream<C>& out) {
		out.flush();
		return out;
	}

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_STREAMS_STRINGIFY_ */
