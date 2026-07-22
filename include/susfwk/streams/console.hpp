// susfwk-streams-console.hpp
//
/*
* =====---------------- console.hpp - structure for working with a static array ----------------=====
*
* Part of ths SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====---------------------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_STREAMS_CONSOLE_
#define _SUSFWK_STREAMS_CONSOLE_

#include "susfwk/utils/core.hpp"
#include "susfwk/utils/iterator.hpp"
#include "susfwk/arch/exit.hpp"
#include "susfwk/arch/global_init.hpp"
#include "susfwk/core/platform.h"
#include "susfwk/streams/istream.hpp"
#include "susfwk/streams/ostream.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// Input and output stream to the console
	template<sus::char_t T = sus::chr>
	class console : public istream<T>, public ostream<T> {
	private:
		#if defined(SUS_SYSTEM_WINDOWS) 
		HANDLE hIn;
		HANDLE hOut;
		#else
		int hIn;
		int hOut;
		#endif /* !SUS_SYSTEM */
	public:
		// The basic constructor for creating a console
		inline console() : istream<T>(true) { 
			#if defined(SUS_SYSTEM_WINDOWS) 
			hIn = GetStdHandle(STD_INPUT_HANDLE);
			hOut = GetStdHandle(STD_OUTPUT_HANDLE);
			#else
						// TODO
			#endif /* !SUS_SYSTEM */
		}
		// Destructor for safely closing the console
		inline ~console() {
			if (is_open()) {
				this->flush();
				this->dump();
			}
		}
		inline console(console&& other) : istream<T>(true) {
			#if defined(SUS_SYSTEM_WINDOWS)
			hIn = other.hIn;
			hOut = other.hOut;
			other.hIn = INVALID_HANDLE_VALUE;
			other.hOut = INVALID_HANDLE_VALUE;
			#else
			// TODO
			#endif /* !SUS_SYSTEM */
		}
		inline console& operator=(console&& other) {
			if (this != &other) {
				this->~console();
				::new(this) console(sus::move(other));
			}
			return *this;
		}
	public:
		// Direct read operation from the console stream
		virtual sus::usize underflow(T* buffer, sus::usize count) override {
			assert(buffer && count);
			#if defined(SUS_SYSTEM_WINDOWS)
			DWORD n;
			if constexpr (sus::is_same_v<T, sus::chr>) {
				if (!ReadConsoleA(hIn, buffer, static_cast<DWORD>(count), &n, NULL)) return 0;
			}
			else {
				if (!ReadConsoleW(hIn, buffer, static_cast<DWORD>(count), &n, NULL)) return 0;
			}
			return n;
			#else
			// TODO
			#endif /* !SUS_SYSTEM */
		}
		// Direct write operation to the console stream
		virtual sus::usize overflow(const T* buffer, sus::usize count) override {
			assert(buffer && count);
			#if defined(SUS_SYSTEM_WINDOWS)
			DWORD n;
			if constexpr (sus::is_same_v<T, sus::chr>) {
				if (!WriteConsoleA(hOut, buffer, static_cast<DWORD>(count), &n, NULL)) return 0;
			} else {
				if (!WriteConsoleW(hOut, buffer, static_cast<DWORD>(count), &n, NULL)) return 0;
			}
			return n;
			#else
			// TODO
			#endif /* !SUS_SYSTEM */
		}
	public:
		// Checking if the console is open
		inline bool is_open() const {
			#if defined(SUS_SYSTEM_WINDOWS)
			return (
				(hIn != INVALID_HANDLE_VALUE && hIn != NULL) &&
				(hOut != INVALID_HANDLE_VALUE && hOut != NULL)
				);
			#else
			// TODO
			#endif /* !SUS_SYSTEM */
		}
		// Implicit check to see if the console is open
		inline operator bool() const { return is_open(); }
	};

	SUS_DEFINE_GLOBALEX(console<sus::chr>, con, B)

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_STREAMS_CONSOLE_ */
