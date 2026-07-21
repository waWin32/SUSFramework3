; susfwk-arch-memops_backend.inc
;
; =====------------------- memops_backend.inc - std memroy operation backend -------------------=====
;
; Part of the SUSFramework3 Project, under the MIT License
; See LICENSE.txt for license information.
; SPDX-License-Identifier: MIT
;
; =====-----------------------------------------------------------------------------------------=====

%ifndef _SUSFWK_ARCH_MEMOPS_BACKEND_INC_
%define _SUSFWK_ARCH_MEMOPS_BACKEND_INC_

; ---------------------------------------------------------------------

extern memcpy_sse2
extern memcpy_avx2
extern memcpy_avx512

extern memcpybkw_sse2
extern memcpybkw_avx2
extern memcpybkw_avx512

extern memmove_sse2
extern memmove_avx2
extern memmove_avx512

extern memset_sse2
extern memset_avx2
extern memset_avx512

extern memset16_sse2
extern memset16_avx2
extern memset16_avx512

extern memset32_sse2
extern memset32_avx2
extern memset32_avx512

extern memset64_sse2
extern memset64_avx2
extern memset64_avx512

extern memcmp_sse2
extern memcmp_avx2
extern memcmp_avx512

extern memcmp16_sse2
extern memcmp16_avx2
extern memcmp16_avx512

extern memcmp32_sse2
extern memcmp32_avx2
extern memcmp32_avx512

extern memcmp64_sse2
extern memcmp64_avx2
extern memcmp64_avx512

extern memchr_sse2
extern memchr_avx2
extern memchr_avx512

extern memchr16_sse2
extern memchr16_avx2
extern memchr16_avx512

extern memchr32_sse2
extern memchr32_avx2
extern memchr32_avx512

extern memchr64_sse2
extern memchr64_avx2
extern memchr64_avx512
		
; ---------------------------------------------------------------------

extern strlen_sse2
extern strlen_avx2
extern strlen_avx512

extern strlen16_sse2
extern strlen16_avx2
extern strlen16_avx512

extern strlen32_sse2
extern strlen32_avx2
extern strlen32_avx512

extern strcpy_sse2
extern strcpy_avx2
extern strcpy_avx512

extern strcpy16_sse2
extern strcpy16_avx2
extern strcpy16_avx512

extern strcpy32_sse2
extern strcpy32_avx2
extern strcpy32_avx512

; ---------------------------------------------------------------------

%endif ; !_SUSFWK_ARCH_MEMOPS_BACKEND_INC_
