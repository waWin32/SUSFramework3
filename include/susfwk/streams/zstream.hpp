// susfwk-streams-zstream.hpp
//
/*
* =====---------------- zstream.hpp - Abstraction of a random access stream ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====-------------------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_STREAMS_ZSTREAM_
#define _SUSFWK_STREAMS_ZSTREAM_

#include "susfwk/utils/core.hpp"

namespace sus {

	// -------------------------------------------------------------------

	class zstream {
	public:
		enum class seekdir {
			beg, cur, end
		};
	public:
		// The function of changing the stream position
		virtual sus::usize seek(seekdir target, sus::isize offset = 0) = 0;
		// Get the current position
		sus::usize tell() { return seek(seekdir::cur, 0); }
	};

	// -------------------------------------------------------------------

	// Check if the stream is zstream
	template<class T>
	struct is_zstream : sus::bool_constant < requires(T s, sus::zstream::seekdir d) {
		{ s.seek(d, 0) };
		{ s.tell() };
	} > {};
	template<typename T>
	inline constexpr bool is_zstream_v = is_zstream<T>::v;
	template<typename T>
	concept zstream_t = is_zstream_v<T>;

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_STREAMS_ZSTREAM_ */