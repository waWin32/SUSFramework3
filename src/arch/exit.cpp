// susfwk-arch-exit.cpp
//
/*
* =====---------------- exit.cpp - program exit functions ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====-------------------------------------------------------------------=====
*/
#include "platform.h"
#include "susfwk/utils/types.hpp"
#include "susfwk/arch/exit.hpp"
#include "susfwk/arch/global_init.hpp"

namespace sus {

	// -------------------------------------------------------------------

#ifdef SUS_DEBUG
#if defined(SUS_SYSTEM_WINDOWS)
	LONG CreateMiniDump(EXCEPTION_POINTERS* pExceptionInfo) {
		HANDLE hFile = CreateFile("crash.dmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		MINIDUMP_EXCEPTION_INFORMATION mdei;
		mdei.ThreadId = GetCurrentThreadId();
		mdei.ExceptionPointers = pExceptionInfo;
		mdei.ClientPointers = FALSE;
		MiniDumpWriteDump(
			GetCurrentProcess(),
			GetCurrentProcessId(),
			hFile,
			(MINIDUMP_TYPE)(
			MiniDumpWithFullMemory |
			MiniDumpWithFullMemoryInfo | 
			MiniDumpWithHandleData |
			MiniDumpWithUnloadedModules |
			MiniDumpWithProcessThreadData |
			MiniDumpWithThreadInfo
			),
			&mdei,
			NULL,
			NULL
		);
		CloseHandle(hFile);
		return 0;
	}
	void set_unhandled_exception_filter() {
		SetUnhandledExceptionFilter(CreateMiniDump);
	}
	SUS_DEFINE_GLOBAL_CONSTRUCTOREX(set_unhandled_exception_filter, A)
#endif /* !SUS_SYSTEM */
#endif // !SUS_DEBUG

	// Emergency exit from the program
	SUS_ATTRIB_NORETURN void abort(const sus::chr* msg) {
		#if defined(SUS_SYSTEM_WINDOWS)
		OutputDebugStringA((LPCSTR)msg);
		DebugBreak();
		TerminateProcess(GetCurrentProcess(), STATUS_FATAL_APP_EXIT);
		#elif defined(SUS_SYSTEM_LINUX)

		#elif defined(SUS_SYSTEM_MACOS)

		#endif // !SUS_SYSTEM
		__builtin_unreachable();
	}
	// Safe exit from the program
	SUS_ATTRIB_NORETURN void exit(sus::i32 code) {
		#if defined(SUS_SYSTEM_WINDOWS)
		ExitProcess((UINT)code);
		#elif defined(SUS_SYSTEM_LINUX)

		#elif defined(SUS_SYSTEM_MACOS)

		#endif // !SUS_SYSTEM
		__builtin_unreachable();
	}

	// -------------------------------------------------------------------

}
