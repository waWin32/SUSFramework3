// susfwk-arch-entry_point.cpp
//
/*
* =====---------------- entry_point.cpp - defining the entry point to the program ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====-------------------------------------------------------------------------------------------=====
*/
#include "susfwk/utils/types.hpp"
#include "susfwk/arch/exit.hpp"
#include "susfwk/core/platform.h"
#include "susfwk/core/pair.hpp"
#include "susfwk/core/string.hpp"
#include "susfwk/core/system_allocator.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// Get Command Line parameters
	static sus::pair<sus::u32, sus::string<sus::chr>*> get_command_line() {
#if defined(SUS_SYSTEM_WINDOWS)
		volatile sus::u32 wargc;
		LPWSTR* wargv = CommandLineToArgvW(GetCommandLineW(), (int*)&wargc);
		auto* args = static_cast<sus::string<sus::chr>*>(sus::sysheap_allocator::malloc(sizeof(sus::string<sus::chr>) * wargc));
		for (sus::u32 i = 0; i < wargc; ++i) {
			auto len = WideCharToMultiByte(CP_ACP, 0, wargv[i], -1, nullptr, 0, nullptr, nullptr);
			::new(&args[i]) sus::string<sus::chr>(len - 1);
			args[i].push_uninitialized<false>(len - 1);
			WideCharToMultiByte(CP_ACP, 0, wargv[i], -1, data(args[i]), len - 1, nullptr, nullptr);
		}
		LocalFree((HLOCAL)wargv);
		return sus::pair{ wargc, args };
#elif defined(SUS_SYSTEM_LINUX)

#elif defined(SUS_SYSTEM_MACOS)

#endif /* !SUS_SYSTEM */
	}
	// Clean up command line parameters
	static void free_command_line(sus::pair<sus::u32, sus::string<sus::chr>*>& args) {
		sus::destroy(args.second, args.first);
		sus::sysheap_allocator::free(args.second);
	}

	// -------------------------------------------------------------------

}

sus::i32 Main(sus::u32 argc, sus::string<sus::chr> argv[]);
void start() {
	auto args = sus::get_command_line();
	sus::i32 ret = Main(args.first, args.second);
	sus::free_command_line(args);
	sus::exit(ret);
}
