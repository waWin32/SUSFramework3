// susfwk-arch-memops.hpp
//
/* 
* =====---------------- memops.hpp - std memroy operation ----------------=====
* 
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
* 
* =====-------------------------------------------------------------------===== 
*/
#ifndef _SUSFWK_ARCH_MEMOPS_
#define _SUSFWK_ARCH_MEMOPS_

#include "susfwk/utils/types.hpp"

namespace sus {
	extern "C" {

		// -------------------------------------------------------------------

		void* memcpy(void* dst, const void* src, sus::usize count);
		void* memcpybkw(void* dst_end, const void* src_end, sus::usize count);
		void* memmove(void* dst, const void* src, sus::usize count);
		void* memset(void* dst, sus::i32 value, sus::usize count);
		sus::i16* memset16(sus::i16* dst, sus::i16 value, sus::usize count);
		sus::i32* memset32(sus::i32* dst, sus::i32 value, sus::usize count);
		sus::i64* memset64(sus::i64* dst, sus::i64 value, sus::usize count);
		sus::wchr* wmemset(sus::wchr* dst, sus::wchr value, sus::usize count);

		SUS_ATTRIB_NODISCARD sus::i32 memcmp(const void* ptr1, const void* ptr2, sus::usize count);
		SUS_ATTRIB_NODISCARD sus::i32 memcmp16(const sus::i16* ptr1, const sus::i16* ptr2, sus::usize count);
		SUS_ATTRIB_NODISCARD sus::i32 memcmp32(const sus::i32* ptr1, const sus::i32* ptr2, sus::usize count);
		SUS_ATTRIB_NODISCARD sus::i32 memcmp64(const sus::i64* ptr1, const sus::i64* ptr2, sus::usize count);
		SUS_ATTRIB_NODISCARD sus::i32 wmemcmp(const sus::wchr* ptr1, const sus::wchr* ptr2, sus::usize count);

		SUS_ATTRIB_NODISCARD void* memchr_(const void* src, sus::i32 value, sus::usize count);
		SUS_ATTRIB_NODISCARD sus::i16* memchr16(const sus::i16* src, sus::i16 value, sus::usize count);
		SUS_ATTRIB_NODISCARD sus::i32* memchr32(const sus::i32* src, sus::i32 value, sus::usize count);
		SUS_ATTRIB_NODISCARD sus::i64* memchr64(const sus::i64* src, sus::i64 value, sus::usize count);
		SUS_ATTRIB_NODISCARD sus::wchr* wmemchr(const sus::wchr* src, sus::wchr value, sus::usize count);

		// -------------------------------------------------------------------

		SUS_ATTRIB_NODISCARD sus::usize strlen(const sus::chr* str);
		SUS_ATTRIB_NODISCARD sus::usize strlen16(const sus::chr16* str);
		SUS_ATTRIB_NODISCARD sus::usize strlen32(const sus::chr32* str);
		SUS_ATTRIB_NODISCARD sus::usize wcslen(const sus::wchr* str);
		sus::chr* strcpy(sus::chr* dest, const sus::chr* src);
		sus::chr16* strcpy16(sus::chr16* dest, const sus::chr16* src);
		sus::chr32* strcpy32(sus::chr32* dest, const sus::chr32* src);
		sus::wchr* wcscpy(sus::wchr* dest, const sus::wchr* src);

		// -------------------------------------------------------------------
	}
}

#endif /* !_SUSFWK_ARCH_MEMOPS_ */
