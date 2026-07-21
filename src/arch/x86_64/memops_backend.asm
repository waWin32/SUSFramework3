; susfwk-arch-memops_backend.asm
;
; =====------------------- memops_backend.asm - std memroy operation backend -------------------=====
;
; Part of the SUSFramework3 Project, under the MIT License
; See LICENSE.txt for license information.
; SPDX-License-Identifier: MIT
;
; =====------------------------------------------------------------------------------------------------=====

%include "x86_64/asmfwk.inc"

default rel

section .text
 
; ---------------------------------------------------------------------

SUS_PRIVATE_FUNC memcpy_fallback
	.begin:
	.qword_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 3
		jz .qword_ldone
	.qword_loop:
		mov rq_temp1, [ri_v2]
		mov [ri_v1], rq_temp1
		add ri_v1, 8
		add ri_v2, 8
		dec ri_v4
		jnz .qword_loop
		and ri_v3, 7
	.qword_ldone:
	.byte_lbegin:
		test ri_v3, ri_v3
		jz .byte_ldone
	.byte_loop:
		mov rb_temp1, [ri_v2]
		mov [ri_v1], rb_temp1
		inc ri_v1
		inc ri_v2
		dec ri_v3
		jnz .byte_loop
	.byte_ldone:
		ret
	.end:

; ri_v1 = dest
; ri_v2 = src
; ri_v3 = count
; rq_uni = return dest
SUS_PUBLIC_FUNC memcpy_sse2
	.begin:
		mov rq_uni, ri_v1
		cmp ri_v3, 32
		jb memcpy_fallback
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 16 - 1
		jz .align_done
		movdqu xmm0, [ri_v2]
		movdqu [ri_v1], xmm0
		add ri_v1, ri_v4
		add ri_v2, ri_v4
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		%if SUS_ARCH_SYS_WINDOWS
			sub rsp, 32
			movdqu [rsp + 0], xmm6
			movdqu [rsp + 16], xmm7
		%endif
		shr ri_v4, 4 + 3
		jz .simd_ltail
	.simd_loop:
		movdqu xmm0, [ri_v2 + 16 * 0]
		movdqu xmm1, [ri_v2 + 16 * 1]
		movdqu xmm2, [ri_v2 + 16 * 2]
		movdqu xmm3, [ri_v2 + 16 * 3]
		movdqu [ri_v1 + 16 * 0], xmm0
		movdqu [ri_v1 + 16 * 1], xmm1
		movdqu [ri_v1 + 16 * 2], xmm2
		movdqu [ri_v1 + 16 * 3], xmm3
		movdqu xmm4, [ri_v2 + 16 * 4]
		movdqu xmm5, [ri_v2 + 16 * 5]
		movdqu xmm6, [ri_v2 + 16 * 6]
		movdqu xmm7, [ri_v2 + 16 * 7]
		movdqu [ri_v1 + 16 * 4], xmm4
		movdqu [ri_v1 + 16 * 5], xmm5
		movdqu [ri_v1 + 16 * 6], xmm6
		movdqu [ri_v1 + 16 * 7], xmm7
		add ri_v1, 16 * 8
		add ri_v2, 16 * 8
		dec ri_v4
		jnz .simd_loop
		and ri_v3, 16 * 8 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		shr ri_v4, 4
		lea rq_temp1, [.simd_tail_jump_table]
		jmp [rq_temp1 + ri_v4 * 8]
	.simd_tail_jump_table:
		dq .simd_ldone, .simd_tail_b1, .simd_tail_b2, .simd_tail_b3, .simd_tail_b4, .simd_tail_b5, .simd_tail_b6, .simd_tail_b7
		.simd_tail_b7:
			movdqu xmm6, [ri_v2 + 16 * 6]
			movdqu [ri_v1 + 16 * 6], xmm6
		.simd_tail_b6:
			movdqu xmm5, [ri_v2 + 16 * 5]
			movdqu [ri_v1 + 16 * 5], xmm5
		.simd_tail_b5:
			movdqu xmm4, [ri_v2 + 16 * 4]
			movdqu [ri_v1 + 16 * 4], xmm4
		.simd_tail_b4:
			movdqu xmm3, [ri_v2 + 16 * 3]
			movdqu [ri_v1 + 16 * 3], xmm3
		.simd_tail_b3:
			movdqu xmm2, [ri_v2 + 16 * 2]
			movdqu [ri_v1 + 16 * 2], xmm2
		.simd_tail_b2:
			movdqu xmm1, [ri_v2 + 16 * 1]
			movdqu [ri_v1 + 16 * 1], xmm1
		.simd_tail_b1:
			movdqu xmm0, [ri_v2 + 16 * 0]
			movdqu [ri_v1 + 16 * 0], xmm0
		shl ri_v4, 4
		add ri_v1, ri_v4
		add ri_v2, ri_v4
		and ri_v3, 16 - 1
	.simd_ldone:
		%if SUS_ARCH_SYS_WINDOWS
			movdqu xmm6, [rsp+0]
			movdqu xmm7, [rsp+16]
			add rsp, 32
		%endif
		jmp memcpy_fallback
	.end:

; ri_v1 = dest
; ri_v2 = src
; ri_v3 = count
; rq_uni = return dest
SUS_PUBLIC_FUNC memcpy_avx2
	.begin:
		cmp ri_v3, 128
		jb memcpy_sse2
		mov rq_uni, ri_v1
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 32 - 1
		jz .align_done
		vmovdqu ymm0, [ri_v2]
		vmovdqu [ri_v1], ymm0
		add ri_v1, ri_v4
		add ri_v2, ri_v4
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		%if SUS_ARCH_SYS_WINDOWS
			sub rsp, 32
			movdqu [rsp + 0], xmm6
			movdqu [rsp + 16], xmm7
		%endif
		shr ri_v4, 5 + 3
		jz .simd_ltail
	.simd_loop:
		vmovdqu ymm0, [ri_v2 + 32 * 0]
		vmovdqu ymm1, [ri_v2 + 32 * 1]
		vmovdqu ymm2, [ri_v2 + 32 * 2]
		vmovdqu ymm3, [ri_v2 + 32 * 3]
		vmovdqu [ri_v1 + 32 * 0], ymm0
		vmovdqu [ri_v1 + 32 * 1], ymm1
		vmovdqu [ri_v1 + 32 * 2], ymm2
		vmovdqu [ri_v1 + 32 * 3], ymm3
		vmovdqu ymm4, [ri_v2 + 32 * 4]
		vmovdqu ymm5, [ri_v2 + 32 * 5]
		vmovdqu ymm6, [ri_v2 + 32 * 6]
		vmovdqu ymm7, [ri_v2 + 32 * 7]
		vmovdqu [ri_v1 + 32 * 4], ymm4
		vmovdqu [ri_v1 + 32 * 5], ymm5
		vmovdqu [ri_v1 + 32 * 6], ymm6
		vmovdqu [ri_v1 + 32 * 7], ymm7
		add ri_v1, 32 * 8
		add ri_v2, 32 * 8
		dec ri_v4
		jnz .simd_loop
		and ri_v3, 32 * 8 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		shr ri_v4, 5
		lea rq_temp1, [.simd_tail_jump_table]
		jmp [rq_temp1 + ri_v4 * 8]
	.simd_tail_jump_table:
		dq .simd_ldone, .simd_tail_b1, .simd_tail_b2, .simd_tail_b3, .simd_tail_b4, .simd_tail_b5, .simd_tail_b6, .simd_tail_b7
		.simd_tail_b7:
			vmovdqu ymm6, [ri_v2 + 32 * 6]
			vmovdqu [ri_v1 + 32 * 6], ymm6
		.simd_tail_b6:
			vmovdqu ymm5, [ri_v2 + 32 * 5]
			vmovdqu [ri_v1 + 32 * 5], ymm5
		.simd_tail_b5:
			vmovdqu ymm4, [ri_v2 + 32 * 4]
			vmovdqu [ri_v1 + 32 * 4], ymm4
		.simd_tail_b4:
			vmovdqu ymm3, [ri_v2 + 32 * 3]
			vmovdqu [ri_v1 + 32 * 3], ymm3
		.simd_tail_b3:
			vmovdqu ymm2, [ri_v2 + 32 * 2]
			vmovdqu [ri_v1 + 32 * 2], ymm2
		.simd_tail_b2:
			vmovdqu ymm1, [ri_v2 + 32 * 1]
			vmovdqu [ri_v1 + 32 * 1], ymm1
		.simd_tail_b1:
			vmovdqu ymm0, [ri_v2 + 32 * 0]
			vmovdqu [ri_v1 + 32 * 0], ymm0
		shl ri_v4, 5
		add ri_v1, ri_v4
		add ri_v2, ri_v4
		and ri_v3, 32 - 1
	.simd_ldone:
		%if SUS_ARCH_SYS_WINDOWS
			movdqu xmm6, [rsp+0]
			movdqu xmm7, [rsp+16]
			add rsp, 32
		%endif
		vzeroupper
		jmp memcpy_fallback
	.end:
	
; ---------------------------------------------------------------------

SUS_PRIVATE_FUNC memcpybkw_fallback
	.begin:
	.qword_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 3
		jz .qword_ldone
	.qword_loop:
		sub ri_v1, 8
		sub ri_v2, 8
		mov rq_temp1, [ri_v2]
		mov [ri_v1], rq_temp1
		dec ri_v4
		jnz .qword_loop
		and ri_v3, 7
	.qword_ldone:
	.byte_lbegin:
		test ri_v3, ri_v3
		jz .byte_ldone
	.byte_loop:
		dec ri_v1
		dec ri_v2
		mov rb_temp1, [ri_v2]
		mov [ri_v1], rb_temp1
		dec ri_v3
		jnz .byte_loop
	.byte_ldone:
		mov rq_uni, ri_v1
		ret
	.end:

; ri_v1 = dest (end)
; ri_v2 = src (end)
; ri_v3 = count
; rq_uni = return dest
SUS_PUBLIC_FUNC memcpybkw_sse2
	.begin:
		cmp ri_v3, 32
		jb memcpybkw_fallback
	.align:
		mov ri_v4, ri_v1
		and ri_v4, 16 - 1
		jz .align_done
		movdqu xmm0, [ri_v2 - 16]
		movdqu [ri_v1 - 16], xmm0
		sub ri_v1, ri_v4
		sub ri_v2, ri_v4
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		%if SUS_ARCH_SYS_WINDOWS
			sub rsp, 32
			movdqu [rsp + 0], xmm6
			movdqu [rsp + 16], xmm7
		%endif
		shr ri_v4, 4 + 3
		jz .simd_ltail
	.simd_loop:
		sub ri_v1, 16 * 8
		sub ri_v2, 16 * 8
		movdqu xmm7, [ri_v2 + 16 * 7]
		movdqu xmm6, [ri_v2 + 16 * 6]
		movdqu xmm5, [ri_v2 + 16 * 5]
		movdqu xmm4, [ri_v2 + 16 * 4]
		movdqu [ri_v1 + 16 * 7], xmm7
		movdqu [ri_v1 + 16 * 6], xmm6
		movdqu [ri_v1 + 16 * 5], xmm5
		movdqu [ri_v1 + 16 * 4], xmm4
		movdqu xmm3, [ri_v2 + 16 * 3]
		movdqu xmm2, [ri_v2 + 16 * 2]
		movdqu xmm1, [ri_v2 + 16 * 1]
		movdqu xmm0, [ri_v2 + 16 * 0]
		movdqu [ri_v1 + 16 * 3], xmm3
		movdqu [ri_v1 + 16 * 2], xmm2
		movdqu [ri_v1 + 16 * 1], xmm1
		movdqu [ri_v1 + 16 * 0], xmm0
		dec ri_v4
		jnz .simd_loop
		and ri_v3, 16 * 8 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		shr ri_v4, 4
		lea rq_temp1, [.simd_tail_jump_table]
		mov rq_temp1, [rq_temp1 + ri_v4 * 8]
		shl ri_v4, 4
		and ri_v3, 16 - 1
		sub ri_v1, ri_v4
		sub ri_v2, ri_v4
		jmp rq_temp1
	.simd_tail_jump_table:
		dq .simd_ldone, .simd_tail_b1, .simd_tail_b2, .simd_tail_b3, .simd_tail_b4, .simd_tail_b5, .simd_tail_b6, .simd_tail_b7
		.simd_tail_b7:
			movdqu xmm6, [ri_v2 + 16 * 6]
			movdqu [ri_v1 + 16 * 6], xmm6
		.simd_tail_b6:
			movdqu xmm5, [ri_v2 + 16 * 5]
			movdqu [ri_v1 + 16 * 5], xmm5
		.simd_tail_b5:
			movdqu xmm4, [ri_v2 + 16 * 4]
			movdqu [ri_v1 + 16 * 4], xmm4
		.simd_tail_b4:
			movdqu xmm3, [ri_v2 + 16 * 3]
			movdqu [ri_v1 + 16 * 3], xmm3
		.simd_tail_b3:
			movdqu xmm2, [ri_v2 + 16 * 2]
			movdqu [ri_v1 + 16 * 2], xmm2
		.simd_tail_b2:
			movdqu xmm1, [ri_v2 + 16 * 1]
			movdqu [ri_v1 + 16 * 1], xmm1
		.simd_tail_b1:
			movdqu xmm0, [ri_v2 + 16 * 0]
			movdqu [ri_v1 + 16 * 0], xmm0
	.simd_ldone:
		%if SUS_ARCH_SYS_WINDOWS
			movdqu xmm7, [rsp+16]
			movdqu xmm6, [rsp+0]
			add rsp, 32
		%endif
		jmp memcpybkw_fallback
	.end:

; ri_v1 = dest (end)
; ri_v2 = src (end)
; ri_v3 = count
; rq_uni = return dest
SUS_PUBLIC_FUNC memcpybkw_avx2
	.begin:
		cmp ri_v3, 128
		jb memcpybkw_sse2
	.align:
		mov ri_v4, ri_v1
		and ri_v4, 32 - 1
		jz .align_done
		vmovdqu ymm0, [ri_v2 - 32]
		vmovdqu [ri_v1 - 32], ymm0
		sub ri_v1, ri_v4
		sub ri_v2, ri_v4
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		%if SUS_ARCH_SYS_WINDOWS
			sub rsp, 32
			movdqu [rsp + 0], xmm6
			movdqu [rsp + 16], xmm7
		%endif
		shr ri_v4, 5 + 3
		jz .simd_ltail
	.simd_loop:
		sub ri_v1, 32 * 8
		sub ri_v2, 32 * 8
		vmovdqu ymm7, [ri_v2 + 32 * 7]
		vmovdqu ymm6, [ri_v2 + 32 * 6]
		vmovdqu ymm5, [ri_v2 + 32 * 5]
		vmovdqu ymm4, [ri_v2 + 32 * 4]
		vmovdqu [ri_v1 + 32 * 7], ymm7
		vmovdqu [ri_v1 + 32 * 6], ymm6
		vmovdqu [ri_v1 + 32 * 5], ymm5
		vmovdqu [ri_v1 + 32 * 4], ymm4
		vmovdqu ymm3, [ri_v2 + 32 * 3]
		vmovdqu ymm2, [ri_v2 + 32 * 2]
		vmovdqu ymm1, [ri_v2 + 32 * 1]
		vmovdqu ymm0, [ri_v2 + 32 * 0]
		vmovdqu [ri_v1 + 32 * 3], ymm3
		vmovdqu [ri_v1 + 32 * 2], ymm2
		vmovdqu [ri_v1 + 32 * 1], ymm1
		vmovdqu [ri_v1 + 32 * 0], ymm0
		dec ri_v4
		jnz .simd_loop
		and ri_v3, 32 * 8 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		shr ri_v4, 5
		lea rq_temp1, [.simd_tail_jump_table]
		mov rq_temp1, [rq_temp1 + ri_v4 * 8]
		shl ri_v4, 5
		sub ri_v1, ri_v4
		sub ri_v2, ri_v4
		jmp rq_temp1
	.simd_tail_jump_table:
		dq .simd_ldone, .simd_tail_b1, .simd_tail_b2, .simd_tail_b3, .simd_tail_b4, .simd_tail_b5, .simd_tail_b6, .simd_tail_b7
		.simd_tail_b7:
			vmovdqu ymm6, [ri_v2 + 32 * 6]
			vmovdqu [ri_v1 + 32 * 6], ymm6
		.simd_tail_b6:
			vmovdqu ymm5, [ri_v2 + 32 * 5]
			vmovdqu [ri_v1 + 32 * 5], ymm5
		.simd_tail_b5:
			vmovdqu ymm4, [ri_v2 + 32 * 4]
			vmovdqu [ri_v1 + 32 * 4], ymm4
		.simd_tail_b4:
			vmovdqu ymm3, [ri_v2 + 32 * 3]
			vmovdqu [ri_v1 + 32 * 3], ymm3
		.simd_tail_b3:
			vmovdqu ymm2, [ri_v2 + 32 * 2]
			vmovdqu [ri_v1 + 32 * 2], ymm2
		.simd_tail_b2:
			vmovdqu ymm1, [ri_v2 + 32 * 1]
			vmovdqu [ri_v1 + 32 * 1], ymm1
		.simd_tail_b1:
			vmovdqu ymm0, [ri_v2 + 32 * 0]
			vmovdqu [ri_v1 + 32 * 0], ymm0
		and ri_v3, 32 - 1
	.simd_ldone:
		%if SUS_ARCH_SYS_WINDOWS
			movdqu xmm7, [rsp+16]
			movdqu xmm6, [rsp+0]
			add rsp, 32
		%endif
		vzeroupper
		jmp memcpybkw_fallback
	.end:

; ---------------------------------------------------------------------

; ri_v1 = dest
; ri_v2 = src
; ri_v3 = count
; rq_uni = return dest
SUS_PUBLIC_FUNC memmove_sse2
	cmp ri_v1, ri_v2
	jb memcpy_sse2
	lea ri_v4, [ri_v2 + ri_v3]
	cmp ri_v1, ri_v4
	ja memcpy_sse2
	add ri_v1, ri_v3
	add ri_v2, ri_v3
	jmp memcpybkw_sse2
	
; ri_v1 = dest
; ri_v2 = src
; ri_v3 = count
; rq_uni = return dest
SUS_PUBLIC_FUNC memmove_avx2
	cmp ri_v1, ri_v2
	jb memcpy_avx2
	lea ri_v4, [ri_v2 + ri_v3]
	cmp ri_v1, ri_v4
	ja memcpy_avx2
	add ri_v1, ri_v3
	add ri_v2, ri_v3
	jmp memcpybkw_avx2
	
; ---------------------------------------------------------------------

SUS_PRIVATE_FUNC memset_fallback
	.begin:
	.qword_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 3
		jz .qword_ldone
	.qword_loop:
		mov [ri_v1], rq_temp2
		add ri_v1, 8
		dec ri_v4
		jnz .qword_loop
		and ri_v3, 7
	.qword_ldone:
	.byte_lbegin:
		test ri_v3, ri_v3
		jz .byte_ldone
	.byte_loop:
		mov [ri_v1], ri_v2b
		inc ri_v1
		dec ri_v3
		jnz .byte_loop
	.byte_ldone:
		ret
	.end:

; ri_v1 = dest
; ri_v2d = value
; ri_v3 = count
; rq_uni = return dest
SUS_PUBLIC_FUNC memset_sse2
	.begin:
		mov rq_uni, ri_v1
		movzx ri_v2, ri_v2d
		mov rq_temp2, 0x0101010101010101
		imul rq_temp2, ri_v2
		cmp ri_v3, 32
		jb memset_fallback
		movq xmm0, rq_temp2
		punpcklqdq xmm0, xmm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 16 - 1
		jz .align_done
		movdqu [ri_v1], xmm0
		add ri_v1, ri_v4
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 4 + 3
		jz .simd_ltail
	.simd_loop:
		movdqu [ri_v1 + 16 * 0], xmm0
		movdqu [ri_v1 + 16 * 1], xmm0
		movdqu [ri_v1 + 16 * 2], xmm0
		movdqu [ri_v1 + 16 * 3], xmm0
		movdqu [ri_v1 + 16 * 4], xmm0
		movdqu [ri_v1 + 16 * 5], xmm0
		movdqu [ri_v1 + 16 * 6], xmm0
		movdqu [ri_v1 + 16 * 7], xmm0
		add ri_v1, 16 * 8
		dec ri_v4
		jnz .simd_loop
		and ri_v3, 16 * 8 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		shr ri_v4, 4
		lea rq_temp1, [.simd_tail_jump_table]
		jmp [rq_temp1 + ri_v4 * 8]
	.simd_tail_jump_table:
		dq .simd_ldone, .simd_tail_b1, .simd_tail_b2, .simd_tail_b3, .simd_tail_b4, .simd_tail_b5, .simd_tail_b6, .simd_tail_b7
		.simd_tail_b7:
			movdqu [ri_v1 + 16 * 6], xmm0
		.simd_tail_b6:
			movdqu [ri_v1 + 16 * 5], xmm0
		.simd_tail_b5:
			movdqu [ri_v1 + 16 * 4], xmm0
		.simd_tail_b4:
			movdqu [ri_v1 + 16 * 3], xmm0
		.simd_tail_b3:
			movdqu [ri_v1 + 16 * 2], xmm0
		.simd_tail_b2:
			movdqu [ri_v1 + 16 * 1], xmm0
		.simd_tail_b1:
			movdqu [ri_v1 + 16 * 0], xmm0
		shl ri_v4, 4
		add ri_v1, ri_v4
		and ri_v3, 16 - 1
	.simd_ldone:
		jmp memset_fallback
	.end:

; ri_v1 = dest
; ri_v2d = value
; ri_v3 = count
; rq_uni = return dest
SUS_PUBLIC_FUNC memset_avx2
	.begin:
		cmp ri_v3, 128
		jb memset_sse2
		mov rq_uni, ri_v1
		vmovd xmm0, ri_v2d
		vpbroadcastb ymm0, xmm0
		vmovq rq_temp2, xmm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 32 - 1
		jz .align_done
		vmovdqu [ri_v1], ymm0
		add ri_v1, ri_v4
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 5 + 3
		jz .simd_ltail
	.simd_loop:
		vmovdqu [ri_v1 + 32 * 0], ymm0
		vmovdqu [ri_v1 + 32 * 1], ymm0
		vmovdqu [ri_v1 + 32 * 2], ymm0
		vmovdqu [ri_v1 + 32 * 3], ymm0
		vmovdqu [ri_v1 + 32 * 4], ymm0
		vmovdqu [ri_v1 + 32 * 5], ymm0
		vmovdqu [ri_v1 + 32 * 6], ymm0
		vmovdqu [ri_v1 + 32 * 7], ymm0
		add ri_v1, 32 * 8
		dec ri_v4
		jnz .simd_loop
		and ri_v3, 32 * 8 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		shr ri_v4, 5
		lea rq_temp1, [.simd_tail_jump_table]
		jmp [rq_temp1 + ri_v4 * 8]
	.simd_tail_jump_table:
		dq .simd_ldone, .simd_tail_b1, .simd_tail_b2, .simd_tail_b3, .simd_tail_b4, .simd_tail_b5, .simd_tail_b6, .simd_tail_b7
		.simd_tail_b7:
			vmovdqu [ri_v1 + 32 * 6], ymm0
		.simd_tail_b6:
			vmovdqu [ri_v1 + 32 * 5], ymm0
		.simd_tail_b5:
			vmovdqu [ri_v1 + 32 * 4], ymm0
		.simd_tail_b4:
			vmovdqu [ri_v1 + 32 * 3], ymm0
		.simd_tail_b3:
			vmovdqu [ri_v1 + 32 * 2], ymm0
		.simd_tail_b2:
			vmovdqu [ri_v1 + 32 * 1], ymm0
		.simd_tail_b1:
			vmovdqu [ri_v1 + 32 * 0], ymm0
		shl ri_v4, 5
		add ri_v1, ri_v4
		and ri_v3, 32 - 1
	.simd_ldone:
		vzeroupper
		jmp memset_fallback
	.end:
	
; ---------------------------------------------------------------------

SUS_PRIVATE_FUNC memset16_fallback
	.begin:
	.qword_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 3 - 1
		jz .qword_ldone
	.qword_loop:
		mov [ri_v1], rq_temp2
		add ri_v1, 8
		dec ri_v4
		jnz .qword_loop
		and ri_v3, 4 - 1
	.qword_ldone:
	.word_lbegin:
		test ri_v3, ri_v3
		jz .word_ldone
	.word_loop:
		mov [ri_v1], ri_v2w
		add ri_v1, 2
		dec ri_v3
		jnz .word_loop
	.word_ldone:
		ret
	.end:

; ri_v1 = dest
; ri_v2w = value
; ri_v3 = count
; rq_uni = return dest
SUS_PUBLIC_FUNC memset16_sse2
	.begin:
		mov rq_uni, ri_v1
		movzx ri_v2, ri_v2w
		mov rq_temp2, 0x0001000100010001
		imul rq_temp2, ri_v2
		cmp ri_v3, 16
		jb memset16_fallback
		movq xmm0, rq_temp2
		punpcklqdq xmm0, xmm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 16 - 1
		jz .align_done
		movdqu [ri_v1], xmm0
		add ri_v1, ri_v4
		shr ri_v4, 1
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 4 + 3 - 1
		jz .simd_ltail
	.simd_loop:
		movdqu [ri_v1 + 16 * 0], xmm0
		movdqu [ri_v1 + 16 * 1], xmm0
		movdqu [ri_v1 + 16 * 2], xmm0
		movdqu [ri_v1 + 16 * 3], xmm0
		movdqu [ri_v1 + 16 * 4], xmm0
		movdqu [ri_v1 + 16 * 5], xmm0
		movdqu [ri_v1 + 16 * 6], xmm0
		movdqu [ri_v1 + 16 * 7], xmm0
		add ri_v1, 16 * 8
		dec ri_v4
		jnz .simd_loop
		and ri_v3, (16 * 8) / 2 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		shr ri_v4, 4 - 1
		lea rq_temp1, [.simd_tail_jump_table]
		jmp [rq_temp1 + ri_v4 * 8]
	.simd_tail_jump_table:
		dq .simd_ldone, .simd_tail_b1, .simd_tail_b2, .simd_tail_b3, .simd_tail_b4, .simd_tail_b5, .simd_tail_b6, .simd_tail_b7
		.simd_tail_b7:
			movdqu [ri_v1 + 16 * 6], xmm0
		.simd_tail_b6:
			movdqu [ri_v1 + 16 * 5], xmm0
		.simd_tail_b5:
			movdqu [ri_v1 + 16 * 4], xmm0
		.simd_tail_b4:
			movdqu [ri_v1 + 16 * 3], xmm0
		.simd_tail_b3:
			movdqu [ri_v1 + 16 * 2], xmm0
		.simd_tail_b2:
			movdqu [ri_v1 + 16 * 1], xmm0
		.simd_tail_b1:
			movdqu [ri_v1 + 16 * 0], xmm0
		shl ri_v4, 4
		add ri_v1, ri_v4
		and ri_v3, 16 / 2 - 1
	.simd_ldone:
		jmp memset16_fallback
	.end:

; ri_v1 = dest
; ri_v2w = value
; ri_v3 = count
; rq_uni = return dest
SUS_PUBLIC_FUNC memset16_avx2
	.begin:
		cmp ri_v3, 64
		jb memset16_sse2
		mov rq_uni, ri_v1
		movzx ri_v2, ri_v2w
		vmovd xmm0, ri_v2d
		vpbroadcastw ymm0, xmm0
		vmovq rq_temp2, xmm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 32 - 1
		jz .align_done
		vmovdqu [ri_v1], ymm0
		add ri_v1, ri_v4
		shr ri_v4, 1
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 5 + 3 - 1
		jz .simd_ltail
	.simd_loop:
		vmovdqu [ri_v1 + 32 * 0], ymm0
		vmovdqu [ri_v1 + 32 * 1], ymm0
		vmovdqu [ri_v1 + 32 * 2], ymm0
		vmovdqu [ri_v1 + 32 * 3], ymm0
		vmovdqu [ri_v1 + 32 * 4], ymm0
		vmovdqu [ri_v1 + 32 * 5], ymm0
		vmovdqu [ri_v1 + 32 * 6], ymm0
		vmovdqu [ri_v1 + 32 * 7], ymm0
		add ri_v1, 32 * 8
		dec ri_v4
		jnz .simd_loop
		and ri_v3, (32 * 8) / 2 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		shr ri_v4, 5 - 1
		lea rq_temp1, [.simd_tail_jump_table]
		jmp [rq_temp1 + ri_v4 * 8]
	.simd_tail_jump_table:
		dq .simd_ldone, .simd_tail_b1, .simd_tail_b2, .simd_tail_b3, .simd_tail_b4, .simd_tail_b5, .simd_tail_b6, .simd_tail_b7
		.simd_tail_b7:
			vmovdqu [ri_v1 + 32 * 6], ymm0
		.simd_tail_b6:
			vmovdqu [ri_v1 + 32 * 5], ymm0
		.simd_tail_b5:
			vmovdqu [ri_v1 + 32 * 4], ymm0
		.simd_tail_b4:
			vmovdqu [ri_v1 + 32 * 3], ymm0
		.simd_tail_b3:
			vmovdqu [ri_v1 + 32 * 2], ymm0
		.simd_tail_b2:
			vmovdqu [ri_v1 + 32 * 1], ymm0
		.simd_tail_b1:
			vmovdqu [ri_v1 + 32 * 0], ymm0
		shl ri_v4, 5
		add ri_v1, ri_v4
		and ri_v3, 32 / 2 - 1
	.simd_ldone:
		vzeroupper
		jmp memset16_fallback
	.end:

; ---------------------------------------------------------------------

SUS_PRIVATE_FUNC memset32_fallback
	.begin:
	.qword_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 2 - 1
		jz .qword_ldone
	.qword_loop:
		mov [ri_v1], rq_temp2
		add ri_v1, 8
		dec ri_v4
		jnz .qword_loop
		and ri_v3, 2 - 1
	.qword_ldone:
	.word_lbegin:
		test ri_v3, ri_v3
		jz .word_ldone
	.word_loop:
		mov [ri_v1], ri_v2d
		add ri_v1, 4
		dec ri_v3
		jnz .word_loop
	.word_ldone:
		ret
	.end:

; ri_v1 = dest
; ri_v2d = value
; ri_v3 = count
; rq_uni = return dest
SUS_PUBLIC_FUNC memset32_sse2
	.begin:
		mov rq_uni, ri_v1
		movzx ri_v2, ri_v2d
		mov rq_temp2, 0x0000000100000001
		imul rq_temp2, ri_v2
		movq xmm0, rq_temp2
		punpcklqdq xmm0, xmm0
		cmp ri_v3, 8
		jb memset32_fallback
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 16 - 1
		jz .align_done
		movdqu [ri_v1], xmm0
		add ri_v1, ri_v4
		shr ri_v4, 2
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 4 + 3 - 2
		jz .simd_ltail
	.simd_loop:
		movdqu [ri_v1 + 16 * 0], xmm0
		movdqu [ri_v1 + 16 * 1], xmm0
		movdqu [ri_v1 + 16 * 2], xmm0
		movdqu [ri_v1 + 16 * 3], xmm0
		movdqu [ri_v1 + 16 * 4], xmm0
		movdqu [ri_v1 + 16 * 5], xmm0
		movdqu [ri_v1 + 16 * 6], xmm0
		movdqu [ri_v1 + 16 * 7], xmm0
		add ri_v1, 16 * 8
		dec ri_v4
		jnz .simd_loop
		and ri_v3, (16 * 8) / 4 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		shr ri_v4, 4 - 2
		lea rq_temp1, [.simd_tail_jump_table]
		jmp [rq_temp1 + ri_v4 * 8]
	.simd_tail_jump_table:
		dq .simd_ldone, .simd_tail_b1, .simd_tail_b2, .simd_tail_b3, .simd_tail_b4, .simd_tail_b5, .simd_tail_b6, .simd_tail_b7
		.simd_tail_b7:
			movdqu [ri_v1 + 16 * 6], xmm0
		.simd_tail_b6:
			movdqu [ri_v1 + 16 * 5], xmm0
		.simd_tail_b5:
			movdqu [ri_v1 + 16 * 4], xmm0
		.simd_tail_b4:
			movdqu [ri_v1 + 16 * 3], xmm0
		.simd_tail_b3:
			movdqu [ri_v1 + 16 * 2], xmm0
		.simd_tail_b2:
			movdqu [ri_v1 + 16 * 1], xmm0
		.simd_tail_b1:
			movdqu [ri_v1 + 16 * 0], xmm0
		shl ri_v4, 4
		add ri_v1, ri_v4
		and ri_v3, 16 / 4 - 1
	.simd_ldone:
		jmp memset32_fallback
	.end:

; ri_v1 = dest
; ri_v2d = value
; ri_v3 = count
; rq_uni = return dest
SUS_PUBLIC_FUNC memset32_avx2
	.begin:
		cmp ri_v3, 32
		jb memset32_sse2
		mov rq_uni, ri_v1
		vmovd xmm0, ri_v2d
		vpbroadcastd ymm0, xmm0
		vmovq rq_temp2, xmm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 32 - 1
		jz .align_done
		vmovdqu [ri_v1], ymm0
		add ri_v1, ri_v4
		shr ri_v4, 2
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 5 + 3 - 2
		jz .simd_ltail
	.simd_loop:
		vmovdqu [ri_v1 + 32 * 0], ymm0
		vmovdqu [ri_v1 + 32 * 1], ymm0
		vmovdqu [ri_v1 + 32 * 2], ymm0
		vmovdqu [ri_v1 + 32 * 3], ymm0
		vmovdqu [ri_v1 + 32 * 4], ymm0
		vmovdqu [ri_v1 + 32 * 5], ymm0
		vmovdqu [ri_v1 + 32 * 6], ymm0
		vmovdqu [ri_v1 + 32 * 7], ymm0
		add ri_v1, 32 * 8
		dec ri_v4
		jnz .simd_loop
		and ri_v3, (32 * 8) / 4 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		shr ri_v4, 5 - 2
		lea rq_temp1, [.simd_tail_jump_table]
		jmp [rq_temp1 + ri_v4 * 8]
	.simd_tail_jump_table:
		dq .simd_ldone, .simd_tail_b1, .simd_tail_b2, .simd_tail_b3, .simd_tail_b4, .simd_tail_b5, .simd_tail_b6, .simd_tail_b7
		.simd_tail_b7:
			vmovdqu [ri_v1 + 32 * 6], ymm0
		.simd_tail_b6:
			vmovdqu [ri_v1 + 32 * 5], ymm0
		.simd_tail_b5:
			vmovdqu [ri_v1 + 32 * 4], ymm0
		.simd_tail_b4:
			vmovdqu [ri_v1 + 32 * 3], ymm0
		.simd_tail_b3:
			vmovdqu [ri_v1 + 32 * 2], ymm0
		.simd_tail_b2:
			vmovdqu [ri_v1 + 32 * 1], ymm0
		.simd_tail_b1:
			vmovdqu [ri_v1 + 32 * 0], ymm0
		shl ri_v4, 5
		add ri_v1, ri_v4
		and ri_v3, 32 / 4 - 1
	.simd_ldone:
		vzeroupper
		jmp memset32_fallback
	.end:
	
; ---------------------------------------------------------------------

SUS_PRIVATE_FUNC memset64_fallback
	.begin:
	.qword_lbegin:
		test ri_v3, ri_v3
		jz .qword_ldone
	.qword_loop:
		mov [ri_v1], rq_temp2
		add ri_v1, 8
		dec ri_v3
		jnz .qword_loop
	.qword_ldone:
		ret
	.end:

; ri_v1 = dest
; ri_v2 = value
; ri_v3 = count
; rq_uni = return dest
SUS_PUBLIC_FUNC memset64_sse2
	.begin:
		cmp ri_v3, 4
		jb memset64_fallback
		mov rq_uni, ri_v1
		movq xmm0, rq_temp2
		punpcklqdq xmm0, xmm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 16 - 1
		jz .align_done
		movdqu [ri_v1], xmm0
		add ri_v1, ri_v4
		sub ri_v3, 8
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 4 + 3 - 3
		jz .simd_ltail
	.simd_loop:
		movdqu [ri_v1 + 16 * 0], xmm0
		movdqu [ri_v1 + 16 * 1], xmm0
		movdqu [ri_v1 + 16 * 2], xmm0
		movdqu [ri_v1 + 16 * 3], xmm0
		movdqu [ri_v1 + 16 * 4], xmm0
		movdqu [ri_v1 + 16 * 5], xmm0
		movdqu [ri_v1 + 16 * 6], xmm0
		movdqu [ri_v1 + 16 * 7], xmm0
		add ri_v1, 16 * 8
		dec ri_v4
		jnz .simd_loop
		and ri_v3, (16 * 8) / 8 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		shr ri_v4, 4 - 3
		lea rq_temp1, [.simd_tail_jump_table]
		jmp [rq_temp1 + ri_v4 * 8]
	.simd_tail_jump_table:
		dq .simd_ldone, .simd_tail_b1, .simd_tail_b2, .simd_tail_b3, .simd_tail_b4, .simd_tail_b5, .simd_tail_b6, .simd_tail_b7
		.simd_tail_b7:
			movdqu [ri_v1 + 16 * 6], xmm0
		.simd_tail_b6:
			movdqu [ri_v1 + 16 * 5], xmm0
		.simd_tail_b5:
			movdqu [ri_v1 + 16 * 4], xmm0
		.simd_tail_b4:
			movdqu [ri_v1 + 16 * 3], xmm0
		.simd_tail_b3:
			movdqu [ri_v1 + 16 * 2], xmm0
		.simd_tail_b2:
			movdqu [ri_v1 + 16 * 1], xmm0
		.simd_tail_b1:
			movdqu [ri_v1 + 16 * 0], xmm0
		shl ri_v4, 4
		add ri_v1, ri_v4
		and ri_v3, 16 / 8 - 1
	.simd_ldone:
		jmp memset64_fallback
	.end:

; ri_v1 = dest
; ri_v2 = value
; ri_v3 = count
; rq_uni = return dest
SUS_PUBLIC_FUNC memset64_avx2
	.begin:
		cmp ri_v3, 16
		jb memset64_fallback
		mov rq_uni, ri_v1
		vmovq xmm0, ri_v2
		vpbroadcastd ymm0, xmm0
		vmovq rq_temp2, xmm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 32 - 1
		jz .align_done
		vmovdqu [ri_v1], ymm0
		add ri_v1, ri_v4
		shr ri_v4, 3
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 5 + 3 - 3
		jz .simd_ltail
	.simd_loop:
		vmovdqu [ri_v1 + 32 * 0], ymm0
		vmovdqu [ri_v1 + 32 * 1], ymm0
		vmovdqu [ri_v1 + 32 * 2], ymm0
		vmovdqu [ri_v1 + 32 * 3], ymm0
		vmovdqu [ri_v1 + 32 * 4], ymm0
		vmovdqu [ri_v1 + 32 * 5], ymm0
		vmovdqu [ri_v1 + 32 * 6], ymm0
		vmovdqu [ri_v1 + 32 * 7], ymm0
		add ri_v1, 32 * 8
		dec ri_v4
		jnz .simd_loop
		and ri_v3, (32 * 8) / 8 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		shr ri_v4, 5 - 3
		lea rq_temp1, [.simd_tail_jump_table]
		jmp [rq_temp1 + ri_v4 * 8]
	.simd_tail_jump_table:
		dq .simd_ldone, .simd_tail_b1, .simd_tail_b2, .simd_tail_b3, .simd_tail_b4, .simd_tail_b5, .simd_tail_b6, .simd_tail_b7
		.simd_tail_b7:
			vmovdqu [ri_v1 + 32 * 6], ymm0
		.simd_tail_b6:
			vmovdqu [ri_v1 + 32 * 5], ymm0
		.simd_tail_b5:
			vmovdqu [ri_v1 + 32 * 4], ymm0
		.simd_tail_b4:
			vmovdqu [ri_v1 + 32 * 3], ymm0
		.simd_tail_b3:
			vmovdqu [ri_v1 + 32 * 2], ymm0
		.simd_tail_b2:
			vmovdqu [ri_v1 + 32 * 1], ymm0
		.simd_tail_b1:
			vmovdqu [ri_v1 + 32 * 0], ymm0
		shl ri_v4, 5
		add ri_v1, ri_v4
		and ri_v3, 32 / 8 - 1
	.simd_ldone:
		vzeroupper
		jmp memset64_fallback
	.end:

; ---------------------------------------------------------------------

SUS_PRIVATE_FUNC memcmp_fallback
	.begin:
	.qword_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 3
		jz .qword_ldone
	.qword_loop:
		mov rq_uni, [ri_v1]
		mov rq_temp1, [ri_v2]
		cmp rq_uni, rq_temp1
		jne .qword_non_equal
		add ri_v1, 8
		add ri_v2, 8
		dec ri_v4
		jnz .qword_loop
		and ri_v3, 7
	.qword_ldone:
	.byte_lbegin:
		test ri_v3, ri_v3
		jz .byte_ldone
	.byte_loop:
		mov rb_uni, [ri_v1]
		mov rh_uni, [ri_v2]
		sub rb_uni, rh_uni
		jnz .byte_non_equal
		inc ri_v1
		inc ri_v2
		dec ri_v3
		jnz .byte_loop
	.byte_ldone:
		xor rd_uni, rd_uni
		ret
	.byte_non_equal:
		movsx rd_uni, rb_uni
		ret
	.qword_non_equal:
		xor rq_uni, rq_temp1
		rep bsf rq_temp1, rq_uni
		shr rq_temp1, 3
		movzx rd_uni, byte [ri_v1 + rq_temp1]
		movzx rd_temp1, byte [ri_v2 + rq_temp1]
		sub rd_uni, rd_temp1
		ret
	.end:

; ri_v1 = ptr1
; ri_v2 = ptr2
; ri_v3 = count
; rd_uni = return cmp
SUS_PUBLIC_FUNC memcmp_sse2
	.begin:
		cmp ri_v3, 32
		jb memcmp_fallback
		mov rw_temp2, 0xffff
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 16 - 1
		jz .align_done
		movdqu xmm0, [ri_v1]
		movdqu xmm1, [ri_v2]
		pcmpeqb xmm0, xmm1
		pmovmskb rd_uni, xmm0
		cmp rw_uni, rw_temp2
		jne .simd_non_equal_b1
		add ri_v1, ri_v4
		add ri_v2, ri_v4
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		%if SUS_ARCH_SYS_WINDOWS
			sub rsp, 32
			movdqu [rsp + 0], xmm6
			movdqu [rsp + 16], xmm7
		%endif
		shr ri_v4, 4 + 2
		jz .simd_ltail
	.simd_loop:
		movdqu xmm0, [ri_v1 + 16 * 0]
		movdqu xmm1, [ri_v1 + 16 * 1]
		movdqu xmm2, [ri_v1 + 16 * 2]
		movdqu xmm3, [ri_v1 + 16 * 3]
		movdqu xmm4, [ri_v2 + 16 * 0]
		movdqu xmm5, [ri_v2 + 16 * 1]
		movdqu xmm6, [ri_v2 + 16 * 2]
		movdqu xmm7, [ri_v2 + 16 * 3]
		pcmpeqb xmm0, xmm4
		pcmpeqb xmm1, xmm5
		pcmpeqb xmm2, xmm6
		pcmpeqb xmm3, xmm7
		pmovmskb rd_uni, xmm0
		pmovmskb rd_temp1, xmm1
		and rd_temp1, rd_uni
		cmp rw_temp1, rw_temp2
		jne .simd_non_equal_1
		pmovmskb rd_uni, xmm2
		pmovmskb rd_temp1, xmm3
		and rd_temp1, rd_uni
		cmp rw_temp1, rw_temp2
		jne .simd_non_equal_2
		add ri_v1, 16 * 4
		add ri_v2, 16 * 4
		dec ri_v4
		jnz .simd_loop
		and ri_v3, 16 * 4 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		and ri_v4, ~(16 - 1)
		jz .simd_ldone
		cmp ri_v4, 1 * 16
		je .simd_tail_b1
		cmp ri_v4, 2 * 16
		je .simd_tail_b2
		.simd_tail_b3:
			movdqu xmm2, [ri_v1 + 16 * 2]
			movdqu xmm6, [ri_v2 + 16 * 2]
			pcmpeqb xmm2, xmm6
			pmovmskb rd_uni, xmm2
			cmp rw_uni, rw_temp2
			jne .simd_non_equal_b3
		.simd_tail_b2:
			movdqu xmm1, [ri_v1 + 16 * 1]
			movdqu xmm5, [ri_v2 + 16 * 1]
			pcmpeqb xmm1, xmm5
			pmovmskb rd_temp1, xmm1
			cmp rw_temp1, rw_temp2
			jne .simd_non_equal_b2
		.simd_tail_b1:
			movdqu xmm0, [ri_v1 + 16 * 0]
			movdqu xmm4, [ri_v2 + 16 * 0]
			pcmpeqb xmm0, xmm4
			pmovmskb rd_uni, xmm0
			cmp rw_uni, rw_temp2
			jne .simd_non_equal_b1
		add ri_v1, ri_v4
		add ri_v2, ri_v4
		and ri_v3, 16 - 1
	.simd_ldone:
		%if SUS_ARCH_SYS_WINDOWS
			movdqu xmm7, [rsp+16]
			movdqu xmm6, [rsp+0]
			add rsp, 32
		%endif
		jmp memcmp_fallback
	.simd_non_equal_1:
		cmp rw_uni, rw_temp2
		jne .simd_non_equal_b1
		.simd_non_equal_b2:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp+16]
				movdqu xmm6, [rsp+0]
				add rsp, 32
			%endif
			not rd_temp1
			rep bsf rd_temp1, rd_temp1
			movzx rq_temp1, rd_temp1
			movzx rd_uni, byte [ri_v1 + rq_temp1 + 1 * 16]
			movzx rd_temp1, byte [ri_v2 + rq_temp1 + 1 * 16]
			sub rd_uni, rd_temp1
			ret
		.simd_non_equal_b1:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp+16]
				movdqu xmm6, [rsp+0]
				add rsp, 32
			%endif
			not rd_uni
			rep bsf rd_uni, rd_uni
			movzx rq_uni, rd_uni
			movzx rd_temp1, byte [ri_v2 + rq_uni + 0 * 16]
			movzx rd_uni, byte [ri_v1 + rq_uni + 0 * 16]
			sub rd_uni, rd_temp1
			ret
	.simd_non_equal_2:
		cmp rw_uni, rw_temp2
		jne .simd_non_equal_b3
		.simd_non_equal_b4:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp+16]
				movdqu xmm6, [rsp+0]
				add rsp, 32
			%endif
			not rd_temp1
			rep bsf rd_temp1, rd_temp1
			movzx rq_temp1, rd_temp1
			movzx rd_uni, byte [ri_v1 + rq_temp1 + 3 * 16]
			movzx rd_temp1, byte [ri_v2 + rq_temp1 + 3 * 16]
			sub rd_uni, rd_temp1
			ret
		.simd_non_equal_b3:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp+16]
				movdqu xmm6, [rsp+0]
				add rsp, 32
			%endif
			not rd_uni
			rep bsf rd_uni, rd_uni
			movzx rq_uni, rd_uni
			movzx rd_temp1, byte [ri_v2 + rq_uni + 2 * 16]
			movzx rd_uni, byte [ri_v1 + rq_uni + 2 * 16]
			sub rd_uni, rd_temp1
			ret
	.end:

; ri_v1 = ptr1
; ri_v2 = ptr2
; ri_v3 = count
; rd_uni = return cmp
SUS_PUBLIC_FUNC memcmp_avx2
	.begin:
		cmp ri_v3, 128
		jb memcmp_sse2
		mov rd_temp2, 0xffffffff
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 32 - 1
		jz .align_done
		vmovdqu ymm0, [ri_v1]
		vmovdqu ymm1, [ri_v2]
		vpcmpeqb ymm0, ymm1
		vpmovmskb rd_uni, ymm0
		cmp rd_uni, rd_temp2
		jne .simd_non_equal_b1
		add ri_v1, ri_v4
		add ri_v2, ri_v4
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		%if SUS_ARCH_SYS_WINDOWS
			sub rsp, 32
			movdqu [rsp + 0], xmm6
			movdqu [rsp + 16], xmm7
		%endif
		shr ri_v4, 5 + 2
		jz .simd_ltail
	.simd_loop:
		vmovdqu ymm0, [ri_v1 + 32 * 0]
		vmovdqu ymm1, [ri_v1 + 32 * 1]
		vmovdqu ymm2, [ri_v1 + 32 * 2]
		vmovdqu ymm3, [ri_v1 + 32 * 3]
		vmovdqu ymm4, [ri_v2 + 32 * 0]
		vmovdqu ymm5, [ri_v2 + 32 * 1]
		vmovdqu ymm6, [ri_v2 + 32 * 2]
		vmovdqu ymm7, [ri_v2 + 32 * 3]
		vpcmpeqb ymm0, ymm4
		vpcmpeqb ymm1, ymm5
		vpcmpeqb ymm2, ymm6
		vpcmpeqb ymm3, ymm7
		vpmovmskb rd_uni, ymm0
		vpmovmskb rd_temp1, ymm1
		and rd_temp1, rd_uni
		cmp rd_temp1, rd_temp2
		jne .simd_non_equal_1
		vpmovmskb rd_uni, ymm2
		vpmovmskb rd_temp1, ymm3
		and rd_temp1, rd_uni
		cmp rd_temp1, rd_temp2
		jne .simd_non_equal_2
		add ri_v1, 32 * 4
		add ri_v2, 32 * 4
		dec ri_v4
		jnz .simd_loop
		and ri_v3, 32 * 4 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		and ri_v4, ~(32 - 1)
		jz .simd_ldone
		cmp ri_v4, 1 * 32
		je .simd_tail_b1
		cmp ri_v4, 2 * 32
		je .simd_tail_b2
		.simd_tail_b3:
			vmovdqu ymm2, [ri_v1 + 32 * 2]
			vmovdqu ymm6, [ri_v2 + 32 * 2]
			vpcmpeqb ymm2, ymm6
			vpmovmskb rd_uni, ymm2
			cmp rd_uni, rd_temp2
			jne .simd_non_equal_b3
		.simd_tail_b2:
			vmovdqu ymm1, [ri_v1 + 32 * 1]
			vmovdqu ymm5, [ri_v2 + 32 * 1]
			vpcmpeqb ymm1, ymm5
			vpmovmskb rd_temp1, ymm1
			cmp rd_temp1, rd_temp2
			jne .simd_non_equal_b2
		.simd_tail_b1:
			vmovdqu ymm0, [ri_v1 + 32 * 0]
			vmovdqu ymm4, [ri_v2 + 32 * 0]
			vpcmpeqb ymm0, ymm4
			vpmovmskb rd_uni, ymm0
			cmp rd_uni, rd_temp2
			jne .simd_non_equal_b1
		add ri_v1, ri_v4
		add ri_v2, ri_v4
		and ri_v3, 32 - 1
	.simd_ldone:
		%if SUS_ARCH_SYS_WINDOWS
			movdqu xmm7, [rsp+16]
			movdqu xmm6, [rsp+0]
			add rsp, 32
		%endif
		vzeroupper
		jmp memcmp_fallback
	.simd_non_equal_1:
		cmp rd_uni, rd_temp2
		jne .simd_non_equal_b1
		.simd_non_equal_b2:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp+16]
				movdqu xmm6, [rsp+0]
				add rsp, 32
			%endif
			vzeroupper
			not rd_temp1
			rep bsf rd_temp1, rd_temp1
			movzx rq_temp1, rd_temp1
			movzx rd_uni, byte [ri_v1 + rq_temp1 + 1 * 32]
			movzx rd_temp1, byte [ri_v2 + rq_temp1 + 1 * 32]
			sub rd_uni, rd_temp1
			ret
		.simd_non_equal_b1:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp+16]
				movdqu xmm6, [rsp+0]
				add rsp, 32
			%endif
			vzeroupper
			not rd_uni
			rep bsf rd_uni, rd_uni
			movzx rq_uni, rd_uni
			movzx rd_temp1, byte [ri_v2 + rq_uni + 0 * 32]
			movzx rd_uni, byte [ri_v1 + rq_uni + 0 * 32]
			sub rd_uni, rd_temp1
			ret
	.simd_non_equal_2:
		cmp rd_uni, rd_temp2
		jne .simd_non_equal_b3
		.simd_non_equal_b4:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp+16]
				movdqu xmm6, [rsp+0]
				add rsp, 32
			%endif
			vzeroupper
			not rd_temp1
			rep bsf rd_temp1, rd_temp1
			movzx rq_temp1, rd_temp1
			movzx rd_uni, byte [ri_v1 + rq_temp1 + 3 * 32]
			movzx rd_temp1, byte [ri_v2 + rq_temp1 + 3 * 32]
			sub rd_uni, rd_temp1
			ret
		.simd_non_equal_b3:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp+16]
				movdqu xmm6, [rsp+0]
				add rsp, 32
			%endif
			vzeroupper
			not rd_uni
			rep bsf rd_uni, rd_uni
			movzx rq_uni, rd_uni
			movzx rd_temp1, byte [ri_v2 + rq_uni + 2 * 32]
			movzx rd_uni, byte [ri_v1 + rq_uni + 2 * 32]
			sub rd_uni, rd_temp1
			ret
	.end:

; ---------------------------------------------------------------------

SUS_PRIVATE_FUNC memcmp16_fallback
	.begin:
	.qword_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 3 - 1
		jz .qword_ldone
	.qword_loop:
		mov rq_uni, [ri_v1]
		mov rq_temp1, [ri_v2]
		cmp rq_uni, rq_temp1
		jne .qword_non_equal
		add ri_v1, 8
		add ri_v2, 8
		dec ri_v4
		jnz .qword_loop
		and ri_v3, 3
	.qword_ldone:
	.word_lbegin:
		test ri_v3, ri_v3
		jz .word_ldone
	.word_loop:
		mov rw_uni, [ri_v1]
		mov rw_temp1, [ri_v2]
		sub rw_uni, rw_temp1
		jnz .word_non_equal
		add ri_v1, 2
		add ri_v2, 2
		dec ri_v3
		jnz .word_loop
	.word_ldone:
		xor rd_uni, rd_uni
		ret
	.word_non_equal:
		movsx rd_uni, rw_uni
		ret
	.qword_non_equal:
		xor rq_uni, rq_temp1
		rep bsf rq_temp1, rq_uni
		shr rq_temp1, 1
		movzx rd_uni, word [ri_v1 + rq_temp1 * 2]
		movzx rd_temp1, word [ri_v2 + rq_temp1 * 2]
		sub rd_uni, rd_temp1
		ret
	.end:

; ri_v1 = ptr1
; ri_v2 = ptr2
; ri_v3 = count
; rd_uni = return cmp
SUS_PUBLIC_FUNC memcmp16_sse2
	.begin:
		cmp ri_v3, 16
		jb memcmp16_fallback
		mov rw_temp2, 0xffff
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 16 - 1
		jz .align_done
		movdqu xmm0, [ri_v1]
		movdqu xmm1, [ri_v2]
		pcmpeqw xmm0, xmm1
		pmovmskb rd_uni, xmm0
		cmp rw_uni, rw_temp2
		jne .simd_non_equal_b1
		add ri_v1, ri_v4
		add ri_v2, ri_v4
		shr ri_v4, 1
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		%if SUS_ARCH_SYS_WINDOWS
			sub rsp, 32
			movdqu [rsp + 0], xmm6
			movdqu [rsp + 16], xmm7
		%endif
		shr ri_v4, 4 + 2 - 1
		jz .simd_ltail
	.simd_loop:
		movdqu xmm0, [ri_v1 + 16 * 0]
		movdqu xmm1, [ri_v1 + 16 * 1]
		movdqu xmm2, [ri_v1 + 16 * 2]
		movdqu xmm3, [ri_v1 + 16 * 3]
		movdqu xmm4, [ri_v2 + 16 * 0]
		movdqu xmm5, [ri_v2 + 16 * 1]
		movdqu xmm6, [ri_v2 + 16 * 2]
		movdqu xmm7, [ri_v2 + 16 * 3]
		pcmpeqw xmm0, xmm4
		pcmpeqw xmm1, xmm5
		pcmpeqw xmm2, xmm6
		pcmpeqw xmm3, xmm7
		pmovmskb rd_uni, xmm0
		pmovmskb rd_temp1, xmm1
		and rd_temp1, rd_uni
		cmp rw_temp1, rw_temp2
		jne .simd_non_equal_1
		pmovmskb rd_uni, xmm2
		pmovmskb rd_temp1, xmm3
		and rd_temp1, rd_uni
		cmp rw_temp1, rw_temp2
		jne .simd_non_equal_2
		add ri_v1, 16 * 4
		add ri_v2, 16 * 4
		dec ri_v4
		jnz .simd_loop
		and ri_v3, 16 * 4 / 2 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		and ri_v4, ~(16 / 2 - 1)
		jz .simd_ldone
		shl ri_v4, 1
		cmp ri_v4, 1 * 16
		je .simd_tail_b1
		cmp ri_v4, 2 * 16
		je .simd_tail_b2
		.simd_tail_b3:
			movdqu xmm2, [ri_v1 + 16 * 2]
			movdqu xmm6, [ri_v2 + 16 * 2]
			pcmpeqw xmm2, xmm6
			pmovmskb rd_uni, xmm2
			cmp rw_uni, rw_temp2
			jne .simd_non_equal_b3
		.simd_tail_b2:
			movdqu xmm1, [ri_v1 + 16 * 1]
			movdqu xmm5, [ri_v2 + 16 * 1]
			pcmpeqw xmm1, xmm5
			pmovmskb rd_temp1, xmm1
			cmp rw_temp1, rw_temp2
			jne .simd_non_equal_b2
		.simd_tail_b1:
			movdqu xmm0, [ri_v1 + 16 * 0]
			movdqu xmm4, [ri_v2 + 16 * 0]
			pcmpeqw xmm0, xmm4
			pmovmskb rd_uni, xmm0
			cmp rw_uni, rw_temp2
			jne .simd_non_equal_b1
		add ri_v1, ri_v4
		add ri_v2, ri_v4
		and ri_v3, 16 / 2 - 1
	.simd_ldone:
		%if SUS_ARCH_SYS_WINDOWS
			movdqu xmm7, [rsp+16]
			movdqu xmm6, [rsp+0]
			add rsp, 32
		%endif
		jmp memcmp_fallback
	.simd_non_equal_1:
		cmp rw_uni, rw_temp2
		jne .simd_non_equal_b1
		.simd_non_equal_b2:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp+16]
				movdqu xmm6, [rsp+0]
				add rsp, 32
			%endif
			not rd_temp1
			rep bsf rd_temp1, rd_temp1
			and rd_temp1, ~1
			movzx rq_temp1, rd_temp1
			movzx rd_uni, word [ri_v1 + rq_temp1 + 1 * 16]
			movzx rd_temp1, word [ri_v2 + rq_temp1 + 1 * 16]
			sub rd_uni, rd_temp1
			ret
		.simd_non_equal_b1:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp+16]
				movdqu xmm6, [rsp+0]
				add rsp, 32
			%endif
			not rd_uni
			rep bsf rd_uni, rd_uni
			and rd_uni, ~1
			movzx rq_uni, rd_uni
			movzx rd_temp1, word [ri_v2 + rq_uni + 0 * 16]
			movzx rd_uni, word [ri_v1 + rq_uni + 0 * 16]
			sub rd_uni, rd_temp1
			ret
	.simd_non_equal_2:
		cmp rw_uni, rw_temp2
		jne .simd_non_equal_b3
		.simd_non_equal_b4:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp+16]
				movdqu xmm6, [rsp+0]
				add rsp, 32
			%endif
			not rd_temp1
			rep bsf rd_temp1, rd_temp1
			and rd_temp1, ~1
			movzx rq_temp1, rd_temp1
			movzx rd_uni, word [ri_v1 + rq_temp1 + 3 * 16]
			movzx rd_temp1, word [ri_v2 + rq_temp1 + 3 * 16]
			sub rd_uni, rd_temp1
			ret
		.simd_non_equal_b3:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp+16]
				movdqu xmm6, [rsp+0]
				add rsp, 32
			%endif
			not rd_uni
			rep bsf rd_uni, rd_uni
			and rd_uni, ~1
			movzx rq_uni, rd_uni
			movzx rd_temp1, word [ri_v2 + rq_uni + 2 * 16]
			movzx rd_uni, word [ri_v1 + rq_uni + 2 * 16]
			sub rd_uni, rd_temp1
			ret
	.end:
		
; ri_v1 = ptr1
; ri_v2 = ptr2
; ri_v3 = count
; rd_uni = return cmp
SUS_PUBLIC_FUNC memcmp16_avx2
	.begin:
		cmp ri_v3, 64
		jb memcmp16_sse2
		mov rd_temp2, 0xffffffff
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 32 - 1
		jz .align_done
		vmovdqu ymm0, [ri_v1]
		vmovdqu ymm1, [ri_v2]
		vpcmpeqw ymm0, ymm1
		vpmovmskb rd_uni, ymm0
		cmp rd_uni, rd_temp2
		jne .simd_non_equal_b1
		add ri_v1, ri_v4
		add ri_v2, ri_v4
		shr ri_v4, 1
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		%if SUS_ARCH_SYS_WINDOWS
			sub rsp, 32
			movdqu [rsp + 0], xmm6
			movdqu [rsp + 16], xmm7
		%endif
		shr ri_v4, 5 + 2 - 1
		jz .simd_ltail
	.simd_loop:
		vmovdqu ymm0, [ri_v1 + 32 * 0]
		vmovdqu ymm1, [ri_v1 + 32 * 1]
		vmovdqu ymm2, [ri_v1 + 32 * 2]
		vmovdqu ymm3, [ri_v1 + 32 * 3]
		vmovdqu ymm4, [ri_v2 + 32 * 0]
		vmovdqu ymm5, [ri_v2 + 32 * 1]
		vmovdqu ymm6, [ri_v2 + 32 * 2]
		vmovdqu ymm7, [ri_v2 + 32 * 3]
		vpcmpeqw ymm0, ymm4
		vpcmpeqw ymm1, ymm5
		vpcmpeqw ymm2, ymm6
		vpcmpeqw ymm3, ymm7
		vpmovmskb rd_uni, ymm0
		vpmovmskb rd_temp1, ymm1
		and rd_temp1, rd_uni
		cmp rd_temp1, rd_temp2
		jne .simd_non_equal_1
		vpmovmskb rd_uni, ymm2
		vpmovmskb rd_temp1, ymm3
		and rd_temp1, rd_uni
		cmp rd_temp1, rd_temp2
		jne .simd_non_equal_2
		add ri_v1, 32 * 4
		add ri_v2, 32 * 4
		dec ri_v4
		jnz .simd_loop
		and ri_v3, 32 * 4 / 2 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		and ri_v4, ~(32 / 2 - 1)
		jz .simd_ldone
		shl ri_v4, 1
		cmp ri_v4, 1 * 32
		je .simd_tail_b1
		cmp ri_v4, 2 * 32
		je .simd_tail_b2
		.simd_tail_b3:
			vmovdqu ymm2, [ri_v1 + 32 * 2]
			vmovdqu ymm6, [ri_v2 + 32 * 2]
			vpcmpeqw ymm2, ymm6
			vpmovmskb rd_uni, ymm2
			cmp rd_uni, rd_temp2
			jne .simd_non_equal_b3
		.simd_tail_b2:
			vmovdqu ymm1, [ri_v1 + 32 * 1]
			vmovdqu ymm5, [ri_v2 + 32 * 1]
			vpcmpeqw ymm1, ymm5
			vpmovmskb rd_temp1, ymm1
			cmp rd_temp1, rd_temp2
			jne .simd_non_equal_b2
		.simd_tail_b1:
			vmovdqu ymm0, [ri_v1 + 32 * 0]
			vmovdqu ymm4, [ri_v2 + 32 * 0]
			vpcmpeqw ymm0, ymm4
			vpmovmskb rd_uni, ymm0
			cmp rd_uni, rd_temp2
			jne .simd_non_equal_b1
		add ri_v1, ri_v4
		add ri_v2, ri_v4
		and ri_v3, 32 / 2 - 1
	.simd_ldone:
		%if SUS_ARCH_SYS_WINDOWS
			movdqu xmm7, [rsp+16]
			movdqu xmm6, [rsp+0]
			add rsp, 32
		%endif
		vzeroupper
		jmp memcmp16_fallback
	.simd_non_equal_1:
		cmp rd_uni, rd_temp2
		jne .simd_non_equal_b1
		.simd_non_equal_b2:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp+16]
				movdqu xmm6, [rsp+0]
				add rsp, 32
			%endif
			vzeroupper
			not rd_temp1
			rep bsf rd_temp1, rd_temp1
			and rd_temp1, ~1
			movzx rq_temp1, rd_temp1
			movzx rd_uni, word [ri_v1 + rq_temp1 + 1 * 32]
			movzx rd_temp1, word [ri_v2 + rq_temp1 + 1 * 32]
			sub rd_uni, rd_temp1
			ret
		.simd_non_equal_b1:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp+16]
				movdqu xmm6, [rsp+0]
				add rsp, 32
			%endif
			vzeroupper
			not rd_uni
			rep bsf rd_uni, rd_uni
			and rd_uni, ~1
			movzx rq_uni, rd_uni
			movzx rd_temp1, word [ri_v2 + rq_uni + 0 * 32]
			movzx rd_uni, word [ri_v1 + rq_uni + 0 * 32]
			sub rd_uni, rd_temp1
			ret
	.simd_non_equal_2:
		cmp rd_uni, rd_temp2
		jne .simd_non_equal_b3
		.simd_non_equal_b4:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp+16]
				movdqu xmm6, [rsp+0]
				add rsp, 32
			%endif
			vzeroupper
			not rd_temp1
			rep bsf rd_temp1, rd_temp1
			and rd_temp1, ~1
			movzx rq_temp1, rd_temp1
			movzx rd_uni, word [ri_v1 + rq_temp1 + 3 * 32]
			movzx rd_temp1, word [ri_v2 + rq_temp1 + 3 * 32]
			sub rd_uni, rd_temp1
			ret
		.simd_non_equal_b3:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp+16]
				movdqu xmm6, [rsp+0]
				add rsp, 32
			%endif
			vzeroupper
			not rd_uni
			rep bsf rd_uni, rd_uni
			and rd_uni, ~1
			movzx rq_uni, rd_uni
			movzx rd_temp1, word [ri_v2 + rq_uni + 2 * 32]
			movzx rd_uni, word [ri_v1 + rq_uni + 2 * 32]
			sub rd_uni, rd_temp1
			ret
	.end:
	
; ---------------------------------------------------------------------

SUS_PRIVATE_FUNC memcmp32_fallback
	.begin: 
	.qword_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 3 - 2
		jz .qword_ldone
	.qword_loop:
		mov rq_uni, [ri_v1]
		mov rq_temp1, [ri_v2]
		cmp rq_uni, rq_temp1
		jne .qword_non_equal
		add ri_v1, 8
		add ri_v2, 8
		dec ri_v4
		jnz .qword_loop
		and ri_v3, 1
	.qword_ldone:
	.word_lbegin:
		test ri_v3, ri_v3
		jz .word_ldone
	.word_loop:
		mov rd_uni, [ri_v1]
		mov rd_temp1, [ri_v2]
		sub rd_uni, rd_temp1
		jnz .word_non_equal
		add ri_v1, 4
		add ri_v2, 4
		dec ri_v3
		jnz .word_loop
	.word_ldone:
		xor rd_uni, rd_uni
	.word_non_equal:
		ret
	.qword_non_equal:
		cmp rd_uni, rd_temp1
		jne .low_diff
		shr rq_uni, 32
		shr rq_temp1, 32
	.low_diff:
		sub rd_uni, rd_temp1
		ret
	.end:

; ri_v1 = ptr1
; ri_v2 = ptr2
; ri_v3 = count
; rd_uni = return cmp
SUS_PUBLIC_FUNC memcmp32_sse2
	.begin:
		cmp ri_v3, 8
		jb memcmp32_fallback
		mov rw_temp2, 0xffff
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 16 - 1
		jz .align_done
		movdqu xmm0, [ri_v1]
		movdqu xmm1, [ri_v2]
		pcmpeqd xmm0, xmm1
		pmovmskb rd_uni, xmm0
		cmp rw_uni, rw_temp2
		jne .simd_non_equal_b1
		add ri_v1, ri_v4
		add ri_v2, ri_v4
		shr ri_v4, 2
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		%if SUS_ARCH_SYS_WINDOWS
			sub rsp, 32
			movdqu [rsp + 0], xmm6
			movdqu [rsp + 16], xmm7
		%endif
		shr ri_v4, 4 + 2 - 2
		jz .simd_ltail
	.simd_loop:
		movdqu xmm0, [ri_v1 + 16 * 0]
		movdqu xmm1, [ri_v1 + 16 * 1]
		movdqu xmm2, [ri_v1 + 16 * 2]
		movdqu xmm3, [ri_v1 + 16 * 3]
		movdqu xmm4, [ri_v2 + 16 * 0]
		movdqu xmm5, [ri_v2 + 16 * 1]
		movdqu xmm6, [ri_v2 + 16 * 2]
		movdqu xmm7, [ri_v2 + 16 * 3]
		pcmpeqd xmm0, xmm4
		pcmpeqd xmm1, xmm5
		pcmpeqd xmm2, xmm6
		pcmpeqd xmm3, xmm7
		pmovmskb rd_uni, xmm0
		pmovmskb rd_temp1, xmm1
		and rd_temp1, rd_uni
		cmp rw_temp1, rw_temp2
		jne .simd_non_equal_1
		pmovmskb rd_uni, xmm2
		pmovmskb rd_temp1, xmm3
		and rd_temp1, rd_uni
		cmp rw_temp1, rw_temp2
		jne .simd_non_equal_2
		add ri_v1, 16 * 4
		add ri_v2, 16 * 4
		dec ri_v4
		jnz .simd_loop
		and ri_v3, 16 * 4 / 4 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		and ri_v4, ~(16 / 4 - 1)
		jz .simd_ldone
		shl ri_v4, 2
		cmp ri_v4, 1 * 16
		je .simd_tail_b1
		cmp ri_v4, 2 * 16
		je .simd_tail_b2
		.simd_tail_b3:
			movdqu xmm2, [ri_v1 + 16 * 2]
			movdqu xmm6, [ri_v2 + 16 * 2]
			pcmpeqd xmm2, xmm6
			pmovmskb rd_uni, xmm2
			cmp rw_uni, rw_temp2
			jne .simd_non_equal_b3
		.simd_tail_b2:
			movdqu xmm1, [ri_v1 + 16 * 1]
			movdqu xmm5, [ri_v2 + 16 * 1]
			pcmpeqd xmm1, xmm5
			pmovmskb rd_temp1, xmm1
			cmp rw_temp1, rw_temp2
			jne .simd_non_equal_b2
		.simd_tail_b1:
			movdqu xmm0, [ri_v1 + 16 * 0]
			movdqu xmm4, [ri_v2 + 16 * 0]
			pcmpeqd xmm0, xmm4
			pmovmskb rd_uni, xmm0
			cmp rw_uni, rw_temp2
			jne .simd_non_equal_b1
		add ri_v1, ri_v4
		add ri_v2, ri_v4
		and ri_v3, 16 / 4 - 1
	.simd_ldone:
		%if SUS_ARCH_SYS_WINDOWS
			movdqu xmm7, [rsp+16]
			movdqu xmm6, [rsp+0]
			add rsp, 32
		%endif
		jmp memcmp32_fallback
	.simd_non_equal_1:
		cmp rw_uni, rw_temp2
		jne .simd_non_equal_b1
		.simd_non_equal_b2:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp+16]
				movdqu xmm6, [rsp+0]
				add rsp, 32
			%endif
			not rd_temp1
			rep bsf rd_temp1, rd_temp1
			and rd_temp1, ~3
			movzx rq_temp1, rd_temp1
			mov rd_uni, dword [ri_v1 + rq_temp1 + 1 * 16]
			mov rd_temp1, dword [ri_v2 + rq_temp1 + 1 * 16]
			sub rd_uni, rd_temp1
			ret
		.simd_non_equal_b1:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp+16]
				movdqu xmm6, [rsp+0]
				add rsp, 32
			%endif
			not rd_uni
			rep bsf rd_uni, rd_uni
			and rd_uni, ~3
			movzx rq_uni, rd_uni
			mov rd_temp1, dword [ri_v2 + rq_uni + 0 * 16]
			mov rd_uni, dword [ri_v1 + rq_uni + 0 * 16]
			sub rd_uni, rd_temp1
			ret
	.simd_non_equal_2:
		cmp rw_uni, rw_temp2
		jne .simd_non_equal_b3
		.simd_non_equal_b4:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp+16]
				movdqu xmm6, [rsp+0]
				add rsp, 32
			%endif
			not rd_temp1
			rep bsf rd_temp1, rd_temp1
			and rd_temp1, ~3
			movzx rq_temp1, rd_temp1
			mov rd_uni, dword [ri_v1 + rq_temp1 + 3 * 16]
			mov rd_temp1, dword [ri_v2 + rq_temp1 + 3 * 16]
			sub rd_uni, rd_temp1
			ret
		.simd_non_equal_b3:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp+16]
				movdqu xmm6, [rsp+0]
				add rsp, 32
			%endif
			not rd_uni
			rep bsf rd_uni, rd_uni
			and rd_uni, ~3
			movzx rq_uni, rd_uni
			mov rd_temp1, dword [ri_v2 + rq_uni + 2 * 16]
			mov rd_uni, dword [ri_v1 + rq_uni + 2 * 16]
			sub rd_uni, rd_temp1
			ret
	.end:

; ri_v1 = ptr1
; ri_v2 = ptr2
; ri_v3 = count
; rd_uni = return cmp
SUS_PUBLIC_FUNC memcmp32_avx2
	.begin:
		cmp ri_v3, 32
		jb memcmp32_sse2
		mov rd_temp2, 0xffffffff
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 32 - 1
		jz .align_done
		vmovdqu ymm0, [ri_v1]
		vmovdqu ymm1, [ri_v2]
		vpcmpeqd ymm0, ymm1
		vpmovmskb rd_uni, ymm0
		cmp rd_uni, rd_temp2
		jne .simd_non_equal_b1
		add ri_v1, ri_v4
		add ri_v2, ri_v4
		shr ri_v4, 2
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		%if SUS_ARCH_SYS_WINDOWS
			sub rsp, 32
			movdqu [rsp + 0], xmm6
			movdqu [rsp + 16], xmm7
		%endif
		shr ri_v4, 5 + 2 - 2
		jz .simd_ltail
	.simd_loop:
		vmovdqu ymm0, [ri_v1 + 32 * 0]
		vmovdqu ymm1, [ri_v1 + 32 * 1]
		vmovdqu ymm2, [ri_v1 + 32 * 2]
		vmovdqu ymm3, [ri_v1 + 32 * 3]
		vmovdqu ymm4, [ri_v2 + 32 * 0]
		vmovdqu ymm5, [ri_v2 + 32 * 1]
		vmovdqu ymm6, [ri_v2 + 32 * 2]
		vmovdqu ymm7, [ri_v2 + 32 * 3]
		vpcmpeqd ymm0, ymm4
		vpcmpeqd ymm1, ymm5
		vpcmpeqd ymm2, ymm6
		vpcmpeqd ymm3, ymm7
		vpmovmskb rd_uni, ymm0
		vpmovmskb rd_temp1, ymm1
		and rd_temp1, rd_uni
		cmp rd_temp1, rd_temp2
		jne .simd_non_equal_1
		vpmovmskb rd_uni, ymm2
		vpmovmskb rd_temp1, ymm3
		and rd_temp1, rd_uni
		cmp rd_temp1, rd_temp2
		jne .simd_non_equal_2
		add ri_v1, 32 * 4
		add ri_v2, 32 * 4
		dec ri_v4
		jnz .simd_loop
		and ri_v3, 32 * 4 / 4 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		and ri_v4, ~(32 / 4 - 1)
		jz .simd_ldone
		shl ri_v4, 2
		cmp ri_v4, 1 * 32
		je .simd_tail_b1
		cmp ri_v4, 2 * 32
		je .simd_tail_b2
		.simd_tail_b3:
			vmovdqu ymm2, [ri_v1 + 32 * 2]
			vmovdqu ymm6, [ri_v2 + 32 * 2]
			vpcmpeqd ymm2, ymm6
			vpmovmskb rd_uni, ymm2
			cmp rd_uni, rd_temp2
			jne .simd_non_equal_b3
		.simd_tail_b2:
			vmovdqu ymm1, [ri_v1 + 32 * 1]
			vmovdqu ymm5, [ri_v2 + 32 * 1]
			vpcmpeqd ymm1, ymm5
			vpmovmskb rd_temp1, ymm1
			cmp rd_temp1, rd_temp2
			jne .simd_non_equal_b2
		.simd_tail_b1:
			vmovdqu ymm0, [ri_v1 + 32 * 0]
			vmovdqu ymm4, [ri_v2 + 32 * 0]
			vpcmpeqd ymm0, ymm4
			vpmovmskb rd_uni, ymm0
			cmp rd_uni, rd_temp2
			jne .simd_non_equal_b1
		add ri_v1, ri_v4
		add ri_v2, ri_v4
		and ri_v3, 32 / 4 - 1
	.simd_ldone:
		%if SUS_ARCH_SYS_WINDOWS
			movdqu xmm7, [rsp + 16]
			movdqu xmm6, [rsp + 0]
			add rsp, 32
		%endif
		vzeroupper
		jmp memcmp32_fallback
	.simd_non_equal_1:
		cmp rd_uni, rd_temp2
		jne .simd_non_equal_b1
		.simd_non_equal_b2:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp + 16]
				movdqu xmm6, [rsp + 0]
				add rsp, 32
			%endif
			vzeroupper
			not rd_temp1
			rep bsf rd_temp1, rd_temp1
			and rd_temp1, ~3
			movzx rq_temp1, rd_temp1
			mov rd_uni, dword [ri_v1 + rq_temp1 + 1 * 32]
			mov rd_temp1, dword [ri_v2 + rq_temp1 + 1 * 32]
			sub rd_uni, rd_temp1
			ret
		.simd_non_equal_b1:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp + 16]
				movdqu xmm6, [rsp + 0]
				add rsp, 32
			%endif
			vzeroupper
			not rd_uni
			rep bsf rd_uni, rd_uni
			and rd_uni, ~3
			movzx rq_uni, rd_uni
			mov rd_temp1, dword [ri_v2 + rq_uni + 0 * 32]
			mov rd_uni, dword [ri_v1 + rq_uni + 0 * 32]
			sub rd_uni, rd_temp1
			ret
	.simd_non_equal_2:
		cmp rd_uni, rd_temp2
		jne .simd_non_equal_b3
		.simd_non_equal_b4:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp + 16]
				movdqu xmm6, [rsp + 0]
				add rsp, 32
			%endif
			vzeroupper
			not rd_temp1
			rep bsf rd_temp1, rd_temp1
			and rd_temp1, ~3
			movzx rq_temp1, rd_temp1
			mov rd_uni, dword [ri_v1 + rq_temp1 + 3 * 32]
			mov rd_temp1, dword [ri_v2 + rq_temp1 + 3 * 32]
			sub rd_uni, rd_temp1
			ret
		.simd_non_equal_b3:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp + 16]
				movdqu xmm6, [rsp + 0]
				add rsp, 32
			%endif
			vzeroupper
			not rd_uni
			rep bsf rd_uni, rd_uni
			and rd_uni, ~3
			movzx rq_uni, rd_uni
			mov rd_temp1, dword [ri_v2 + rq_uni + 2 * 32]
			mov rd_uni, dword [ri_v1 + rq_uni + 2 * 32]
			sub rd_uni, rd_temp1
			ret
	.end:

; ---------------------------------------------------------------------

SUS_PRIVATE_FUNC memcmp64_fallback
	.begin: 
	.qword_lbegin:
		test ri_v3, ri_v3
		jz .qword_ldone
	.qword_loop:
		mov rq_uni, [ri_v1]
		mov rq_temp1, [ri_v2]
		sub rq_uni, rq_temp1
		jnz .qword_ldone
		add ri_v1, 8
		add ri_v2, 8
		dec ri_v3
		jnz .qword_loop
	.qword_ldone:
		ret
	.end:

; ri_v1 = ptr1
; ri_v2 = ptr2
; ri_v3 = count
; rd_uni = return cmp
SUS_PUBLIC_FUNC memcmp64_sse2
	.begin:
		cmp ri_v3, 4
		jb memcmp64_fallback
		mov rw_temp2, 0xffff
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 16 - 1
		jz .align_done
		movdqu xmm0, [ri_v1]
		movdqu xmm1, [ri_v2]
		pcmpeqq xmm0, xmm1
		pmovmskb rd_uni, xmm0
		cmp rw_uni, rw_temp2
		jne .simd_non_equal_b1
		add ri_v1, ri_v4
		add ri_v2, ri_v4
		shr ri_v4, 3
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		%if SUS_ARCH_SYS_WINDOWS
			sub rsp, 32
			movdqu [rsp + 0], xmm6
			movdqu [rsp + 16], xmm7
		%endif
		shr ri_v4, 4 + 2 - 3
		jz .simd_ltail
	.simd_loop:
		movdqu xmm0, [ri_v1 + 16 * 0]
		movdqu xmm1, [ri_v1 + 16 * 1]
		movdqu xmm2, [ri_v1 + 16 * 2]
		movdqu xmm3, [ri_v1 + 16 * 3]
		movdqu xmm4, [ri_v2 + 16 * 0]
		movdqu xmm5, [ri_v2 + 16 * 1]
		movdqu xmm6, [ri_v2 + 16 * 2]
		movdqu xmm7, [ri_v2 + 16 * 3]
		pcmpeqq xmm0, xmm4
		pcmpeqq xmm1, xmm5
		pcmpeqq xmm2, xmm6
		pcmpeqq xmm3, xmm7
		pmovmskb rd_uni, xmm0
		pmovmskb rd_temp1, xmm1
		and rd_temp1, rd_uni
		cmp rw_temp1, rw_temp2
		jne .simd_non_equal_1
		pmovmskb rd_uni, xmm2
		pmovmskb rd_temp1, xmm3
		and rd_temp1, rd_uni
		cmp rw_temp1, rw_temp2
		jne .simd_non_equal_2
		add ri_v1, 16 * 4
		add ri_v2, 16 * 4
		dec ri_v4
		jnz .simd_loop
		and ri_v3, 16 * 4 / 4 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		and ri_v4, ~(16 / 4 - 1)
		jz .simd_ldone
		shl ri_v4, 2
		cmp ri_v4, 1 * 16
		je .simd_tail_b1
		cmp ri_v4, 2 * 16
		je .simd_tail_b2
		.simd_tail_b3:
			movdqu xmm2, [ri_v1 + 16 * 2]
			movdqu xmm6, [ri_v2 + 16 * 2]
			pcmpeqq xmm2, xmm6
			pmovmskb rd_uni, xmm2
			cmp rw_uni, rw_temp2
			jne .simd_non_equal_b3
		.simd_tail_b2:
			movdqu xmm1, [ri_v1 + 16 * 1]
			movdqu xmm5, [ri_v2 + 16 * 1]
			pcmpeqq xmm1, xmm5
			pmovmskb rd_temp1, xmm1
			cmp rw_temp1, rw_temp2
			jne .simd_non_equal_b2
		.simd_tail_b1:
			movdqu xmm0, [ri_v1 + 16 * 0]
			movdqu xmm4, [ri_v2 + 16 * 0]
			pcmpeqq xmm0, xmm4
			pmovmskb rd_uni, xmm0
			cmp rw_uni, rw_temp2
			jne .simd_non_equal_b1
		add ri_v1, ri_v4
		add ri_v2, ri_v4
		and ri_v3, 16 / 8 - 1
	.simd_ldone:
		%if SUS_ARCH_SYS_WINDOWS
			movdqu xmm7, [rsp+16]
			movdqu xmm6, [rsp+0]
			add rsp, 32
		%endif
		jmp memcmp64_fallback
	.simd_non_equal_1:
		cmp rw_uni, rw_temp2
		jne .simd_non_equal_b1
		.simd_non_equal_b2:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp+16]
				movdqu xmm6, [rsp+0]
				add rsp, 32
			%endif
			not rd_temp1
			rep bsf rd_temp1, rd_temp1
			and rd_temp1, ~7
			movzx rq_temp1, rd_temp1
			mov rq_uni, qword [ri_v1 + rq_temp1 + 1 * 16]
			mov rq_temp1, qword [ri_v2 + rq_temp1 + 1 * 16]
			sub rq_uni, rq_temp1
			ret
		.simd_non_equal_b1:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp+16]
				movdqu xmm6, [rsp+0]
				add rsp, 32
			%endif
			not rd_uni
			rep bsf rd_uni, rd_uni
			and rd_uni, ~7
			movzx rq_uni, rd_uni
			mov rq_temp1, qword [ri_v2 + rq_uni + 0 * 16]
			mov rq_uni, qword [ri_v1 + rq_uni + 0 * 16]
			sub rq_uni, rq_temp1
			ret
	.simd_non_equal_2:
		cmp rw_uni, rw_temp2
		jne .simd_non_equal_b3
		.simd_non_equal_b4:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp+16]
				movdqu xmm6, [rsp+0]
				add rsp, 32
			%endif
			not rd_temp1
			rep bsf rd_temp1, rd_temp1
			and rd_temp1, ~7
			movzx rq_temp1, rd_temp1
			mov rq_uni, qword [ri_v1 + rq_temp1 + 3 * 16]
			mov rq_temp1, qword [ri_v2 + rq_temp1 + 3 * 16]
			sub rq_uni, rq_temp1
			ret
		.simd_non_equal_b3:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp+16]
				movdqu xmm6, [rsp+0]
				add rsp, 32
			%endif
			not rd_uni
			rep bsf rd_uni, rd_uni
			and rd_uni, ~7
			movzx rq_uni, rd_uni
			mov rq_temp1, qword [ri_v2 + rq_uni + 2 * 16]
			mov rq_uni, qword [ri_v1 + rq_uni + 2 * 16]
			sub rq_uni, rq_temp1
			ret
	.end:

; ri_v1 = ptr1
; ri_v2 = ptr2
; ri_v3 = count
; rd_uni = return cmp
SUS_PUBLIC_FUNC memcmp64_avx2
	.begin:
		cmp ri_v3, 16
		jb memcmp64_sse2
		mov rd_temp2, 0xffffffff
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 32 - 1
		jz .align_done
		vmovdqu ymm0, [ri_v1]
		vmovdqu ymm1, [ri_v2]
		vpcmpeqq ymm0, ymm1
		vpmovmskb rd_uni, ymm0
		cmp rd_uni, rd_temp2
		jne .simd_non_equal_b1
		add ri_v1, ri_v4
		add ri_v2, ri_v4
		shr ri_v4, 3
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		%if SUS_ARCH_SYS_WINDOWS
			sub rsp, 32
			movdqu [rsp + 0], xmm6
			movdqu [rsp + 16], xmm7
		%endif
		shr ri_v4, 5 + 2 - 3
		jz .simd_ltail
	.simd_loop:
		vmovdqu ymm0, [ri_v1 + 32 * 0]
		vmovdqu ymm1, [ri_v1 + 32 * 1]
		vmovdqu ymm2, [ri_v1 + 32 * 2]
		vmovdqu ymm3, [ri_v1 + 32 * 3]
		vmovdqu ymm4, [ri_v2 + 32 * 0]
		vmovdqu ymm5, [ri_v2 + 32 * 1]
		vmovdqu ymm6, [ri_v2 + 32 * 2]
		vmovdqu ymm7, [ri_v2 + 32 * 3]
		vpcmpeqq ymm0, ymm4
		vpcmpeqq ymm1, ymm5
		vpcmpeqq ymm2, ymm6
		vpcmpeqq ymm3, ymm7
		vpmovmskb rd_uni, ymm0
		vpmovmskb rd_temp1, ymm1
		and rd_temp1, rd_uni
		cmp rd_temp1, rd_temp2
		jne .simd_non_equal_1
		vpmovmskb rd_uni, ymm2
		vpmovmskb rd_temp1, ymm3
		and rd_temp1, rd_uni
		cmp rd_temp1, rd_temp2
		jne .simd_non_equal_2
		add ri_v1, 32 * 4
		add ri_v2, 32 * 4
		dec ri_v4
		jnz .simd_loop
		and ri_v3, 32 * 4 / 8 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		and ri_v4, ~(32 / 8 - 1)
		jz .simd_ldone
		shl ri_v4, 3
		cmp ri_v4, 1 * 32
		je .simd_tail_b1
		cmp ri_v4, 2 * 32
		je .simd_tail_b2
		.simd_tail_b3:
			vmovdqu ymm2, [ri_v1 + 32 * 2]
			vmovdqu ymm6, [ri_v2 + 32 * 2]
			vpcmpeqq ymm2, ymm6
			vpmovmskb rd_uni, ymm2
			cmp rd_uni, rd_temp2
			jne .simd_non_equal_b3
		.simd_tail_b2:
			vmovdqu ymm1, [ri_v1 + 32 * 1]
			vmovdqu ymm5, [ri_v2 + 32 * 1]
			vpcmpeqq ymm1, ymm5
			vpmovmskb rd_temp1, ymm1
			cmp rd_temp1, rd_temp2
			jne .simd_non_equal_b2
		.simd_tail_b1:
			vmovdqu ymm0, [ri_v1 + 32 * 0]
			vmovdqu ymm4, [ri_v2 + 32 * 0]
			vpcmpeqq ymm0, ymm4
			vpmovmskb rd_uni, ymm0
			cmp rd_uni, rd_temp2
			jne .simd_non_equal_b1
		add ri_v1, ri_v4
		add ri_v2, ri_v4
		and ri_v3, 32 / 8 - 1
	.simd_ldone:
		%if SUS_ARCH_SYS_WINDOWS
			movdqu xmm7, [rsp + 16]
			movdqu xmm6, [rsp + 0]
			add rsp, 32
		%endif
		vzeroupper
		jmp memcmp64_fallback
	.simd_non_equal_1:
		cmp rd_uni, rd_temp2
		jne .simd_non_equal_b1
		.simd_non_equal_b2:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp + 16]
				movdqu xmm6, [rsp + 0]
				add rsp, 32
			%endif
			vzeroupper
			not rd_temp1
			rep bsf rd_temp1, rd_temp1
			and rd_temp1, ~7
			movzx rq_temp1, rd_temp1
			mov rq_uni, qword [ri_v1 + rq_temp1 + 1 * 32]
			mov rq_temp1, qword [ri_v2 + rq_temp1 + 1 * 32]
			sub rq_uni, rq_temp1
			ret
		.simd_non_equal_b1:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp + 16]
				movdqu xmm6, [rsp + 0]
				add rsp, 32
			%endif
			vzeroupper
			not rd_uni
			rep bsf rd_uni, rd_uni
			and rd_uni, ~7
			movzx rq_uni, rd_uni
			mov rq_temp1, qword [ri_v2 + rq_uni + 0 * 32]
			mov rq_uni, qword [ri_v1 + rq_uni + 0 * 32]
			sub rq_uni, rq_temp1
			ret
	.simd_non_equal_2:
		cmp rd_uni, rd_temp2
		jne .simd_non_equal_b3
		.simd_non_equal_b4:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp + 16]
				movdqu xmm6, [rsp + 0]
				add rsp, 32
			%endif
			vzeroupper
			not rd_temp1
			rep bsf rd_temp1, rd_temp1
			and rd_temp1, ~7
			movzx rq_temp1, rd_temp1
			mov rq_uni, qword [ri_v1 + rq_temp1 + 3 * 32]
			mov rq_temp1, qword [ri_v2 + rq_temp1 + 3 * 32]
			sub rq_uni, rq_temp1
			ret
		.simd_non_equal_b3:
			%if SUS_ARCH_SYS_WINDOWS
				movdqu xmm7, [rsp + 16]
				movdqu xmm6, [rsp + 0]
				add rsp, 32
			%endif
			vzeroupper
			not rd_uni
			rep bsf rd_uni, rd_uni
			and rd_uni, ~7
			movzx rq_uni, rd_uni
			mov rq_temp1, qword [ri_v2 + rq_uni + 2 * 32]
			mov rq_uni, qword [ri_v1 + rq_uni + 2 * 32]
			sub rq_uni, rq_temp1
			ret
	.end:

; ---------------------------------------------------------------------

SUS_PRIVATE_FUNC memchr_fallback
	.begin:
	.qword_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 3
		jz .qword_ldone
		push rq_save1
		push rq_save2
		mov rq_save1, 0x0101010101010101
		mov rq_save2, 0x8080808080808080
	.qword_loop:
		mov rq_uni, [ri_v1]
		xor rq_uni, rq_temp2
		mov rq_temp1, rq_save1
		sub rq_temp1, rq_uni
		not rq_uni
		and rq_uni, rq_temp1
		and rq_uni, rq_save2
		jnz .qword_found
		add ri_v1, 8
		dec ri_v4
		jnz .qword_loop
		pop rq_save2
		pop rq_save1
		and ri_v3, 7
	.qword_ldone:
	.byte_lbegin:
		test ri_v3, ri_v3
		jz .byte_ldone
	.byte_loop:
		mov rb_uni, [ri_v1]
		cmp rb_uni, ri_v2b
		je .byte_found
		inc ri_v1
		dec ri_v3
		jnz .byte_loop
	.byte_ldone:
		xor rq_uni, rq_uni
		ret
	.byte_found:
		mov rq_uni, ri_v1
		ret
	.qword_found:
		pop rq_save2
		pop rq_save1
		rep bsf rq_uni, rq_uni
		shr rq_uni, 3
		add rq_uni, ri_v1
		ret
	.end:

; ri_v1 = ptr1
; ri_v2d = value
; ri_v3 = count
; rq_uni = return ptr
SUS_PUBLIC_FUNC memchr_sse2
	.begin:
		movzx ri_v2, ri_v2d
		mov rq_temp2, 0x0101010101010101
		imul rq_temp2, ri_v2
		cmp ri_v3, 32
		jb memchr_fallback
		movq xmm0, rq_temp2
		punpcklqdq xmm0, xmm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 16 - 1
		jz .align_done
		movdqu xmm1, [ri_v1]
		pcmpeqb xmm1, xmm0
		pmovmskb rd_uni, xmm1
		test rd_uni, rd_uni
		jnz .simd_found_b1
		add ri_v1, ri_v4
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 4 + 2
		jz .simd_ltail
	.simd_loop:
		movdqu xmm1, [ri_v1 + 16 * 0]
		movdqu xmm2, [ri_v1 + 16 * 1]
		movdqu xmm3, [ri_v1 + 16 * 2]
		movdqu xmm4, [ri_v1 + 16 * 3]
		pcmpeqb xmm1, xmm0
		pcmpeqb xmm2, xmm0
		pcmpeqb xmm3, xmm0
		pcmpeqb xmm4, xmm0
		pmovmskb rd_uni, xmm1
		pmovmskb rd_temp1, xmm2
		or rd_temp1, rd_uni
		jnz .simd_found_1
		pmovmskb rd_uni, xmm3
		pmovmskb rd_temp1, xmm4
		or rd_temp1, rd_uni
		jnz .simd_found_2
		add ri_v1, 16 * 4
		dec ri_v4
		jnz .simd_loop
		and ri_v3, 16 * 4 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		and ri_v4, ~(16 - 1)
		jz .simd_ldone
		cmp ri_v4, 1 * 16
		je .simd_tail_b1
		cmp ri_v4, 2 * 16
		je .simd_tail_b2
		.simd_tail_b3:
			movdqu xmm3, [ri_v1 + 16 * 2]
			pcmpeqb xmm3, xmm0
			pmovmskb rd_uni, xmm3
			jnz .simd_found_b3
		.simd_tail_b2:
			movdqu xmm2, [ri_v1 + 16 * 1]
			pcmpeqb xmm2, xmm0
			pmovmskb rd_temp1, xmm2
			jnz .simd_found_b2
		.simd_tail_b1:
			movdqu xmm1, [ri_v1 + 16 * 0]
			pcmpeqb xmm1, xmm0
			pmovmskb rd_uni, xmm1
			jnz .simd_found_b1
		add ri_v1, ri_v4
		and ri_v3, 16 - 1
	.simd_ldone:
		jmp memchr_fallback
	.simd_found_1:
		test rd_uni, rd_uni
		jnz .simd_found_b1
		.simd_found_b2:
			rep bsf rd_temp1, rd_temp1
			movzx rq_temp1, rd_temp1
			lea rq_uni, [rq_temp1 + ri_v1 + 16]
			ret
		.simd_found_b1:
			rep bsf rd_uni, rd_uni
			movzx rq_uni, rd_uni
			add rq_uni, ri_v1
			ret
	.simd_found_2:
		test rd_uni, rd_uni
		jnz .simd_found_b3
		.simd_found_b4:
			rep bsf rd_temp1, rd_temp1
			movzx rq_temp1, rd_temp1
			lea rq_uni, [rq_temp1 + ri_v1 + 16 * 3]
			ret
		.simd_found_b3:
			rep bsf rd_uni, rd_uni
			movzx rq_uni, rd_uni
			lea rq_uni, [rq_uni + ri_v1 + 16 * 2]
			ret
	.end:
		
; ri_v1 = ptr1
; ri_v2d = value
; ri_v3 = count
; rq_uni = return ptr
SUS_PUBLIC_FUNC memchr_avx2
	.begin:
		cmp ri_v3, 128
		jb memchr_sse2
		vmovd xmm0, ri_v2d
		vpbroadcastb ymm0, xmm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 32 - 1
		jz .align_done
		vmovdqu ymm1, [ri_v1]
		vpcmpeqb ymm1, ymm0
		vpmovmskb rd_uni, ymm1
		test rd_uni, rd_uni
		jnz .simd_found_b1
		add ri_v1, ri_v4
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 5 + 2
		jz .simd_ltail
	.simd_loop:
		vmovdqu ymm1, [ri_v1 + 32 * 0]
		vmovdqu ymm2, [ri_v1 + 32 * 1]
		vmovdqu ymm3, [ri_v1 + 32 * 2]
		vmovdqu ymm4, [ri_v1 + 32 * 3]
		vpcmpeqb ymm1, ymm0
		vpcmpeqb ymm2, ymm0
		vpcmpeqb ymm3, ymm0
		vpcmpeqb ymm4, ymm0
		vpmovmskb rd_uni, ymm1
		vpmovmskb rd_temp1, ymm2
		or rd_temp1, rd_uni
		jnz .simd_found_1
		vpmovmskb rd_uni, ymm3
		vpmovmskb rd_temp1, ymm4
		or rd_temp1, rd_uni
		jnz .simd_found_2
		add ri_v1, 32 * 4
		dec ri_v4
		jnz .simd_loop
		and ri_v3, 32 * 4 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		and ri_v4, ~(32 - 1)
		jz .simd_ldone
		cmp ri_v4, 1 * 32
		je .simd_tail_b1
		cmp ri_v4, 2 * 32
		je .simd_tail_b2
		.simd_tail_b3:
			vmovdqu ymm3, [ri_v1 + 32 * 2]
			vpcmpeqb ymm3, ymm0
			vpmovmskb rd_uni, ymm3
			jnz .simd_found_b3
		.simd_tail_b2:
			vmovdqu ymm2, [ri_v1 + 32 * 1]
			vpcmpeqb ymm2, ymm0
			vpmovmskb rd_temp1, ymm2
			jnz .simd_found_b2
		.simd_tail_b1:
			vmovdqu ymm1, [ri_v1 + 32 * 0]
			vpcmpeqb ymm1, ymm0
			vpmovmskb rd_uni, ymm1
			jnz .simd_found_b1
		add ri_v1, ri_v4
		and ri_v3, 32 - 1
	.simd_ldone:
		vzeroupper
		movzx ri_v2, ri_v2d
		mov rq_temp2, 0x0101010101010101
		imul rq_temp2, ri_v2
		jmp memchr_fallback
	.simd_found_1:
		test rd_uni, rd_uni
		jnz .simd_found_b1
		.simd_found_b2:
			vzeroupper
			rep bsf rd_temp1, rd_temp1
			movzx rq_temp1, rd_temp1
			lea rq_uni, [rq_temp1 + ri_v1 + 32]
			ret
		.simd_found_b1:
			vzeroupper
			rep bsf rd_uni, rd_uni
			movzx rq_uni, rd_uni
			add rq_uni, ri_v1
			ret
	.simd_found_2:
		test rd_uni, rd_uni
		jnz .simd_found_b3
		.simd_found_b4:
			vzeroupper
			rep bsf rd_temp1, rd_temp1
			movzx rq_temp1, rd_temp1
			lea rq_uni, [rq_temp1 + ri_v1 + 32 * 3]
			ret
		.simd_found_b3:
			vzeroupper
			rep bsf rd_uni, rd_uni
			movzx rq_uni, rd_uni
			lea rq_uni, [rq_uni + ri_v1 + 32 * 2]
			ret
	.end:

; ---------------------------------------------------------------------

SUS_PRIVATE_FUNC memchr16_fallback
	.begin:
	.qword_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 3 - 1
		jz .qword_ldone
		push rq_save1
		push rq_save2
		mov rq_save1, 0x0001000100010001
		mov rq_save2, 0x8000800080008000
	.qword_loop:
		mov rq_uni, [ri_v1]
		xor rq_uni, rq_temp2
		mov rq_temp1, rq_save1
		sub rq_temp1, rq_uni
		not rq_uni
		and rq_uni, rq_temp1
		and rq_uni, rq_save2
		jnz .qword_found
		add ri_v1, 8
		dec ri_v4
		jnz .qword_loop
		pop rq_save2
		pop rq_save1
		and ri_v3, 3
	.qword_ldone:
	.word_lbegin:
		test ri_v3, ri_v3
		jz .word_ldone
	.word_loop:
		mov rw_uni, [ri_v1]
		cmp rw_uni, ri_v2w
		je .word_found
		add ri_v1, 2
		dec ri_v3
		jnz .word_loop
	.word_ldone:
		xor rq_uni, rq_uni
		ret
	.word_found:
		mov rq_uni, ri_v1
		ret
	.qword_found:
		pop rq_save2
		pop rq_save1
		rep bsf rq_uni, rq_uni
		and rq_uni, ~1
		add rq_uni, ri_v1
		ret
	.end:

; ri_v1 = ptr1
; ri_v2w = value
; ri_v3 = count
; rq_uni = return ptr
SUS_PUBLIC_FUNC memchr16_sse2
	.begin:
		movzx ri_v2, ri_v2w
		mov rq_temp2, 0x0001000100010001
		imul rq_temp2, ri_v2
		cmp ri_v3, 16
		jb memchr16_fallback
		movq xmm0, rq_temp2
		punpcklqdq xmm0, xmm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 16 - 1
		jz .align_done
		movdqu xmm1, [ri_v1]
		pcmpeqw xmm1, xmm0
		pmovmskb rd_uni, xmm1
		test rd_uni, rd_uni
		jnz .simd_found_b1
		add ri_v1, ri_v4
		shr ri_v4, 1
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 4 + 2 - 1
		jz .simd_ltail
	.simd_loop:
		movdqu xmm1, [ri_v1 + 16 * 0]
		movdqu xmm2, [ri_v1 + 16 * 1]
		movdqu xmm3, [ri_v1 + 16 * 2]
		movdqu xmm4, [ri_v1 + 16 * 3]
		pcmpeqw xmm1, xmm0
		pcmpeqw xmm2, xmm0
		pcmpeqw xmm3, xmm0
		pcmpeqw xmm4, xmm0
		pmovmskb rd_uni, xmm1
		pmovmskb rd_temp1, xmm2
		or rd_temp1, rd_uni
		jnz .simd_found_1
		pmovmskb rd_uni, xmm3
		pmovmskb rd_temp1, xmm4
		or rd_temp1, rd_uni
		jnz .simd_found_2
		add ri_v1, 16 * 4
		dec ri_v4
		jnz .simd_loop
		and ri_v3, 16 * 4 / 2 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		and ri_v4, ~(16 / 2 - 1)
		jz .simd_ldone
		cmp ri_v4, 1 * 16
		je .simd_tail_b1
		cmp ri_v4, 2 * 16
		je .simd_tail_b2
		.simd_tail_b3:
			movdqu xmm3, [ri_v1 + 16 * 2]
			pcmpeqw xmm3, xmm0
			pmovmskb rd_uni, xmm3
			jnz .simd_found_b3
		.simd_tail_b2:
			movdqu xmm2, [ri_v1 + 16 * 1]
			pcmpeqw xmm2, xmm0
			pmovmskb rd_temp1, xmm2
			jnz .simd_found_b2
		.simd_tail_b1:
			movdqu xmm1, [ri_v1 + 16 * 0]
			pcmpeqw xmm1, xmm0
			pmovmskb rd_uni, xmm1
			jnz .simd_found_b1
		add ri_v1, ri_v4
		and ri_v3, 16 / 2 - 1
	.simd_ldone:
		jmp memchr16_fallback
	.simd_found_1:
		test rd_uni, rd_uni
		jnz .simd_found_b1
		.simd_found_b2:
			rep bsf rd_temp1, rd_temp1
			and rd_temp1, ~1
			movzx rq_temp1, rd_temp1
			lea rq_uni, [rq_temp1 + ri_v1 + 16]
			ret
		.simd_found_b1:
			rep bsf rd_uni, rd_uni
			and rd_uni, ~1
			movzx rq_uni, rd_uni
			add rq_uni, ri_v1
			ret
	.simd_found_2:
		test rd_uni, rd_uni
		jnz .simd_found_b3
		.simd_found_b4:
			rep bsf rd_temp1, rd_temp1
			and rd_temp1, ~1
			movzx rq_temp1, rd_temp1
			lea rq_uni, [rq_temp1 + ri_v1 + 16 * 3]
			ret
		.simd_found_b3:
			rep bsf rd_uni, rd_uni
			and rd_uni, ~1
			movzx rq_uni, rd_uni
			lea rq_uni, [rq_uni + ri_v1 + 16 * 2]
			ret
	.end:
	
; ri_v1 = ptr1
; ri_v2w = value
; ri_v3 = count
; rq_uni = return ptr
SUS_PUBLIC_FUNC memchr16_avx2
	.begin:
		cmp ri_v3, 64
		jb memchr16_sse2
		movzx ri_v2, ri_v2w
		vmovd xmm0, ri_v2d
		vpbroadcastw ymm0, xmm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 32 - 1
		jz .align_done
		vmovdqu ymm1, [ri_v1]
		vpcmpeqw ymm1, ymm0
		vpmovmskb rd_uni, ymm1
		test rd_uni, rd_uni
		jnz .simd_found_b1
		add ri_v1, ri_v4
		shr ri_v4, 1
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 5 + 2 - 1
		jz .simd_ltail
	.simd_loop:
		vmovdqu ymm1, [ri_v1 + 32 * 0]
		vmovdqu ymm2, [ri_v1 + 32 * 1]
		vmovdqu ymm3, [ri_v1 + 32 * 2]
		vmovdqu ymm4, [ri_v1 + 32 * 3]
		vpcmpeqw ymm1, ymm0
		vpcmpeqw ymm2, ymm0
		vpcmpeqw ymm3, ymm0
		vpcmpeqw ymm4, ymm0
		vpmovmskb rd_uni, ymm1
		vpmovmskb rd_temp1, ymm2
		or rd_temp1, rd_uni
		jnz .simd_found_1
		vpmovmskb rd_uni, ymm3
		vpmovmskb rd_temp1, ymm4
		or rd_temp1, rd_uni
		jnz .simd_found_2
		add ri_v1, 32 * 4
		dec ri_v4
		jnz .simd_loop
		and ri_v3, 32 * 4 / 2 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		and ri_v4, ~(32 / 2 - 1)
		jz .simd_ldone
		cmp ri_v4, 1 * 32
		je .simd_tail_b1
		cmp ri_v4, 2 * 32
		je .simd_tail_b2
		.simd_tail_b3:
			vmovdqu ymm3, [ri_v1 + 32 * 2]
			vpcmpeqw ymm3, ymm0
			vpmovmskb rd_uni, ymm3
			jnz .simd_found_b3
		.simd_tail_b2:
			vmovdqu ymm2, [ri_v1 + 32 * 1]
			vpcmpeqw ymm2, ymm0
			vpmovmskb rd_temp1, ymm2
			jnz .simd_found_b2
		.simd_tail_b1:
			vmovdqu ymm1, [ri_v1 + 32 * 0]
			vpcmpeqw ymm1, ymm0
			vpmovmskb rd_uni, ymm1
			jnz .simd_found_b1
		add ri_v1, ri_v4
		and ri_v3, 32 / 2 - 1
	.simd_ldone:
		vzeroupper
		mov rq_temp2, 0x0001000100010001
		imul rq_temp2, ri_v2
		jmp memchr16_fallback
	.simd_found_1:
		test rd_uni, rd_uni
		jnz .simd_found_b1
		.simd_found_b2:
			vzeroupper
			rep bsf rd_temp1, rd_temp1
			movzx rq_temp1, rd_temp1
			and rq_temp1, ~1
			lea rq_uni, [rq_temp1 + ri_v1 + 32]
			ret
		.simd_found_b1:
			vzeroupper
			rep bsf rd_uni, rd_uni
			movzx rq_uni, rd_uni
			and rq_uni, ~1
			add rq_uni, ri_v1
			ret
	.simd_found_2:
		test rd_uni, rd_uni
		jnz .simd_found_b3
		.simd_found_b4:
			vzeroupper
			rep bsf rd_temp1, rd_temp1
			movzx rq_temp1, rd_temp1
			and rq_temp1, ~1
			lea rq_uni, [rq_temp1 + ri_v1 + 32 * 3]
			ret
		.simd_found_b3:
			vzeroupper
			rep bsf rd_uni, rd_uni
			movzx rq_uni, rd_uni
			and rq_uni, ~1
			lea rq_uni, [rq_uni + ri_v1 + 32 * 2]
			ret
	.end:

; ---------------------------------------------------------------------

SUS_PRIVATE_FUNC memchr32_fallback
	.begin:
	.qword_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 3 - 2
		jz .qword_ldone
		push rq_save1
		push rq_save2
		mov rq_save1, 0x0000000100000001
		mov rq_save2, 0x8000000080000000
	.qword_loop:
		mov rq_uni, [ri_v1]
		xor rq_uni, rq_temp2
		mov rq_temp1, rq_save1
		sub rq_temp1, rq_uni
		not rq_uni
		and rq_uni, rq_temp1
		and rq_uni, rq_save2
		jnz .qword_found
		add ri_v1, 8
		dec ri_v4
		jnz .qword_loop
		pop rq_save2
		pop rq_save1
		and ri_v3, 1
	.qword_ldone:
	.word_lbegin:
		test ri_v3, ri_v3
		jz .word_ldone
	.word_loop:
		mov rd_uni, [ri_v1]
		cmp rd_uni, ri_v2d
		je .word_found
		add ri_v1, 4
		dec ri_v3
		jnz .word_loop
	.word_ldone:
		xor rq_uni, rq_uni
		ret
	.word_found:
		mov rq_uni, ri_v1
		ret
	.qword_found:
		pop rq_save2
		pop rq_save1
		rep bsf rq_uni, rq_uni
		and rq_uni, ~3
		add rq_uni, ri_v1
		ret
	.end:

; ri_v1 = ptr1
; ri_v2d = value
; ri_v3 = count
; rq_uni = return ptr
SUS_PUBLIC_FUNC memchr32_sse2
	.begin:
		movzx ri_v2, ri_v2d
		mov rq_temp2, 0x0000000100000001
		imul rq_temp2, ri_v2
		cmp ri_v3, 8
		jb memchr32_fallback
		movq xmm0, rq_temp2
		punpcklqdq xmm0, xmm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 16 - 1
		jz .align_done
		movdqu xmm1, [ri_v1]
		pcmpeqd xmm1, xmm0
		pmovmskb rd_uni, xmm1
		test rd_uni, rd_uni
		jnz .simd_found_b1
		add ri_v1, ri_v4
		shr ri_v4, 2
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 4 + 2 - 2
		jz .simd_ltail
	.simd_loop:
		movdqu xmm1, [ri_v1 + 16 * 0]
		movdqu xmm2, [ri_v1 + 16 * 1]
		movdqu xmm3, [ri_v1 + 16 * 2]
		movdqu xmm4, [ri_v1 + 16 * 3]
		pcmpeqd xmm1, xmm0
		pcmpeqd xmm2, xmm0
		pcmpeqd xmm3, xmm0
		pcmpeqd xmm4, xmm0
		pmovmskb rd_uni, xmm1
		pmovmskb rd_temp1, xmm2
		or rd_temp1, rd_uni
		jnz .simd_found_1
		pmovmskb rd_uni, xmm3
		pmovmskb rd_temp1, xmm4
		or rd_temp1, rd_uni
		jnz .simd_found_2
		add ri_v1, 16 * 4
		dec ri_v4
		jnz .simd_loop
		and ri_v3, 16 * 4 / 4 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		and ri_v4, ~(16 / 4 - 1)
		jz .simd_ldone
		cmp ri_v4, 1 * 16
		je .simd_tail_b1
		cmp ri_v4, 2 * 16
		je .simd_tail_b2
		.simd_tail_b3:
			movdqu xmm3, [ri_v1 + 16 * 2]
			pcmpeqd xmm3, xmm0
			pmovmskb rd_uni, xmm3
			jnz .simd_found_b3
		.simd_tail_b2:
			movdqu xmm2, [ri_v1 + 16 * 1]
			pcmpeqd xmm2, xmm0
			pmovmskb rd_temp1, xmm2
			jnz .simd_found_b2
		.simd_tail_b1:
			movdqu xmm1, [ri_v1 + 16 * 0]
			pcmpeqd xmm1, xmm0
			pmovmskb rd_uni, xmm1
			jnz .simd_found_b1
		add ri_v1, ri_v4
		and ri_v3, 16 / 4 - 1
	.simd_ldone:
		jmp memchr32_fallback
	.simd_found_1:
		test rd_uni, rd_uni
		jnz .simd_found_b1
		.simd_found_b2:
			rep bsf rd_temp1, rd_temp1
			and rd_temp1, ~3
			movzx rq_temp1, rd_temp1
			lea rq_uni, [rq_temp1 + ri_v1 + 16]
			ret
		.simd_found_b1:
			rep bsf rd_uni, rd_uni
			and rd_uni, ~3
			movzx rq_uni, rd_uni
			add rq_uni, ri_v1
			ret
	.simd_found_2:
		test rd_uni, rd_uni
		jnz .simd_found_b3
		.simd_found_b4:
			rep bsf rd_temp1, rd_temp1
			and rd_temp1, ~3
			movzx rq_temp1, rd_temp1
			lea rq_uni, [rq_temp1 + ri_v1 + 16 * 3]
			ret
		.simd_found_b3:
			rep bsf rd_uni, rd_uni
			and rd_uni, ~3
			movzx rq_uni, rd_uni
			lea rq_uni, [rq_uni + ri_v1 + 16 * 2]
			ret
	.end:

; ri_v1 = ptr1
; ri_v2d = value
; ri_v3 = count
; rq_uni = return ptr
SUS_PUBLIC_FUNC memchr32_avx2
	.begin:
		cmp ri_v3, 32
		jb memchr32_sse2
		vmovd xmm0, ri_v2d
		vpbroadcastd ymm0, xmm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 32 - 1
		jz .align_done
		vmovdqu ymm1, [ri_v1]
		vpcmpeqd ymm1, ymm0
		vpmovmskb rd_uni, ymm1
		test rd_uni, rd_uni
		jnz .simd_found_b1
		add ri_v1, ri_v4
		shr ri_v4, 2
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 5 + 2 - 2
		jz .simd_ltail
	.simd_loop:
		vmovdqu ymm1, [ri_v1 + 32 * 0]
		vmovdqu ymm2, [ri_v1 + 32 * 1]
		vmovdqu ymm3, [ri_v1 + 32 * 2]
		vmovdqu ymm4, [ri_v1 + 32 * 3]
		vpcmpeqd ymm1, ymm0
		vpcmpeqd ymm2, ymm0
		vpcmpeqd ymm3, ymm0
		vpcmpeqd ymm4, ymm0
		vpmovmskb rd_uni, ymm1
		vpmovmskb rd_temp1, ymm2
		or rd_temp1, rd_uni
		jnz .simd_found_1
		vpmovmskb rd_uni, ymm3
		vpmovmskb rd_temp1, ymm4
		or rd_temp1, rd_uni
		jnz .simd_found_2
		add ri_v1, 32 * 4
		dec ri_v4
		jnz .simd_loop
		and ri_v3, 32 * 4 / 4 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		and ri_v4, ~(32 / 4 - 1)
		jz .simd_ldone
		cmp ri_v4, 1 * 32
		je .simd_tail_b1
		cmp ri_v4, 2 * 32
		je .simd_tail_b2
		.simd_tail_b3:
			vmovdqu ymm3, [ri_v1 + 32 * 2]
			vpcmpeqd ymm3, ymm0
			vpmovmskb rd_uni, ymm3
			jnz .simd_found_b3
		.simd_tail_b2:
			vmovdqu ymm2, [ri_v1 + 32 * 1]
			vpcmpeqd ymm2, ymm0
			vpmovmskb rd_temp1, ymm2
			jnz .simd_found_b2
		.simd_tail_b1:
			vmovdqu ymm1, [ri_v1 + 32 * 0]
			vpcmpeqd ymm1, ymm0
			vpmovmskb rd_uni, ymm1
			jnz .simd_found_b1
		add ri_v1, ri_v4
		and ri_v3, 32 / 4 - 1
	.simd_ldone:
		vzeroupper
		movzx ri_v2, ri_v2d
		mov rq_temp2, 0x0000000100000001
		imul rq_temp2, ri_v2
		jmp memchr32_fallback
	.simd_found_1:
		test rd_uni, rd_uni
		jnz .simd_found_b1
		.simd_found_b2:
			vzeroupper
			rep bsf rd_temp1, rd_temp1
			movzx rq_temp1, rd_temp1
			and rq_temp1, ~3
			lea rq_uni, [rq_temp1 + ri_v1 + 32]
			ret
		.simd_found_b1:
			vzeroupper
			rep bsf rd_uni, rd_uni
			movzx rq_uni, rd_uni
			and rq_uni, ~3
			add rq_uni, ri_v1
			ret
	.simd_found_2:
		test rd_uni, rd_uni
		jnz .simd_found_b3
		.simd_found_b4:
			vzeroupper
			rep bsf rd_temp1, rd_temp1
			movzx rq_temp1, rd_temp1
			and rq_temp1, ~3
			lea rq_uni, [rq_temp1 + ri_v1 + 32 * 3]
			ret
		.simd_found_b3:
			vzeroupper
			rep bsf rd_uni, rd_uni
			movzx rq_uni, rd_uni
			and rq_uni, ~3
			lea rq_uni, [rq_uni + ri_v1 + 32 * 2]
			ret
	.end:

; ---------------------------------------------------------------------

SUS_PRIVATE_FUNC memchr64_fallback
	.begin:
	.qword_lbegin:
		test ri_v3, ri_v3
		jz .qword_ldone
	.qword_loop:
		mov rq_uni, [ri_v1]
		cmp rq_uni, ri_v2
		je .found_qword
		add ri_v1, 8
		dec ri_v3
		jnz .qword_loop
	.qword_ldone:
		xor rq_uni, rq_uni
		ret
	.found_qword:
		mov rq_uni, ri_v1
		ret
	.end:

; ri_v1 = ptr1
; ri_v2 = value
; ri_v3 = count
; rq_uni = return ptr
SUS_PUBLIC_FUNC memchr64_sse2
	.begin:
		cmp ri_v3, 4
		jb memchr64_fallback
		movq xmm0, ri_v2
		punpcklqdq xmm0, xmm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 16 - 1
		jz .align_done
		movdqu xmm1, [ri_v1]
		pcmpeqq xmm1, xmm0
		pmovmskb rd_uni, xmm1
		test rd_uni, rd_uni
		jnz .simd_found_b1
		add ri_v1, ri_v4
		shr ri_v4, 3
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 4 + 2 - 3
		jz .simd_ltail
	.simd_loop:
		movdqu xmm1, [ri_v1 + 16 * 0]
		movdqu xmm2, [ri_v1 + 16 * 1]
		movdqu xmm3, [ri_v1 + 16 * 2]
		movdqu xmm4, [ri_v1 + 16 * 3]
		pcmpeqq xmm1, xmm0
		pcmpeqq xmm2, xmm0
		pcmpeqq xmm3, xmm0
		pcmpeqq xmm4, xmm0
		pmovmskb rd_uni, xmm1
		pmovmskb rd_temp1, xmm2
		or rd_temp1, rd_uni
		jnz .simd_found_1
		pmovmskb rd_uni, xmm3
		pmovmskb rd_temp1, xmm4
		or rd_temp1, rd_uni
		jnz .simd_found_2
		add ri_v1, 16 * 4
		dec ri_v4
		jnz .simd_loop
		and ri_v3, 16 * 4 / 8 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		and ri_v4, ~(16 / 8 - 1)
		jz .simd_ldone
		cmp ri_v4, 1 * 16
		je .simd_tail_b1
		cmp ri_v4, 2 * 16
		je .simd_tail_b2
		.simd_tail_b3:
			movdqu xmm3, [ri_v1 + 16 * 2]
			pcmpeqq xmm3, xmm0
			pmovmskb rd_uni, xmm3
			jnz .simd_found_b3
		.simd_tail_b2:
			movdqu xmm2, [ri_v1 + 16 * 1]
			pcmpeqq xmm2, xmm0
			pmovmskb rd_temp1, xmm2
			jnz .simd_found_b2
		.simd_tail_b1:
			movdqu xmm1, [ri_v1 + 16 * 0]
			pcmpeqq xmm1, xmm0
			pmovmskb rd_uni, xmm1
			jnz .simd_found_b1
		add ri_v1, ri_v4
		and ri_v3, 16 / 8 - 1
	.simd_ldone:
		jmp memchr64_fallback
	.simd_found_1:
		test rd_uni, rd_uni
		jnz .simd_found_b1
		.simd_found_b2:
			rep bsf rd_temp1, rd_temp1
			and rd_temp1, ~7
			movzx rq_temp1, rd_temp1
			lea rq_uni, [rq_temp1 + ri_v1 + 16]
			ret
		.simd_found_b1:
			rep bsf rd_uni, rd_uni
			and rd_uni, ~7
			movzx rq_uni, rd_uni
			add rq_uni, ri_v1
			ret
	.simd_found_2:
		test rd_uni, rd_uni
		jnz .simd_found_b3
		.simd_found_b4:
			rep bsf rd_temp1, rd_temp1
			and rd_temp1, ~7
			movzx rq_temp1, rd_temp1
			lea rq_uni, [rq_temp1 + ri_v1 + 16 * 3]
			ret
		.simd_found_b3:
			rep bsf rd_uni, rd_uni
			and rd_uni, ~7
			movzx rq_uni, rd_uni
			lea rq_uni, [rq_uni + ri_v1 + 16 * 2]
			ret
	.end:

; ri_v1 = ptr1
; ri_v2 = value
; ri_v3 = count
; rq_uni = return ptr
SUS_PUBLIC_FUNC memchr64_avx2
	.begin:
		cmp ri_v3, 16
		jb memchr64_sse2
		vmovq xmm0, ri_v2
		vpbroadcastd ymm0, xmm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 32 - 1
		jz .align_done
		vmovdqu ymm1, [ri_v1]
		vpcmpeqq ymm1, ymm0
		vpmovmskb rd_uni, ymm1
		test rd_uni, rd_uni
		jnz .simd_found_b1
		add ri_v1, ri_v4
		shr ri_v4, 3
		sub ri_v3, ri_v4
	.align_done:
	.simd_lbegin:
		mov ri_v4, ri_v3
		shr ri_v4, 5 + 2 - 3
		jz .simd_ltail
	.simd_loop:
		vmovdqu ymm1, [ri_v1 + 32 * 0]
		vmovdqu ymm2, [ri_v1 + 32 * 1]
		vmovdqu ymm3, [ri_v1 + 32 * 2]
		vmovdqu ymm4, [ri_v1 + 32 * 3]
		vpcmpeqq ymm1, ymm0
		vpcmpeqq ymm2, ymm0
		vpcmpeqq ymm3, ymm0
		vpcmpeqq ymm4, ymm0
		vpmovmskb rd_uni, ymm1
		vpmovmskb rd_temp1, ymm2
		or rd_temp1, rd_uni
		jnz .simd_found_1
		vpmovmskb rd_uni, ymm3
		vpmovmskb rd_temp1, ymm4
		or rd_temp1, rd_uni
		jnz .simd_found_2
		add ri_v1, 32 * 4
		dec ri_v4
		jnz .simd_loop
		and ri_v3, 32 * 4 / 8 - 1
	.simd_ltail:
		mov ri_v4, ri_v3
		and ri_v4, ~(32 / 8 - 1)
		jz .simd_ldone
		cmp ri_v4, 1 * 32
		je .simd_tail_b1
		cmp ri_v4, 2 * 32
		je .simd_tail_b2
		.simd_tail_b3:
			vmovdqu ymm3, [ri_v1 + 32 * 2]
			vpcmpeqq ymm3, ymm0
			vpmovmskb rd_uni, ymm3
			jnz .simd_found_b3
		.simd_tail_b2:
			vmovdqu ymm2, [ri_v1 + 32 * 1]
			vpcmpeqq ymm2, ymm0
			vpmovmskb rd_temp1, ymm2
			jnz .simd_found_b2
		.simd_tail_b1:
			vmovdqu ymm1, [ri_v1 + 32 * 0]
			vpcmpeqq ymm1, ymm0
			vpmovmskb rd_uni, ymm1
			jnz .simd_found_b1
		add ri_v1, ri_v4
		and ri_v3, 32 / 8 - 1
	.simd_ldone:
		vzeroupper
		jmp memchr64_fallback
	.simd_found_1:
		test rd_uni, rd_uni
		jnz .simd_found_b1
		.simd_found_b2:
			vzeroupper
			rep bsf rd_temp1, rd_temp1
			movzx rq_temp1, rd_temp1
			and rq_temp1, ~7
			lea rq_uni, [rq_temp1 + ri_v1 + 32]
			ret
		.simd_found_b1:
			vzeroupper
			rep bsf rd_uni, rd_uni
			movzx rq_uni, rd_uni
			and rq_uni, ~7
			add rq_uni, ri_v1
			ret
	.simd_found_2:
		test rd_uni, rd_uni
		jnz .simd_found_b3
		.simd_found_b4:
			vzeroupper
			rep bsf rd_temp1, rd_temp1
			movzx rq_temp1, rd_temp1
			and rq_temp1, ~7
			lea rq_uni, [rq_temp1 + ri_v1 + 32 * 3]
			ret
		.simd_found_b3:
			vzeroupper
			rep bsf rd_uni, rd_uni
			movzx rq_uni, rd_uni
			and rq_uni, ~7
			lea rq_uni, [rq_uni + ri_v1 + 32 * 2]
			ret
	.end:

; ---------------------------------------------------------------------

; ri_v1 = str
; rq_uni = return len
SUS_PUBLIC_FUNC strlen_sse2
	.begin:
		mov rq_temp1, ri_v1
		pxor xmm0, xmm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 16 - 1
		jz .align_done
		movdqu xmm1, [ri_v1]
		pcmpeqb xmm1, xmm0
		pmovmskb rd_uni, xmm1
		test rd_uni, rd_uni
		jnz .found_b1
		add ri_v1, ri_v4
	.align_done:
	.simd_lbegin:
		jmp .simd_lbegin_entry
	.simd_loop:
		add ri_v1, 16 * 2
	.simd_lbegin_entry:
		movdqa xmm1, [ri_v1 + 16 * 0]
		movdqa xmm2, [ri_v1 + 16 * 1]
		pcmpeqb xmm1, xmm0
		pcmpeqb xmm2, xmm0
		pmovmskb rd_uni, xmm1
		pmovmskb rd_temp2, xmm2
		or rd_temp2, rd_uni
		jz .simd_loop
	.simd_ldone:
		test rd_uni, rd_uni
		jnz .found_b1
		.found_b2:
			rep bsf rd_temp2, rd_temp2
			movzx rq_temp2, rd_temp2
			lea rq_uni, [ri_v1 + rq_temp2 + 16]
			sub rq_uni, rq_temp1
			ret
		.found_b1:
			rep bsf rd_uni, rd_uni
			movzx rq_uni, rd_uni
			add rq_uni, ri_v1
			sub rq_uni, rq_temp1
			ret
	.end:
	
; ri_v1 = str
; rq_uni = return len
SUS_PUBLIC_FUNC strlen_avx2
	.begin:
		mov rq_temp1, ri_v1
		vpxor ymm0, ymm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 32 - 1
		jz .align_done
		vpcmpeqb ymm1, ymm0, [ri_v1]
		vpmovmskb rd_uni, ymm1
		test rd_uni, rd_uni
		jnz .found_b1
		add ri_v1, ri_v4
	.align_done:
	.simd_lbegin:
		jmp .simd_lbegin_entry
	.simd_loop:
		add ri_v1, 32 * 2
	.simd_lbegin_entry:
		vpcmpeqb ymm1, ymm0, [ri_v1 + 32 * 0]
		vpcmpeqb ymm2, ymm0, [ri_v1 + 32 * 1]
		vpmovmskb rd_uni, ymm1
		vpmovmskb rd_temp2, ymm2
		or rd_temp2, rd_uni
		jz .simd_loop
	.simd_ldone:
		test rd_uni, rd_uni
		jnz .found_b1
		.found_b2:
			vzeroupper
			rep bsf rd_temp2, rd_temp2
			movzx rq_temp2, rd_temp2
			lea rq_uni, [ri_v1 + rq_temp2 + 32]
			sub rq_uni, rq_temp1
			ret
		.found_b1:
			vzeroupper
			rep bsf rd_uni, rd_uni
			movzx rq_uni, rd_uni
			add rq_uni, ri_v1
			sub rq_uni, rq_temp1
			ret
	.end:

; ---------------------------------------------------------------------

; ri_v1 = wcs
; rq_uni = return len
SUS_PUBLIC_FUNC strlen16_sse2
	.begin:
		mov rq_temp1, ri_v1
		pxor xmm0, xmm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 16 - 1
		jz .align_done
		movdqu xmm1, [ri_v1]
		pcmpeqw xmm1, xmm0
		pmovmskb rd_uni, xmm1
		test rd_uni, rd_uni
		jnz .found_b1
		add ri_v1, ri_v4
	.align_done:
	.simd_lbegin:
		jmp .simd_lbegin_entry
	.simd_loop:
		add ri_v1, 16 * 2
	.simd_lbegin_entry:
		movdqa xmm1, [ri_v1 + 16 * 0]
		movdqa xmm2, [ri_v1 + 16 * 1]
		pcmpeqw xmm1, xmm0
		pcmpeqw xmm2, xmm0
		pmovmskb rd_uni, xmm1
		pmovmskb rd_temp2, xmm2
		or rd_temp2, rd_uni
		jz .simd_loop
	.simd_ldone:
		test rd_uni, rd_uni
		jnz .found_b1
		.found_b2:
			rep bsf rd_uni, rd_temp2
			movzx rq_uni, rd_uni
			lea rq_uni, [ri_v1 + rq_uni + 16]
			sub rq_uni, rq_temp1
			shr rq_uni, 1
			ret
		.found_b1:
			rep bsf rd_uni, rd_uni
			movzx rq_uni, rd_uni
			add rq_uni, ri_v1
			sub rq_uni, rq_temp1
			shr rq_uni, 1
			ret
	.end:
	
; ri_v1 = wcs
; rq_uni = return len
SUS_PUBLIC_FUNC strlen16_avx2
	.begin:
		mov rq_temp1, ri_v1
		vpxor ymm0, ymm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 32 - 1
		jz .align_done
		vpcmpeqw ymm1, ymm0, [ri_v1]
		vpmovmskb rd_uni, ymm1
		test rd_uni, rd_uni
		jnz .found_b1
		add ri_v1, ri_v4
	.align_done:
	.simd_lbegin:
		jmp .simd_lbegin_entry
	.simd_loop:
		add ri_v1, 32 * 2
	.simd_lbegin_entry:
		vpcmpeqw ymm1, ymm0, [ri_v1 + 32 * 0]
		vpcmpeqw ymm2, ymm0, [ri_v1 + 32 * 1]
		vpmovmskb rd_uni, ymm1
		vpmovmskb rd_temp2, ymm2
		or rd_temp2, rd_uni
		jz .simd_loop
	.simd_ldone:
		test rd_uni, rd_uni
		jnz .found_b1
		.found_b2:
			vzeroupper
			rep bsf rd_uni, rd_temp2
			movzx rq_uni, rd_uni
			lea rq_uni, [ri_v1 + rq_uni + 32]
			sub rq_uni, rq_temp1
			shr rq_uni, 1
			ret
		.found_b1:
			vzeroupper
			rep bsf rd_uni, rd_uni
			movzx rq_uni, rd_uni
			add rq_uni, ri_v1
			sub rq_uni, rq_temp1
			shr rq_uni, 1
			ret
	.end:

; ---------------------------------------------------------------------

; ri_v1 = wcs
; rq_uni = return len
SUS_PUBLIC_FUNC strlen32_sse2
	.begin:
		mov rq_temp1, ri_v1
		pxor xmm0, xmm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 16 - 1
		jz .align_done
		movdqu xmm1, [ri_v1]
		pcmpeqd xmm1, xmm0
		pmovmskb rd_uni, xmm1
		test rd_uni, rd_uni
		jnz .found_b1
		add ri_v1, ri_v4
	.align_done:
	.simd_lbegin:
	.simd_loop:
		movdqa xmm1, [ri_v1 + 16 * 0]
		movdqa xmm2, [ri_v1 + 16 * 1]
		pcmpeqd xmm1, xmm0
		pcmpeqd xmm2, xmm0
		pmovmskb rd_uni, xmm1
		pmovmskb rd_temp2, xmm2
		add ri_v1, 16 * 2
		or rd_temp2, rd_uni
		jz .simd_loop
	.simd_ldone:
		test rd_uni, rd_uni
		jnz .found_b1
		.found_b2:
			rep bsf rd_uni, rd_temp2
			movzx rq_uni, rd_uni
			lea rq_uni, [ri_v1 + rq_uni - 16 * 1]
			sub rq_uni, rq_temp1
			shr rq_uni, 2
			ret
		.found_b1:
			rep bsf rd_uni, rd_uni
			movzx rq_uni, rd_uni
			add rq_uni, [ri_v1 + rq_uni - 16 * 2]
			sub rq_uni, rq_temp1
			shr rq_uni, 2
			ret
	.end:
	
; ri_v1 = str32
; rq_uni = return len
SUS_PUBLIC_FUNC strlen32_avx2
	.begin:
		mov rq_temp1, ri_v1
		vpxor ymm0, ymm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 32 - 1
		jz .align_done
		vpcmpeqd ymm1, ymm0, [ri_v1]
		vpmovmskb rd_uni, ymm1
		test rd_uni, rd_uni
		jnz .found_b1
		add ri_v1, ri_v4
	.align_done:
	.simd_lbegin:
	.simd_loop:
		vpcmpeqd ymm1, ymm0, [ri_v1 + 32 * 0]
		vpcmpeqd ymm2, ymm0, [ri_v1 + 32 * 1]
		vpmovmskb rd_uni, ymm1
		vpmovmskb rd_temp2, ymm2
		add ri_v1, 32 * 2
		or rd_temp2, rd_uni
		jz .simd_loop
	.simd_ldone:
		test rd_uni, rd_uni
		jnz .found_b1
		.found_b2:
			vzeroupper
			rep bsf rd_uni, rd_temp2
			movzx rq_uni, rd_uni
			lea rq_uni, [ri_v1 + rq_uni - 32 * 1]
			sub rq_uni, rq_temp1
			shr rq_uni, 2
			ret
		.found_b1:
			vzeroupper
			rep bsf rd_uni, rd_uni
			movzx rq_uni, rd_uni
			lea rq_uni, [ri_v1 + rq_uni - 32 * 2]
			sub rq_uni, rq_temp1
			shr rq_uni, 2
			ret
	.end:

; ---------------------------------------------------------------------

; ri_v1 = dest
; ri_v2 = src
; rq_uni = return dest
SUS_PUBLIC_FUNC strcpy_sse2
	.begin:
		mov rq_uni, ri_v1
		pxor xmm0, xmm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 16 - 1
		jz .align_done
		movdqu xmm1, [ri_v2]
		movdqu xmm2, xmm1
		pcmpeqb xmm2, xmm0
		pmovmskb rd_temp1, xmm2
		test rd_temp1, rd_temp1
		jnz .found_b1
		movdqu [ri_v1], xmm1
		add ri_v1, ri_v4
		add ri_v2, ri_v4
	.align_done:
	.simd_lbegin:
		jmp .simd_lbegin_entry
	.simd_loop:
		movdqu [ri_v1 + 16 * 0], xmm1
		movdqu [ri_v1 + 16 * 1], xmm2
		add ri_v1, 16 * 2
		add ri_v2, 16 * 2
	.simd_lbegin_entry:
		movdqu xmm1, [ri_v2 + 16 * 0]
		movdqu xmm2, [ri_v2 + 16 * 1]
		movdqu xmm3, xmm1
		movdqu xmm4, xmm2
		pcmpeqb xmm3, xmm0
		pcmpeqb xmm4, xmm0
		pmovmskb rd_temp1, xmm3
		pmovmskb rd_temp2, xmm4
		or rd_temp2, rd_temp1
		jz .simd_loop
	.simd_ldone:
		test rd_temp1, rd_temp1
		jnz .found_b1
		.found_b2:
			movdqu [ri_v1], xmm1
			rep bsf ri_v3d, rd_temp2
			movdqu xmm1, xmm2
			add ri_v1, 16
			add ri_v2, 16
			jmp .remains
		.found_b1:
			rep bsf ri_v3d, rd_temp1
		.remains:
			test ri_v3d, ri_v3d
			jz .remains_done
			cmp ri_v3d, 8
			jb .qword_block_done
		.qword_block:
			movq rq_temp1, xmm1
			mov [ri_v1], rq_temp1
			add ri_v1, 8
			add ri_v2, 8
			sub ri_v3d, 8
			jz .remains_done
		.qword_block_done:
		.byte_lbegin:
		.byte_loop:
			mov rb_temp1, [ri_v2]
			mov [ri_v1], rb_temp1
			inc ri_v1
			inc ri_v2
			dec ri_v3d
			jnz .byte_loop
		.byte_ldone:
		.remains_done:
			mov byte [ri_v1], 0
			ret
	.end:
	
; ri_v1 = dest
; ri_v2 = src
; rq_uni = return dest
SUS_PUBLIC_FUNC strcpy_avx2
	.begin:
		mov rq_uni, ri_v1
		vpxor ymm0, ymm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 32 - 1
		jz .align_done
		vmovdqu ymm1, [ri_v2]
		vpcmpeqb ymm2, ymm0, ymm1
		vpmovmskb rd_temp1, ymm2
		test rd_temp1, rd_temp1
		jnz .found_b1
		vmovdqu [ri_v1], ymm1
		add ri_v1, ri_v4
		add ri_v2, ri_v4
	.align_done:
	.simd_lbegin:
		jmp .simd_lbegin_entry
	.simd_loop:
		vmovdqu [ri_v1 + 32 * 0], ymm1
		vmovdqu [ri_v1 + 32 * 1], ymm2
		add ri_v1, 32 * 2
		add ri_v2, 32 * 2
	.simd_lbegin_entry:
		vmovdqu ymm1, [ri_v2 + 32 * 0]
		vmovdqu ymm2, [ri_v2 + 32 * 1]
		vpcmpeqb ymm3, ymm0, ymm1
		vpcmpeqb ymm4, ymm0, ymm2
		vpmovmskb rd_temp1, ymm3
		vpmovmskb rd_temp2, ymm4
		or rd_temp2, rd_temp1
		jz .simd_loop
	.simd_ldone:
		test rd_temp1, rd_temp1
		jnz .found_b1
		.found_b2:
			vmovdqu [ri_v1], ymm1
			rep bsf ri_v3d, rd_temp2
			add ri_v1, 32
			add ri_v2, 32
			jmp .remains
		.found_b1:
			rep bsf ri_v3d, rd_temp1
		.remains:
			test ri_v3d, ri_v3d
			jz .remains_done
		.qword_lbegin:
			mov ri_v4d, ri_v3d
			shr ri_v4d, 3
			jz .qword_ldone
		.qword_loop:
			mov rq_temp1, [ri_v2]
			mov [ri_v1], rq_temp1
			add ri_v1, 8
			add ri_v2, 8
			dec ri_v4d
			jnz .qword_loop
			and ri_v3d, 8 - 1
			jz .remains_done
		.qword_ldone:
		.byte_lbegin:
		.byte_loop:
			mov rb_temp1, [ri_v2]
			mov [ri_v1], rb_temp1
			inc ri_v1
			inc ri_v2
			dec ri_v3d
			jnz .byte_loop
		.byte_ldone:
		.remains_done:
			vzeroupper
			mov byte [ri_v1], 0
			ret
	.end:

; ---------------------------------------------------------------------

; ri_v1 = dest
; ri_v2 = src
; rq_uni = return dest
SUS_PUBLIC_FUNC strcpy16_sse2
	.begin:
		mov rq_uni, ri_v1
		pxor xmm0, xmm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 16 - 1
		jz .align_done
		movdqu xmm1, [ri_v2]
		movdqu xmm2, xmm1
		pcmpeqw xmm2, xmm0
		pmovmskb rd_temp1, xmm2
		test rd_temp1, rd_temp1
		jnz .found_b1
		movdqu [ri_v1], xmm1
		add ri_v1, ri_v4
		add ri_v2, ri_v4
	.align_done:
	.simd_lbegin:
		jmp .simd_lbegin_entry
	.simd_loop:
		movdqu [ri_v1 + 16 * 0], xmm1
		movdqu [ri_v1 + 16 * 1], xmm2
		add ri_v1, 16 * 2
		add ri_v2, 16 * 2
	.simd_lbegin_entry:
		movdqu xmm1, [ri_v2 + 16 * 0]
		movdqu xmm2, [ri_v2 + 16 * 1]
		movdqu xmm3, xmm1
		movdqu xmm4, xmm2
		pcmpeqw xmm3, xmm0
		pcmpeqw xmm4, xmm0
		pmovmskb rd_temp1, xmm3
		pmovmskb rd_temp2, xmm4
		or rd_temp2, rd_temp1
		jz .simd_loop
	.simd_ldone:
		test rd_temp1, rd_temp1
		jnz .found_b1
		.found_b2:
			movdqu [ri_v1], xmm1
			rep bsf ri_v3d, rd_temp2
			movdqu xmm1, xmm2
			add ri_v1, 16
			add ri_v2, 16
			jmp .remains
		.found_b1:
			rep bsf ri_v3d, rd_temp1
		.remains:
			test ri_v3d, ri_v3d
			jz .remains_done
			cmp ri_v3d, 8
			jb .qword_block_done
		.qword_block:
			movq rq_temp1, xmm1
			mov [ri_v1], rq_temp1
			add ri_v1, 8
			add ri_v2, 8
			sub ri_v3d, 8
			jz .remains_done
		.qword_block_done:
			shr ri_v3d, 1
		.byte_lbegin:
		.byte_loop:
			mov rw_temp1, [ri_v2]
			mov [ri_v1], rw_temp1
			add ri_v1, 2
			add ri_v2, 2
			dec ri_v3d
			jnz .byte_loop
		.byte_ldone:
		.remains_done:
			mov word [ri_v1], 0
			ret
	.end:
	
; ri_v1 = dest
; ri_v2 = src
; rq_uni = return dest
SUS_PUBLIC_FUNC strcpy16_avx2
	.begin:
		mov rq_uni, ri_v1
		vpxor ymm0, ymm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 32 - 1
		jz .align_done
		vmovdqu ymm1, [ri_v2]
		vpcmpeqw ymm2, ymm0, ymm1
		vpmovmskb rd_temp1, ymm2
		test rd_temp1, rd_temp1
		jnz .found_b1
		vmovdqu [ri_v1], ymm1
		add ri_v1, ri_v4
		add ri_v2, ri_v4
	.align_done:
	.simd_lbegin:
		jmp .simd_lbegin_entry
	.simd_loop:
		vmovdqu [ri_v1 + 32 * 0], ymm1
		vmovdqu [ri_v1 + 32 * 1], ymm2
		add ri_v1, 32 * 2
		add ri_v2, 32 * 2
	.simd_lbegin_entry:
		vmovdqu ymm1, [ri_v2 + 32 * 0]
		vmovdqu ymm2, [ri_v2 + 32 * 1]
		vpcmpeqw ymm3, ymm0, ymm1
		vpcmpeqw ymm4, ymm0, ymm2
		vpmovmskb rd_temp1, ymm3
		vpmovmskb rd_temp2, ymm4
		or rd_temp2, rd_temp1
		jz .simd_loop
	.simd_ldone:
		test rd_temp1, rd_temp1
		jnz .found_b1
		.found_b2:
			vmovdqu [ri_v1], ymm1
			rep bsf ri_v3d, rd_temp2
			add ri_v1, 32
			add ri_v2, 32
			jmp .remains
		.found_b1:
			rep bsf ri_v3d, rd_temp1
		.remains:
			test ri_v3d, ri_v3d
			jz .remains_done
		.qword_lbegin:
			mov ri_v4d, ri_v3d
			shr ri_v4d, 3
			jz .qword_ldone
		.qword_loop:
			mov rq_temp1, [ri_v2]
			mov [ri_v1], rq_temp1
			add ri_v1, 8
			add ri_v2, 8
			dec ri_v4d
			jnz .qword_loop
			and ri_v3d, 8 / 2 - 1
			jz .remains_done
		.qword_ldone:
		.word_lbegin:
		.word_loop:
			mov rw_temp1, [ri_v2]
			mov [ri_v1], rw_temp1
			add ri_v1, 2
			add ri_v2, 2
			dec ri_v3d
			jnz .word_loop
		.word_ldone:
		.remains_done:
			vzeroupper
			mov word [ri_v1], 0
			ret
	.end:

; ---------------------------------------------------------------------

; ri_v1 = dest
; ri_v2 = src
; rq_uni = return dest
SUS_PUBLIC_FUNC strcpy32_sse2
	.begin:
		mov rq_uni, ri_v1
		pxor xmm0, xmm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 16 - 1
		jz .align_done
		movdqu xmm1, [ri_v2]
		movdqu xmm2, xmm1
		pcmpeqd xmm2, xmm0
		pmovmskb rd_temp1, xmm2
		test rd_temp1, rd_temp1
		jnz .found_b1
		movdqu [ri_v1], xmm1
		add ri_v1, ri_v4
		add ri_v2, ri_v4
	.align_done:
	.simd_lbegin:
		jmp .simd_lbegin_entry
	.simd_loop:
		movdqu [ri_v1 + 16 * 0], xmm1
		movdqu [ri_v1 + 16 * 1], xmm2
		add ri_v1, 16 * 2
		add ri_v2, 16 * 2
	.simd_lbegin_entry:
		movdqu xmm1, [ri_v2 + 16 * 0]
		movdqu xmm2, [ri_v2 + 16 * 1]
		movdqu xmm3, xmm1
		movdqu xmm4, xmm2
		pcmpeqd xmm3, xmm0
		pcmpeqd xmm4, xmm0
		pmovmskb rd_temp1, xmm3
		pmovmskb rd_temp2, xmm4
		or rd_temp2, rd_temp1
		jz .simd_loop
	.simd_ldone:
		test rd_temp1, rd_temp1
		jnz .found_b1
		.found_b2:
			movdqu [ri_v1], xmm1
			rep bsf ri_v3d, rd_temp2
			movdqu xmm1, xmm2
			add ri_v1, 16
			add ri_v2, 16
			jmp .remains
		.found_b1:
			rep bsf ri_v3d, rd_temp1
		.remains:
			test ri_v3d, ri_v3d
			jz .remains_done
			cmp ri_v3d, 8
			jb .qword_block_done
		.qword_block:
			movq rq_temp1, xmm1
			mov [ri_v1], rq_temp1
			add ri_v1, 8
			add ri_v2, 8
			sub ri_v3d, 8
			jz .remains_done
		.qword_block_done:
			shr ri_v3d, 2
		.word_lbegin:
		.word_loop:
			mov rd_temp1, [ri_v2]
			mov [ri_v1], rd_temp1
			add ri_v1, 4
			add ri_v2, 4
			dec ri_v3d
			jnz .word_loop
		.word_ldone:
		.remains_done:
			mov dword [ri_v1], 0
			ret
	.end:
	
; ri_v1 = dest
; ri_v2 = src
; rq_uni = return dest
SUS_PUBLIC_FUNC strcpy32_avx2
	.begin:
		mov rq_uni, ri_v1
		vpxor ymm0, ymm0
	.align:
		mov ri_v4, ri_v1
		neg ri_v4
		and ri_v4, 32 - 1
		jz .align_done
		vmovdqu ymm1, [ri_v2]
		vpcmpeqd ymm2, ymm0, ymm1
		vpmovmskb rd_temp1, ymm2
		test rd_temp1, rd_temp1
		jnz .found_b1
		vmovdqu [ri_v1], ymm1
		add ri_v1, ri_v4
		add ri_v2, ri_v4
	.align_done:
	.simd_lbegin:
		jmp .simd_lbegin_entry
	.simd_loop:
		vmovdqu [ri_v1 + 32 * 0], ymm1
		vmovdqu [ri_v1 + 32 * 1], ymm2
		add ri_v1, 32 * 2
		add ri_v2, 32 * 2
	.simd_lbegin_entry:
		vmovdqu ymm1, [ri_v2 + 32 * 0]
		vmovdqu ymm2, [ri_v2 + 32 * 1]
		vpcmpeqd ymm3, ymm0, ymm1
		vpcmpeqd ymm4, ymm0, ymm2
		vpmovmskb rd_temp1, ymm3
		vpmovmskb rd_temp2, ymm4
		or rd_temp2, rd_temp1
		jz .simd_loop
	.simd_ldone:
		test rd_temp1, rd_temp1
		jnz .found_b1
		.found_b2:
			vmovdqu [ri_v1], ymm1
			rep bsf ri_v3d, rd_temp2
			add ri_v1, 32
			add ri_v2, 32
			jmp .remains
		.found_b1:
			rep bsf ri_v3d, rd_temp1
		.remains:
			test ri_v3d, ri_v3d
			jz .remains_done
		.qword_lbegin:
			mov ri_v4d, ri_v3d
			shr ri_v4d, 3
			jz .qword_ldone
		.qword_loop:
			mov rq_temp1, [ri_v2]
			mov [ri_v1], rq_temp1
			add ri_v1, 8
			add ri_v2, 8
			dec ri_v4d
			jnz .qword_loop
			and ri_v3d, 8 / 4 - 1
			jz .remains_done
		.qword_ldone:
		.word_lbegin:
		.word_loop:
			mov rd_temp1, [ri_v2]
			mov [ri_v1], rd_temp1
			add ri_v1, 4
			add ri_v2, 4
			dec ri_v3d
			jnz .word_loop
		.word_ldone:
		.remains_done:
			vzeroupper
			mov dword [ri_v1], 0
			ret
	.end:

; ---------------------------------------------------------------------
