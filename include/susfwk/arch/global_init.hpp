// susfwk-arch-global_init.hpp
//
/*
* =====---------------- global_init.hpp - calling constructors and destructors from the loader ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====--------------------------------------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_ARCH_GLOBAL_INIT_
#define _SUSFWK_ARCH_GLOBAL_INIT_

#include "susfwk/utils/core.hpp"

namespace sus::callback { using init_func = void(*)(); }

extern "C" {

#ifdef SUS_SYSTEM_WINDOWS

	// -------------------------------------------------------------------

	#pragma comment(linker, "/INCLUDE:_tls_used") // Mandatory inclusion

	extern sus::callback::init_func __global_init_a;
	extern sus::callback::init_func __global_init_z;
	extern sus::callback::init_func __global_cleanup_a;
	extern sus::callback::init_func __global_cleanup_z;
	extern sus::callback::init_func __tls_init_a;
	extern sus::callback::init_func __tls_init_z;
	extern sus::callback::init_func __tls_cleanup_a;
	extern sus::callback::init_func __tls_cleanup_z;
	inline sus::u8 __tls_guard = 1;
	SUS_ATTRIB_USED inline sus::i32 _fltused = 1;
	void __call_global_initializers();
	void __call_global_cleanup();
	void __call_tls_initializers();
	void __call_tls_cleanup();
	extern sus::u32 _tls_index;
	inline int atexit(void (*)(void)) { return 0; }
	extern "C" int __dyn_tls_init();
	inline void __tlregdtor(void(*)(void)) {}
	inline int __dyn_tls_on_demand_init() { return 0; }
	extern "C" inline int __cdecl _purecall() { return 0; }
	extern "C" void __chkstk();

	// -------------------------------------------------------------------

	// Declare the gloabl class
	#define SUS_DEFINE_BASE_GLOBAL(Type, Name) inline Type Name;
	#define SUS_DEFINE_GLOBALEX(Type, Name, priority, ...)\
		SUS_DEFINE_BASE_GLOBAL(Type, Name)\
		SUS_ATTRIB_USED inline void __tinit_##Name() { ::new (&Name) Type(__VA_ARGS__); }\
		SUS_ATTRIB_USED inline void __tclean_##Name() { (&Name)->~Type(); }\
		extern "C" {\
			inline volatile SUS_ATTRIB_USED sus::callback::init_func __tinit_ptr_##Name __attribute__((section(".CRT$RGC" #priority), aligned(8))) = __tinit_##Name;\
			inline volatile SUS_ATTRIB_USED sus::callback::init_func __tclean_ptr_##Name __attribute__((section(".CRT$RGD" #priority), aligned(8))) = __tclean_##Name;\
		}
	// Define the gloabl class
	#define SUS_DEFINE_GLOBAL(Type, Name, ...) SUS_DEFINE_GLOBALEX(Type, Name, X, __VA_ARGS__)

	// Declare a function as a constructor
	#define SUS_DEFINE_GLOBAL_CONSTRUCTOREX(Name, priority) extern "C" {\
		inline volatile SUS_ATTRIB_USED sus::callback::init_func __tinit_ptr_##Name __attribute__((section(".CRT$RGC" #priority), aligned(8))) = Name;\
	}
	// Declare a function as a constructor
	#define SUS_DEFINE_GLOBAL_CONSTRUCTOR(Name) SUS_DEFINE_GLOBAL_CONSTRUCTOREX(Name, X)
	// Declare a function as a destructor
	#define SUS_DEFINE_GLOBAL_DESTRUCTOREX(Name, priority) extern "C" {\
		inline volatile SUS_ATTRIB_USED sus::callback::init_func __tinit_ptr_##Name __attribute__((section(".CRT$RGD" #priority), aligned(8))) = Name;\
	}
	// Declare a function as a destructor
	#define SUS_DEFINE_GLOBAL_DESTRUCTOR(Name) SUS_DEFINE_GLOBAL_DESTRUCTOREX(Name, X)

	// -------------------------------------------------------------------

	#define SUS_DEFINE_BASE_TLS(Type, Name) inline __attribute__((tls_model("local-exec"))) thread_local Type Name;
	// Declare a TLS class
	#define SUS_DEFINE_TLSEX(Type, Name, priority, ...)\
		SUS_DEFINE_BASE_TLS(Type, Name)\
		SUS_ATTRIB_USED inline void __tinit_##Name() { ::new (&Name) Type(__VA_ARGS__); }\
		SUS_ATTRIB_USED inline void __tclean_##Name() { (&Name)->~Type(); }\
		extern "C" {\
			inline volatile SUS_ATTRIB_USED sus::callback::init_func __tinit_ptr_##Name __attribute__((section(".CRT$RTC" #priority), aligned(8))) = __tinit_##Name;\
			inline volatile SUS_ATTRIB_USED sus::callback::init_func __tclean_ptr_##Name __attribute__((section(".CRT$RTD" #priority), aligned(8))) = __tclean_##Name;\
		}
	// Define the TLS class
	#define SUS_DEFINE_TLS(Type, Name, ...) SUS_DEFINE_TLSEX(Type, Name, X, __VA_ARGS__)
	
	// Declare a function as a constructor
	#define SUS_DEFINE_TLS_CONSTRUCTOREX(Name, priority) extern "C" {\
		inline volatile SUS_ATTRIB_USED sus::callback::init_func __tinit_ptr_##Name __attribute__((section(".CRT$RTC" #priority), aligned(8))) = Name;\
	}
	// Declare a function as a constructor
	#define SUS_DEFINE_TLS_CONSTRUCTOR(Name) SUS_DEFINE_TLS_CONSTRUCTOREX(Name, X)
	// Declare a function as a destructor
	#define SUS_DEFINE_TLS_DESTRUCTOREX(Name, priority) extern "C" {\
		inline volatile SUS_ATTRIB_USED sus::callback::init_func __tinit_ptr_##Name __attribute__((section(".CRT$RTD" #priority), aligned(8))) = Name;\
	}
	// Declare a function as a destructor
	#define SUS_DEFINE_TLS_DESTRUCTOR(Name) SUS_DEFINE_TLS_DESTRUCTOREX(Name, X)

	// -------------------------------------------------------------------

#else

	// -------------------------------------------------------------------

	// todo

	// -------------------------------------------------------------------

#endif // !SUS_SYSTEM

}

#endif /* !_SUSFWK_ARCH_GLOBAL_INIT_ */
