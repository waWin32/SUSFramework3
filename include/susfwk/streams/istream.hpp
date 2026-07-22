// susfwk-streams-istream.hpp
//
/*
* =====---------------- istream.hpp - Abstract input stream ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====---------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_STREAMS_ISTREAM_
#define _SUSFWK_STREAMS_ISTREAM_

#include "susfwk/utils/core.hpp"
#include "susfwk/utils/iterator.hpp"
#include "susfwk/utils/container.hpp"
#include "susfwk/arch/math.hpp"
#include "susfwk/arch/strops.hpp"
#include "susfwk/core/algorithms.hpp"
#include "susfwk/core/vector.hpp"
#include "susfwk/core/range.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// Blocking the input stream
	template<sus::char_t T = sus::chr>
	class istream {
	private:
		sus::vector <T, sus::optimization_policy{ .priority = sus::optimization_policy::priority_t::speed }> buffer_{};
		sus::usize read_ = 0;
		bool auto_dump_;
	public:
		using char_type = T;
		// Maximum buffer size
		static constexpr sus::usize MAX_BUFFER_SIZE = 4096;
		// The function of reading from the stream directly
		virtual sus::usize underflow(T* buffer, sus::usize count) = 0;
		~istream() = default;
		istream(bool auto_dump = false) : auto_dump_(auto_dump) {}
	private:
		// Compress the buffer when empty
		inline void compress() {
			if (read_ > size(buffer_) / 2) {
				buffer_.erase(begin(buffer_), read_);
				read_ = 0;
			}
		}
		// Read data from a stream
		inline sus::usize recv() {
			auto end = size(buffer_);
			buffer_.push_uninitialized(MAX_BUFFER_SIZE);
			auto n = underflow(begin(buffer_) + end, MAX_BUFFER_SIZE);
			if (n != MAX_BUFFER_SIZE) buffer_.pop(MAX_BUFFER_SIZE - n);
			return n;
		}
	public:
		// Reset the read buffer
		inline void dump() {
			buffer_.clear();
			read_ = 0;
		}
		// End the read operation
		void end_input() { if (auto_dump_) dump(); }
		// Read the range from the stream and save
		template<sus::container_t C> requires(sus::is_iterator_random_access_v<sus::iterator_t<C>> && sus::is_same_v<sus::iterator_value_t<sus::iterator_t<C>>, T>)
		inline sus::usize read(C& dst) {
			auto count = size(dst);
			auto received = size(buffer_) - read_;
			if (received < count) {
				count -= received;
				if (count > MAX_BUFFER_SIZE) { // Buffer bypass reading
					sus::uninitialized_move_forward(begin(buffer_) + read_, begin(buffer_) + read_ + received, begin(dst));
					dump();
					return underflow(begin(dst) + received, count) + received;
				}
				else { // Reading with buffer
					received += sus::min(recv(), count);
				}
			}
			else received = count;
			sus::uninitialized_move_forward(begin(buffer_) + read_, begin(buffer_) + read_ + received, begin(dst));
			read_ += received;
			compress();
			return received;
		}
		// Read one character from the stream
		inline bool read(T& c) {
			if (size(buffer_) == read_) if (!recv()) return false;
			c = buffer_[read_];
			++read_;
			compress();
			return true;
		}
		// Look up n characters in the stream without shifting it
		inline sus::range<const T*> peek(sus::usize count) {
			auto received = size(buffer_) - read_;
			if (received < count) {
				count -= received;
				if (count > MAX_BUFFER_SIZE) { // Buffer bypass reading
					auto end = size(buffer_);
					buffer_.push_uninitialized(count);
					auto n = underflow(begin(buffer_) + end, count);
					if (n != count) buffer_.pop(count - n);
					received += n;
				}
				else { // Reading with buffer
					received += sus::min<sus::usize>(recv(), count);
				}
			}
			else received = count;
			return sus::range<const T*>{ begin(buffer_) + read_, received };
		}
		// Look up one character from the stream without shifting it
		inline sus::u64 peek() {
			if (size(buffer_) == read_) if (!recv()) return sus::numeric_limits<sus::u64>::max();
			return buffer_[read_];
		}
		// Skip n characters of the stream
		inline sus::usize discard(sus::usize count) {
			if (!count) return 0;
			auto received = size(buffer_) - read_;
			if (received < count) {
				count -= received;
				if (count > MAX_BUFFER_SIZE) { // Buffer bypass reading
					dump();
					T temp[MAX_BUFFER_SIZE];
					while (received < count) {
						sus::usize n = underflow(temp, sus::min<sus::usize>(count - received, MAX_BUFFER_SIZE));
						received += n;
						if (n < MAX_BUFFER_SIZE) break;
					}
				}
				else { // Reading with buffer
					received += sus::min<sus::usize>(recv(), count);
				}
			}
			else received = count;
			read_ += received;
			compress();
			return received;
		}
		// Skip 1 character of the stream
		inline bool discard() {
			if (size(buffer_) == read_) if (!recv()) return false;
			++read_;
			compress();
			return true;
		}
	};

	// -------------------------------------------------------------------

	// Check if the stream is ostream
	template<class T>
	struct is_istream : sus::bool_constant < requires(T s, T::char_type c) {
		{ s.read(c) };
		{ s.dump() };
		{ s.underflow(&c, 1) };
	} > {
	};
	template<typename T>
	inline constexpr bool is_istream_v = is_istream<T>::v;
	template<typename T>
	concept istream_t = is_istream_v<T>;

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_STREAMS_ISTREAM_ */
