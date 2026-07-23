// susfwk-streams-parse.hpp
//
/*
* =====---------------- parse.hpp - the concept of converting a string to a type ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====------------------------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_STREAMS_PARSE_
#define _SUSFWK_STREAMS_PARSE_

#include "susfwk/utils/core.hpp"
#include "susfwk/core/string.hpp"
#include "susfwk/streams/istream.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// Parsing options
	template<typename C>
	struct parse_options {
		bool has_spaces = true;
		bool handle_syntax_errors = true;
		C separator = C(',');
		struct {
			C open = C('[');
			C close = C(']');
			C separator = C(',');
		} array{};
		struct {
			C open = C('(');
			C close = C(')');
			C separator = C(',');
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
			C decimal_point = C('.');
		} numeric{};

		inline bool skip_spaces(sus::istream<C>& in) const {
			if (has_spaces) {
				while (true) {
					auto view = in.peek(sus::istream<C>::MAX_BUFFER_SIZE);
					if (sus::empty(view)) return false;
					auto skipped = sus::striml(data(view)) - data(view);
					if (skipped < size(view)) {
						in.discard(skipped);
						break;
					}
					else if (size(view) != sus::istream<C>::MAX_BUFFER_SIZE) return false;
					in.discard(sus::istream<C>::MAX_BUFFER_SIZE);
				}
			}
			return true;
		}
		template<sus::container_t R> requires(sus::is_same_v<sus::iterator_value_t<sus::iterator_t<R>>, C>)
		inline bool expect_sequence(sus::istream<C>& in, R& seq) const {
			if (sus::empty(seq)) return true;
			if (handle_syntax_errors) {
				auto view = in.peek(size(seq));
				if (size(view) < size(seq)) return false;
				for (sus::usize i = 0; i < size(view); ++i) {
					if (view[i] != seq[i]) return false;
				}
				in.discard(size(seq));
			}
			else if (in.discard(size(seq)) < size(seq)) return false;
			return true;
		}
		inline bool expect(sus::istream<C>& in, C expected) const {
			C c;
			if (!in.read()) return false;
			if (handle_syntax_errors) if (static_cast<C>(c) != expected) return false;
			return true;
		}
	};

	// -------------------------------------------------------------------

	// Fallback on an unknown type
	template<sus::char_t C = sus::chr, auto f = sus::parse_options<C>{}, typename T>
	constexpr const C* parse(const C*, T&) {
		static_assert(false, "Unknown type parsing");
		return nullptr;
	}
	// Pull a variable from a stream with formatting
	template<sus::char_t C, auto f = sus::parse_options<C>{}, typename T>
	constexpr bool operator>>(sus::istream<C>& in, T& x) {
		if (parse<C, f>(in, x)) { in.end_input(); return true; }
		return false;
	}
	// Pull a char from a stream with formatting
	template<sus::char_t C, auto f = sus::parse_options<C>{} >
	constexpr bool operator>>(sus::istream<C>& in, C& x) {
		if (in.read(x)) { in.end_input(); return true; }
		return false;
	}

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_STREAMS_PARSE_ */
