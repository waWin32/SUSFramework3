; susfwk-arch-memops.inc
;
; =====------------------- memops.inc - std memroy operation -------------------=====
;
; Part of the SUSFramework3 Project, under the MIT License
; See LICENSE.txt for license information.
; SPDX-License-Identifier: MIT
;
; =====-------------------------------------------------------------------------=====

%include "x86_64/asmfwk.inc"
%include "x86_64/memops_backend.inc"

default rel

section .rodata align=8
align 8
memops_sse2_table:
	dq memcpy_sse2
    dq memcpybkw_sse2
    dq memmove_sse2
    dq memset_sse2
    dq memset16_sse2
    dq memset32_sse2
    dq memset64_sse2
    dq memcmp_sse2
    dq memcmp16_sse2
    dq memcmp32_sse2
    dq memcmp64_sse2
    dq memchr_sse2
    dq memchr16_sse2
    dq memchr32_sse2
    dq memchr64_sse2
    dq strlen_sse2
    dq strlen16_sse2
    dq strlen32_sse2
    dq strcpy_sse2
    dq strcpy16_sse2
    dq strcpy32_sse2
align 8
memops_avx2_table:
    dq memcpy_avx2
    dq memcpybkw_avx2
    dq memmove_avx2
    dq memset_avx2
    dq memset16_avx2
    dq memset32_avx2
    dq memset64_avx2
    dq memcmp_avx2
    dq memcmp16_avx2
    dq memcmp32_avx2
    dq memcmp64_avx2
    dq memchr_avx2
    dq memchr16_avx2
    dq memchr32_avx2
    dq memchr64_avx2
    dq strlen_avx2
    dq strlen16_avx2
    dq strlen32_avx2
    dq strcpy_avx2
    dq strcpy16_avx2
    dq strcpy32_avx2

; ---------------------------------------------------------------------

section .bss align=8
align 8
memops_backend:
    resq (memops_avx2_table - memops_sse2_table) / 8

section .text

SUS_PUBLIC_FUNC resolve_backend
    push rq_save1
    xor rd_uni, rd_uni
    cpuid
    cmp rd_uni, 7
    jb .use_sse2
    mov rd_uni, 7
    xor ecx, ecx
    cpuid
    test rd_save1, 1 << 5
    jnz .use_avx2
.use_sse2:
    pop rq_save1
    lea ri_v1, [memops_backend]
    lea ri_v2, [memops_sse2_table]
    mov ri_v3, memops_avx2_table - memops_sse2_table
    jmp memcpy_sse2
.use_avx2:
    pop rq_save1
    lea ri_v1, [memops_backend]
    lea ri_v2, [memops_avx2_table]
    mov ri_v3, memops_avx2_table - memops_sse2_table
    jmp memcpy_avx2
section .CRT$RGC9 rdata align=8
    dq resolve_backend

; ---------------------------------------------------------------------

section .text

%macro define_func 2
    SUS_PUBLIC_FUNC %1
        mov rq_uni, [rel memops_backend + %2]
        jmp rq_uni
%endmacro
%macro define_wchar_func 2
    SUS_PUBLIC_FUNC %1
        %ifdef SUS_ARCH_SYS_LINUX
        mov rq_uni, [rel memops_backend + %2 + 16]
        %else
        mov rq_uni, [rel memops_backend + %2 + 8]
        %endif 
        jmp rq_uni
%endmacro

define_func memcpy, 0
define_func memcpybkw, 8
define_func memmove, 16
define_func memset, 24
define_func memset16, 32
define_func memset32, 40
define_func memset64, 48
define_wchar_func wmemset, 24
define_func memcmp, 56
define_func memcmp16, 64
define_func memcmp32, 72
define_func memcmp64, 80
define_wchar_func wmemcmp, 56
define_func memchr_, 88
define_func memchr16, 96
define_func memchr32, 104
define_func memchr64, 112
define_wchar_func wmemchr, 88
define_func strlen, 120
define_func strlen16, 128
define_func strlen32, 136
define_wchar_func wcslen, 120
define_func strcpy, 144
define_func strcpy16, 152
define_func strcpy32, 160
define_wchar_func wcscpy, 144

; ---------------------------------------------------------------------
