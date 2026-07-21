; susfwk-arch-strops_backend.asm
;
; =====------------------- strops_backend.asm - simd string functions -------------------=====
;
; Part of the SUSFramework3 Project, under the MIT License
; See LICENSE.txt for license information.
; SPDX-License-Identifier: MIT
;
; =====----------------------------------------------------------------------------------=====

%include "x86_64/asmfwk.inc"

default rel

section .text
 
; ---------------------------------------------------------------------

section .rodata
align 16
const_32_16_16: times 16 db 32
const_8_16_16: times 16 db 8
const_14_16_16: times 16 db 14

%macro striml_check_block 0
	movdqa xmm5, xmm0
	movdqa xmm6, xmm3
	pcmpgtb xmm5, xmm2
	pcmpgtb xmm6, xmm0
	pcmpeqb xmm0, xmm1
	pand xmm5, xmm6
	por xmm0, xmm5
	pxor xmm0, xmm4
	pmovmskb rd_temp1, xmm0
	test rd_temp1, rd_temp1
%endmacro
; ri_v1 = str
; rq_uni = return str
SUS_PUBLIC_FUNC striml_
	.begin:
		mov rb_temp1, [ri_v1]
		cmp rb_temp1, 32
		je .has_spaces
		cmp rb_temp1, 9
		jb .early_return
		cmp rb_temp1, 13
		ja .early_return
	.has_spaces:
		inc ri_v1
		mov rq_uni, ri_v1
		movdqa xmm1, [const_32_16_16]
		movdqa xmm2, [const_8_16_16]
		movdqa xmm3, [const_14_16_16]
		pcmpeqb xmm4, xmm4
		%if SUS_ARCH_SYS_WINDOWS
			sub rsp, 16
			movdqu [rsp + 16 * 0], xmm6
		%endif
	.simd_lbegin:
		neg ri_v1
		and ri_v1, 16 - 1
		jz .simd_lbegin_entry
		movdqu xmm0, [rq_uni]
		striml_check_block
		jnz .simd_ldone
		add rq_uni, ri_v1
		jmp .simd_lbegin_entry
	.simd_loop:
		add rq_uni, 16
	.simd_lbegin_entry:
		movdqa xmm0, [rq_uni]
		striml_check_block
		jz .simd_loop
	.simd_ldone:
		%if SUS_ARCH_SYS_WINDOWS
			movdqu [rsp + 16 * 0], xmm6
			add rsp, 16
		%endif
		rep bsf rd_temp1, rd_temp1
		movzx rq_temp1, rd_temp1
		add rq_uni, rq_temp1
	.return:
		ret
	.early_return:
		mov rq_uni, ri_v1
		ret
	.end:

; ---------------------------------------------------------------------

section .rodata
align 16
const_32_8_16: times 8 db 32
const_8_8_16: times 8 db 8
const_14_8_16: times 8 db 14

%macro striml16_check_block 0
	movdqa xmm5, xmm0
	movdqa xmm6, xmm3
	pcmpgtw xmm5, xmm2
	pcmpgtw xmm6, xmm0
	pcmpeqw xmm0, xmm1
	pand xmm5, xmm6
	por xmm0, xmm5
	pxor xmm0, xmm4
	pmovmskb rd_temp1, xmm0
	test rd_temp1, rd_temp1
%endmacro
; ri_v1 = str
; rq_uni = return str
SUS_PUBLIC_FUNC striml16_
	.begin:
		mov rw_temp1, [ri_v1]
		cmp rw_temp1, 32
		je .has_spaces
		cmp rw_temp1, 9
		jb .early_return
		cmp rw_temp1, 13
		ja .early_return
	.has_spaces:
		add ri_v1, 2
		mov rq_uni, ri_v1
		movdqa xmm1, [const_32_8_16]
		movdqa xmm2, [const_8_8_16]
		movdqa xmm3, [const_14_8_16]
		pcmpeqb xmm4, xmm4
		%if SUS_ARCH_SYS_WINDOWS
			sub rsp, 16
			movdqu [rsp + 16 * 0], xmm6
		%endif
	.simd_lbegin:
		neg ri_v1
		and ri_v1, 16 - 1
		jz .simd_lbegin_entry
		movdqu xmm0, [rq_uni]
		striml16_check_block
		jnz .simd_ldone
		add rq_uni, ri_v1
		jmp .simd_lbegin_entry
	.simd_loop:
		add rq_uni, 16
	.simd_lbegin_entry:
		movdqa xmm0, [rq_uni]
		striml16_check_block
		jz .simd_loop
	.simd_ldone:
		%if SUS_ARCH_SYS_WINDOWS
			movdqu [rsp + 16 * 0], xmm6
			add rsp, 16
		%endif
		rep bsf rd_temp1, rd_temp1
		and rd_temp1, ~1
		movzx rq_temp1, rd_temp1
		add rq_uni, rq_temp1
	.return:
		ret
	.early_return:
		mov rq_uni, ri_v1
		ret
	.end:

; ---------------------------------------------------------------------

section .rodata
align 16
const_32_4_16: times 4 db 32
const_8_4_16: times 4 db 8
const_14_4_16: times 4 db 14

%macro striml32_check_block 0
	movdqa xmm5, xmm0
	movdqa xmm6, xmm3
	pcmpgtd xmm5, xmm2
	pcmpgtd xmm6, xmm0
	pcmpeqd xmm0, xmm1
	pand xmm5, xmm6
	por xmm0, xmm5
	pxor xmm0, xmm4
	pmovmskb rd_temp1, xmm0
	test rd_temp1, rd_temp1
%endmacro
; ri_v1 = str
; rq_uni = return str
SUS_PUBLIC_FUNC striml32_
	.begin:
		mov rd_temp1, [ri_v1]
		cmp rd_temp1, 32
		je .has_spaces
		cmp rd_temp1, 9
		jb .early_return
		cmp rd_temp1, 13
		ja .early_return
	.has_spaces:
		add ri_v1, 4
		mov rq_uni, ri_v1
		movdqa xmm1, [const_32_4_16]
		movdqa xmm2, [const_8_4_16]
		movdqa xmm3, [const_14_4_16]
		pcmpeqb xmm4, xmm4
		%if SUS_ARCH_SYS_WINDOWS
			sub rsp, 16
			movdqu [rsp + 16 * 0], xmm6
		%endif
	.simd_lbegin:
		neg ri_v1
		and ri_v1, 16 - 1
		jz .simd_lbegin_entry
		movdqu xmm0, [rq_uni]
		striml32_check_block
		jnz .simd_ldone
		add rq_uni, ri_v1
		jmp .simd_lbegin_entry
	.simd_loop:
		add rq_uni, 16
	.simd_lbegin_entry:
		movdqa xmm0, [rq_uni]
		striml32_check_block
		jz .simd_loop
	.simd_ldone:
		%if SUS_ARCH_SYS_WINDOWS
			movdqu [rsp + 16 * 0], xmm6
			add rsp, 16
		%endif
		rep bsf rd_temp1, rd_temp1
		and rd_temp1, ~3
		movzx rq_temp1, rd_temp1
		add rq_uni, rq_temp1
	.return:
		ret
	.early_return:
		mov rq_uni, ri_v1
		ret
	.end:

; ---------------------------------------------------------------------
