// susfwk-utils-types.hpp
//
/*
* =====---------------- types.hpp - basic data types ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====--------------------------------------------------------------=====
*/
#ifndef _SUSFWK_UTILS_TYPES_
#define _SUSFWK_UTILS_TYPES_

namespace sus {

	// -------------------------------------------------------------------

	// Architecture
	#if defined(_M_X64) || defined(__x86_64__) || defined(__amd64__)
	#define SUS_ARCH_X86
	#elif defined(__aarch64__) || defined(_M_ARM64)
	#define SUS_ARCH_ARM
	#elif defined(__riscv) && __riscv_xlen == 64
	#define SUS_ARCH_RISCV
	#else
	#define SUS_ARCH_UNKNOWN
	#endif // !SUS_ARCH
	// System
	#if defined(_WIN32) || defined(_WIN64)
	#define SUS_SYSTEM_WINDOWS
	#elif defined(__linux__)
	#define SUS_SYSTEM_LINUX
	#elif defined(__APPLE__) || defined(__MACH__)
	#define SUS_SYSTEM_MACOS
	#else
	#define SUS_SYSTEM_EMBEDDED
	#endif // !_WIN32/__linux__/__APPLE__
	// Type of build
	#ifdef _DEBUG
	#define SUS_DEBUG
	#else
	#define SUS_RELEASE
	#endif // !_DEBUG/!SUS_RELEASE

	// -------------------------------------------------------------------

	#define SUS_STRINGIFY(x) #x
	#define SUS_TOSTRING(x) SUS_STRINGIFY(x)

	// -------------------------------------------------------------------

	using i8 = signed char;
	using u8 = unsigned char;
	using i16 = signed short;
	using u16 = unsigned short;
	using i32 = signed int;
	using u32 = unsigned int;
	using i64 = signed long long;
	using u64 = unsigned long long;
	using f32 = float;
	using f64 = double;
	using chr = char;
	using chr16 = char16_t;
	using chr32 = char32_t;
	using wchr = wchar_t;
	using usize = decltype(sizeof(0));
	using isize = decltype((char*)0 - (char*)0);

	// -------------------------------------------------------------------

	// The field may not occupy an area in memory.
	#define SUS_ATTRIB_NO_UNIQUE_ADDRESS [[no_unique_address]]
	// The attribute is required to be accepted
	#define SUS_ATTRIB_NODISCARD [[nodiscard]]
	// Attribute is the function stops executing
	#define SUS_ATTRIB_NORETURN [[noreturn]]
	// Attribute is no side effect, but reads memory
	#define SUS_ATTRIB_PURE [[gnu::pure]]
	// Attribute is no side effect
	#define SUS_ATTRIB_CONST [[gnu::const]]
	// Attribute is a highly loaded function
	#define SUS_ATTRIB_HOT [[gnu::hot]]
	// Attribute is a low-loaded function
	#define SUS_ATTRIB_COLD [[gnu::cold]]
	// Attribute - may not be used
	#define SUS_ATTRIB_MAYBE_UNUSED [[maybe_unused]]
	// Attribute - must be enabled
	#define SUS_ATTRIB_USED __attribute__((used))

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_UTILS_TYPES_ */
