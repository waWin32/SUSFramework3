// susfwk-streams-ostream.hpp
//
/*
* =====---------------- ostream.hpp - Abstract output stream ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====----------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_STREAMS_OSTREAM_
#define _SUSFWK_STREAMS_OSTREAM_

#include "susfwk/utils/core.hpp"
#include "susfwk/utils/iterator.hpp"
#include "susfwk/utils/container.hpp"
#include "susfwk/arch/strops.hpp"
#include "susfwk/core/vector.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// Blocking the output stream
	template<sus::char_t T = sus::chr>
	class ostream {
	private:
		sus::vector <T, sus::optimization_policy{ .priority = sus::optimization_policy::priority_t::speed } > buffer_{};
	public:
		using char_type = T;
		// Maximum buffer size
		static constexpr sus::usize MAX_BUFFER_SIZE = 1024 * 4;
		// The function of direct output to the stream
		virtual sus::usize overflow(const T* buffer, sus::usize count) = 0;
		ostream() = default;
		~ostream() = default;
	public:
		// Flush the buffer into the stream
		inline sus::usize flush() {
			if (!size(buffer_)) return 0;
			auto n = overflow(data(buffer_), size(buffer_));
			if (n != size(buffer_))
				buffer_.template erase<false>(begin(buffer_), n);
			else
				buffer_.clear();
			return n;
		}
		// Output the range to the stream
		template<sus::container_t C> requires(sus::is_iterator_random_access_v<sus::iterator_t<C>>&& sus::is_same_v<sus::iterator_value_t<sus::iterator_t<C>>, T>)
		inline sus::usize write(const C& src) {
			if (size(buffer_) + size(src) > MAX_BUFFER_SIZE) {
				flush();
				if (size(src) > MAX_BUFFER_SIZE) return overflow(data(src), size(src));
			}
			buffer_.push(src);
			return size(src);
		}
		// Output 1 character to the stream
		inline void write(const T& x) {
			if (size(buffer_) >= MAX_BUFFER_SIZE) flush();
			buffer_.push(x);
		}
		// Output n character to the stream
		inline void write(const T& x, sus::usize n) {
			if (size(buffer_) + n > MAX_BUFFER_SIZE) {
				flush();
			}
			buffer_.push(x, n);
			if (n > MAX_BUFFER_SIZE) return flush();
		}
	};

	// -------------------------------------------------------------------

	// Check if the stream is ostream
	template<class T>
	struct is_ostream : sus::bool_constant<requires(T s, T::char_type c) {
		{ s.write(c) };
		{ s.flush() };
		{ s.overflow(&c, 1) };
	}> {};
	template<typename T>
	inline constexpr bool is_ostream_v = is_ostream<T>::v;
	template<typename T>
	concept ostream_t = is_ostream_v<T>;

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_STREAMS_OSTREAM_ */
