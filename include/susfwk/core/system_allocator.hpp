// susfwk-core-system_allocator.hpp
//
/*
* =====---------------- system_allocator.hpp - basic system allocator ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====-------------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_CORE_SYSTEM_ALLOCATOR_
#define _SUSFWK_CORE_SYSTEM_ALLOCATOR_

#include "susfwk/utils/core.hpp"
#include "susfwk/utils/allocator.hpp"
#include "susfwk/arch/exit.hpp"
#include "susfwk/core/platform.h"
#include "susfwk/core/errorsys.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// System allocator in the process heap
	class sysheap_allocator : public allocator_full_father<sysheap_allocator> {
	public:
		// Allocate new memory to the heap
		static void* allocate(sus::usize size)  {
			if (!size) return nullptr;
			void* block = nullptr;
			#ifdef SUS_SYSTEM_WINDOWS
			block = HeapAlloc(GetProcessHeap(), 0, (SIZE_T)size);
			#elif defined(SUS_SYSTEM_LINUX)

			#elif defined(SUS_SYSTEM_MACOS)

			#else

			#endif // !SUS_SYSTEM_WINDOWS/!SUS_SYSTEM_LINUX/!SUS_SYSTEM_MACOS
			if (!block) {
				set_last_error(sus::error{ sus::error::code::system_allocator_heap_allocation, sus::error::reason::memory, sus::error::severity::critical });
				sus::abort("Fatal memory allocation error");
			}
			return block;
		}
		// Free up memory from the heap
		static void deallocate(void* block) {
			assert(block != nullptr);
			bool success = false;
			#ifdef SUS_SYSTEM_WINDOWS
			success = (bool)HeapFree(GetProcessHeap(), 0, block);
			#elif defined(SUS_SYSTEM_LINUX)

			#elif defined(SUS_SYSTEM_MACOS)

			#else

			#endif // !SUS_SYSTEM_WINDOWS/!SUS_SYSTEM_LINUX/!SUS_SYSTEM_MACOS
			if (!success) {
				set_last_error(sus::error{ sus::error::code::system_allocator_heap_freeing, sus::error::reason::memory, sus::error::severity::critical });
				sus::abort("Fatal memory release error");
			}
		}
		// Redeploy heap memory
		inline static void* reallocate(void* block, sus::usize size) {
			if (!block) return sus::sysheap_allocator::allocate(size);
			if (!size) { sus::sysheap_allocator::deallocate(block); return nullptr; }
			void* newBlock = nullptr;
			#ifdef SUS_SYSTEM_WINDOWS
			newBlock = HeapReAlloc(GetProcessHeap(), 0, (LPVOID)block, size);
			#elif defined(SUS_SYSTEM_LINUX)

			#elif defined(SUS_SYSTEM_MACOS)

			#else

			#endif // !SUS_SYSTEM_WINDOWS/!SUS_SYSTEM_LINUX/!SUS_SYSTEM_MACOS
			if (!newBlock) {
				set_last_error(sus::error{ sus::error::code::system_allocator_heap_reallocation, sus::error::reason::memory, sus::error::severity::critical });
				sus::abort("Fatal memory relocation error");
			}
			return newBlock;
		}
	};

	// -------------------------------------------------------------------

	// System allocator to virtual memory
	class sysvrt_allocator : public allocator_base_father<sysvrt_allocator> {
	public:
		// Allocate memory to virtual system memory
		inline static void* allocate(sus::usize size){
			if (!size) return nullptr;
			void* block = nullptr;
			#ifdef SUS_SYSTEM_WINDOWS
			block = HeapAlloc(GetProcessHeap(), 0, (SIZE_T)size);
			#elif defined(SUS_SYSTEM_LINUX)

			#elif defined(SUS_SYSTEM_MACOS)

			#else

			#endif // !SUS_SYSTEM_WINDOWS/!SUS_SYSTEM_LINUX/!SUS_SYSTEM_MACOS
			if (!block) {
				set_last_error(sus::error{ sus::error::code::system_allocator_heap_allocation, sus::error::reason::memory, sus::error::severity::critical });
				sus::abort("Fatal memory allocation error");
			}
			return block;
		}
		// Free memory from the virtual system memory
		inline static void deallocate(void* block) {
			assert(block != nullptr);
			bool success = false;
			#ifdef SUS_SYSTEM_WINDOWS
			success = (bool)VirtualFree(block, 0, MEM_RELEASE);
			#elif defined(SUS_SYSTEM_LINUX)

			#elif defined(SUS_SYSTEM_MACOS)

			#else

			#endif // !SUS_SYSTEM_WINDOWS/!SUS_SYSTEM_LINUX/!SUS_SYSTEM_MACOS
			if (!success) {
				set_last_error(sus::error{ sus::error::code::system_allocator_virtual_freeing, sus::error::reason::memory, sus::error::severity::critical });
				sus::abort("Fatal memory release error");
			}
		}
	};

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_CORE_SYSTEM_ALLOCATOR_ */
