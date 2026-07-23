// susfwk-streams-fstring.hpp
//
/*
* =====---------------- fstring.hpp - formatting string ----------------=====
*
* Part of ths SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====-----------------------------------------------------------------=====
*/
#ifndef _SUSFWK_STREAMS_FSTRING_
#define _SUSFWK_STREAMS_FSTRING_

#include "susfwk/utils/core.hpp"
#include "susfwk/utils/iterator.hpp"
#include "susfwk/arch/global_init.hpp"
#include "susfwk/arch/exit.hpp"
#include "susfwk/core/string.hpp"
#include "susfwk/streams/istream.hpp"
#include "susfwk/streams/ostream.hpp"
#include "susfwk/streams/zstream.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// Input and output streams for the fstring
	template<sus::char_t T = sus::chr>
	class fstring : public istream<T>, public ostream<T>, public zstream {
	private:
		sus::string<T> data_;
		sus::usize pos_ = 0;
	public:
		fstring() = default;
		template<sus::container_t C> requires(sus::is_same_v<sus::iterator_value_t<sus::iterator_t<C>>, T>)
		fstring(C&& s) : data_(sus::forward<C>(s)) {};
		inline ~fstring() {
			this->flush();
			this->dump();
		}
		inline fstring(fstring&& other) = default;
		inline fstring& operator=(fstring&& other) {
			if (this != &other) {
				data_ = sus::move(other.data_);
			}
			return *this;
		}
	public:
		// Direct read operation from a fstring stream
		virtual sus::usize underflow(T* buffer, sus::usize count) override {
			count = sus::min(count, size(data_) - pos_);
			sus::uninitialized_copy_forward(begin(data_) + pos_, begin(data_) + pos_ + count, buffer);
			pos_ += count;
			return count;
		}
		// Direct write operation to a fstring stream
		virtual sus::usize overflow(const T* buffer, sus::usize count) override {
			auto replace = sus::min(count, size(data_) - pos_);
			if (replace) sus::copy_forward(buffer, buffer + replace, begin(data_) + pos_);
			if (count != replace) data_.push(sus::range{ buffer + replace, count - replace });
			pos_ += count;
			return count;
		}
		// Moving to a position in the fstring
		virtual sus::usize seek(seekdir target, sus::isize offset = 0) override {
			switch (target) {
			case seekdir::beg:
				pos_ = static_cast<sus::usize>(offset);
				break;
			case seekdir::cur:
				pos_ = static_cast<sus::usize>(static_cast<sus::isize>(pos_) + offset);
				break;
			case seekdir::end:
				pos_ = static_cast<sus::usize>(static_cast<sus::isize>(size(data_)) + offset);
				break;
			}
			if (pos_ > size(data_)) pos_ = size(data_);
			return pos_;
		}
		// Output a string from a fstring stream
		inline auto& str() { 
			this->flush();
			return data_;
		}
	};

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_STREAMS_FSTRING_ */