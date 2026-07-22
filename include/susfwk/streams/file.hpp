// susfwk-streams-file.hpp
//
/*
* =====---------------- file.hpp - file input and output system ----------------=====
*
* Part of ths SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====-------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_STREAMS_FILE_
#define _SUSFWK_STREAMS_FILE_

#include "susfwk/utils/core.hpp"
#include "susfwk/utils/iterator.hpp"
#include "susfwk/arch/global_init.hpp"
#include "susfwk/arch/exit.hpp"
#include "susfwk/arch/strops.hpp"
#include "susfwk/core/platform.h"
#include "susfwk/streams/istream.hpp"
#include "susfwk/streams/ostream.hpp"
#include "susfwk/streams/zstream.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// Tag for creating a file as a temporary file
	struct file_temp_tag_ {};
	inline constexpr file_temp_tag_ file_temp_tag;
	// Input and output streams for the file
	template<sus::char_t T = sus::chr>
	class file : public istream<T>, public ostream<T>, public zstream {
	private:
		#if defined(SUS_SYSTEM_WINDOWS) 
		HANDLE hFile;
		#else
				// TODO
		#endif /* !SUS_SYSTEM */
	public:
		enum class mode : sus::u32 {
			read = 1 << 0,	// The right to read from a file
			write = 1 << 1,	// The right to write to a file
			app = 1 << 2,	// Guaranteed that the recording will always go to the end of the file
			ate = 1 << 3,	// When opened, the file will write to the end by default
			create = 1 << 4,// If the file already exists, then the error
			open = 1 << 5,	// If there was no file, then the error
			trun = 1 << 6,	// When opening, trim the file to zero size
			readwrite = read | write
		};
	public:
		// Designer with a delayed opening file
		inline file() {
			#if defined(SUS_SYSTEM_WINDOWS)
			hFile = INVALID_HANDLE_VALUE;
			#else
			// TODO
			#endif /* !SUS_SYSTEM */
		}
		// Constructor with file opening
		template<sus::container_t R> requires(sus::is_char_v<sus::iterator_value_t<sus::iterator_t<R>>>)
		inline file(const R& file_path, sus::u32 m = (sus::u32)mode::readwrite) {
			assert(!((m & (sus::u32)mode::create) == (sus::u32)mode::create && (m & (sus::u32)mode::open) == (sus::u32)mode::open));
			#if defined(SUS_SYSTEM_WINDOWS)
			DWORD dwDesiredAccess = 0;
			DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
			if ((m & (sus::u32)mode::read) == (sus::u32)mode::read)
				dwDesiredAccess |= GENERIC_READ;
			if ((m & (sus::u32)mode::write) == (sus::u32)mode::write) {
				if ((m & (sus::u32)mode::app) == (sus::u32)mode::app)
					dwDesiredAccess |= FILE_APPEND_DATA;
				else
					dwDesiredAccess |= GENERIC_WRITE;
			}
			DWORD dwCreationDisposition = 0;
			if ((m & (sus::u32)mode::open) == (sus::u32)mode::open) {
				if ((m & (sus::u32)mode::trun) == (sus::u32)mode::trun)
					dwCreationDisposition |= TRUNCATE_EXISTING;
				else
					dwCreationDisposition |= OPEN_EXISTING;
			}
			else if ((m & (sus::u32)mode::create) == (sus::u32)mode::create) {
				dwCreationDisposition |= CREATE_NEW;
			}
			else {
				if ((m & (sus::u32)mode::trun) == (sus::u32)mode::trun)
					dwCreationDisposition |= CREATE_ALWAYS;
				else
					dwCreationDisposition |= OPEN_ALWAYS;
			}
			if constexpr (sus::is_same_v<sus::iterator_value_t<sus::iterator_t<R>>, sus::chr>)
				hFile = CreateFileA(data(file_path), dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
			else
				hFile = CreateFileW(data(file_path), dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
			if (!is_open()) {
				sus::set_last_error(sus::error(sus::error::code::iostream_file_open, sus::error::reason::system, sus::error::severity::critical, GetLastError()));
				return;
			}
			if ((m & (sus::u32)mode::ate) == (sus::u32)mode::ate) {
				LARGE_INTEGER liZero = {};
				SetFilePointerEx(hFile, liZero, NULL, FILE_END);
			}
			#else
			// TODO
			#endif /* !SUS_SYSTEM */
		}
		// Constructor for creating a temporary file
		template<sus::container_t R> requires(sus::is_char_v<sus::iterator_value_t<sus::iterator_t<R>>>)
		inline file(sus::file_temp_tag_, const R& prefix) {
			#if defined(SUS_SYSTEM_WINDOWS)
			sus::iterator_value_t<sus::iterator_t<R>> tempPath[MAX_PATH];
			if constexpr (sus::is_same_v<sus::iterator_value_t<sus::iterator_t<R>>, sus::chr>)
				GetTempPathA(MAX_PATH, tempPath);
			else
				GetTempPathW(MAX_PATH, tempPath);
			sus::iterator_value_t<sus::iterator_t<R>> tempFile[MAX_PATH];
			UINT result;
			if constexpr (sus::is_same_v<sus::iterator_value_t<sus::iterator_t<R>>, sus::chr>)
				result = GetTempFileNameA(tempPath, data(prefix), 0, tempFile);
			else
				result = GetTempFileNameW(tempPath, data(prefix), 0, tempFile);
			if (!result) {
				sus::set_last_error(sus::error(sus::error::code::iostream_temp_open, sus::error::reason::system, sus::error::severity::critical, GetLastError()));
				return;
			}
			if constexpr (sus::is_same_v<sus::iterator_value_t<sus::iterator_t<R>>, sus::chr>)
				hFile = CreateFileA(tempFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, NULL);
			else
				hFile = CreateFileW(tempFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, NULL);
			if (!is_open()) {
				sus::set_last_error(sus::error(sus::error::code::iostream_file_open, sus::error::reason::system, sus::error::severity::critical, GetLastError()));
				return;
			}
			#else
			// TODO
			#endif /* !SUS_SYSTEM */
		}
		// Closing the file
		inline ~file() {
			if (is_open()) {
				this->flush();
				this->dump();
				CloseHandle(hFile);
			}
		}
		inline file(file&& other) {
			#if defined(SUS_SYSTEM_WINDOWS)
			hFile = other.hFile;
			other.hFile = INVALID_HANDLE_VALUE;
			#else
			// TODO
			#endif /* !SUS_SYSTEM */
		}
		inline file& operator=(file&& other) {
			if (this != &other) {
				this->~file();
				::new(this) file(sus::move(other));
			}
			return *this;
		}
	public:
		// Direct read operation from a file stream
		virtual sus::usize underflow(T* buffer, sus::usize count) override {
			assert(is_open());
			#if defined(SUS_SYSTEM_WINDOWS)
			DWORD n;
			if (!ReadFile(hFile, buffer, static_cast<DWORD>(count) * sizeof(T), &n, NULL)) return 0;
			return n / sizeof(T);
			#else
			// TODO
			#endif /* !SUS_SYSTEM */
		}
		// Direct write operation to a file stream
		virtual sus::usize overflow(const T* buffer, sus::usize count) override {
			assert(is_open());
			#if defined(SUS_SYSTEM_WINDOWS)
			DWORD n;
			if (!WriteFile(hFile, buffer, static_cast<DWORD>(count) * sizeof(T), &n, NULL)) return 0;
			return n / sizeof(T);
			#else
			// TODO
			#endif /* !SUS_SYSTEM */
		}
		// Moving to a position in the file
		virtual sus::usize seek(seekdir target, sus::isize offset = 0) override {
			assert(is_open());
			this->flush(); this->dump();
			#if defined(SUS_SYSTEM_WINDOWS)
			DWORD dwMoveMethod = FILE_BEGIN;
			switch (target) {
			case seekdir::beg:
				dwMoveMethod = FILE_BEGIN;
				break;
			case seekdir::cur:
				dwMoveMethod = FILE_CURRENT;
				break;
			case seekdir::end:
				dwMoveMethod = FILE_END;
				break;
			}
			LARGE_INTEGER liNewPos;
			LARGE_INTEGER liOffset{ .QuadPart = static_cast<LONGLONG>(offset) };
			if (!SetFilePointerEx(hFile, liOffset, &liNewPos, dwMoveMethod)) {
				sus::set_last_error(sus::error(sus::error::code::iostream_file_seek, sus::error::reason::system, sus::error::severity::critical, GetLastError()));
				return static_cast<sus::usize>(-1);
			}
			return static_cast<sus::usize>(liNewPos.QuadPart);
			#else
			// TODO
			#endif /* !SUS_SYSTEM */
		}
	public:
		// Checking if the file is open
		inline bool is_open() const {
			#if defined(SUS_SYSTEM_WINDOWS)
			return hFile != INVALID_HANDLE_VALUE && hFile != NULL;
			#else
			// TODO
			#endif /* !SUS_SYSTEM */
		}
		// Implicit check to see if the file is open
		inline operator bool() const { return is_open(); }
	};

	// -------------------------------------------------------------------

	// Rename or move a file
	template<sus::container_t C> requires(sus::is_char_v<sus::iterator_value_t<sus::iterator_t<C>>>)
	inline bool rename(const C& old_name, const C& new_name) {
		#if defined(SUS_SYSTEM_WINDOWS)
		if constexpr (sus::is_same_v<sus::iterator_value_t<sus::iterator_t<C>>, sus::chr>)
			return MoveFileA(data(old_name), data(new_name));
		else
			return MoveFileW(data(old_name), data(new_name));
		#else
		// TODO
		#endif /* !SUS_SYSTEM */
	}
	// Delete the file on the way
	template<sus::container_t C> requires(sus::is_char_v<sus::iterator_value_t<sus::iterator_t<C>>>)
	inline bool remove(const C& file_name) {
		#if defined(SUS_SYSTEM_WINDOWS)
		if constexpr (sus::is_same_v<sus::iterator_value_t<sus::iterator_t<C>>, sus::chr>)
			return DeleteFileA(data(file_name));
		else
			return DeleteFileW(data(file_name));
		#else
		// TODO
		#endif /* !SUS_SYSTEM */
	}

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_STREAMS_FILE_ */
