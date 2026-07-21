// susfwk-core-errorsys.hpp
//
/*
* =====---------------- errorsys.hpp - std error handling system ----------------=====
*
* Part of ths SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====--------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_CORE_ERRORSYS_
#define _SUSFWK_CORE_ERRORSYS_

#include "susfwk/utils/core.hpp"
#include "susfwk/arch/global_init.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// Identify the beginning of the error section
	#define SUS_ERROR_CODE(moduleName, error) moduleName##_##error
	// Identify the beginning of the error section
	#define SUS_ERROR_CODE_SECTION(moduleName) moduleName##_section
	// Identify the end of the error section
	#define SUS_ERROR_CODE_SECTION_END(moduleName) moduleName##_section_end
	// Determine if the error belongs to this section
	#define SUS_ERROR_SECTION(errorCode, sectionName) (((errorCode) > error_code::SUS_ERROR_CODE_SECTION(sectionName)) && ((errorCode) < error_code::SUS_ERROR_CODE_SECTION_END(sectionName)))

	// Error structure
	struct error {
	public:
		// The specific error code
		enum class code : sus::u16 {
			success,
			SUS_ERROR_CODE_SECTION(system_allocator),
			SUS_ERROR_CODE(system_allocator, heap_allocation),
			SUS_ERROR_CODE(system_allocator, heap_reallocation),
			SUS_ERROR_CODE(system_allocator, heap_freeing),
			SUS_ERROR_CODE(system_allocator, virtual_allocation),
			SUS_ERROR_CODE(system_allocator, virtual_freeing),
			SUS_ERROR_CODE_SECTION_END(system_allocator),
			SUS_ERROR_CODE_SECTION(iostream),
			SUS_ERROR_CODE(iostream, file_open),
			SUS_ERROR_CODE(iostream, temp_open),
			SUS_ERROR_CODE(iostream, file_seek),
			SUS_ERROR_CODE(iostream, file_size),
			SUS_ERROR_CODE(iostream, invalid_input_operation),
			SUS_ERROR_CODE_SECTION_END(iostream)
		} code_ = code::success;
		// The type of error is a quick way to identify the cause of the error
		enum class reason : sus::u8 {
			general,	// Common error - the cause is unknown
			memory,		// The error occurred while working with memory
			system,		// The error occurred when calling the system function
			resource,	// The error occurred while working with the resource
			syntax		// Syntax parsing error
		} reason_ = reason::general;
		// Severity of an error - quickly determining the criticality of an error
		enum class severity : sus::u8 {
			common,		// A common error
			warning,	// Warning
			critical,	// A critical error, such as a memory allocation error, requires logging, processing, and possibly crashing
			fatal		// A fatal error, such as panic, requires a decision to be made: continue execution (but with restrictions) or immediately terminate execution.
		} severity_ = severity::common;
		// The context code
		sus::u32 extra_ = 0;
	public:
		constexpr error() = default;
		constexpr error(code c, reason r = reason::general, severity s = severity::common, sus::u32 e = 0)
			: code_(c), reason_(r), severity_(s), extra_(e) {
		}
		constexpr ~error() = default;
		constexpr bool is_success() const { return code_ == code::success; }
		constexpr bool is_error() const { return code_ != code::success; }
	};

	// -------------------------------------------------------------------

	// Global in thread last error
	SUS_DEFINE_TLSEX(error, g_lastError, A)
	using error_handler = void(*)(sus::error);
	// Global Error Handler
	SUS_DEFINE_BASE_TLS(error_handler, g_error_handler)

	// Set an error
	inline void set_last_error(const sus::error& err) {
		if (g_error_handler) g_error_handler(err);
		g_lastError = err;
	}
	// Get the latest error
	inline sus::error get_last_error() { return g_lastError; }

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_CORE_ERRORSYS_ */
