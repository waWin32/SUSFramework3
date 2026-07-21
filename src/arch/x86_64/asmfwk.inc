; susfwk-arch-asmfwk.inc
;
; =====------------------- asmfwk.asm - a universal wrapper over the ABI -------------------=====
;
; Part of the SUSFramework3 Project, under the MIT License
; See LICENSE.txt for license information.
; SPDX-License-Identifier: MIT
;
; =====-------------------------------------------------------------------------------------=====

%ifndef _SUSFWK_ARCH_ASMFWK_INC_
%define _SUSFWK_ARCH_ASMFWK_INC_

; ---------------------------------------------------------------------

%ifidn __OUTPUT_FORMAT__, win64
	%define	SUS_ARCH_SYSTEM windows
	%define SUS_ARCH_SYS_WINDOWS 1
	%define SUS_ARCH_SYS_LINUX 0
	%define SUS_ARCH_SYS_MACOS 0
	%define SUS_ARCH_SYS_UNIX 0
	%define SUS_ARCH_SYS_BARE 0
%elifidn __OUTPUT_FORMAT__, elf64
	%define	SUS_ARCH_SYSTEM linux
	%define SUS_ARCH_SYS_WINDOWS 0
	%define SUS_ARCH_SYS_LINUX 1
	%define SUS_ARCH_SYS_MACOS 0
	%define SUS_ARCH_SYS_UNIX 1
	%define SUS_ARCH_SYS_BARE 0
%elifidn __OUTPUT_FORMAT__, macho64
	%define	SUS_ARCH_SYSTEM macos
	%define SUS_ARCH_SYS_WINDOWS 0
	%define SUS_ARCH_SYS_LINUX 0
	%define SUS_ARCH_SYS_MACOS 1
	%define SUS_ARCH_SYS_UNIX 1
	%define SUS_ARCH_SYS_BARE 0
%else
	%define	SUS_ARCH_SYSTEM bare
	%define SUS_ARCH_SYS_WINDOWS 0
	%define SUS_ARCH_SYS_LINUX 0
	%define SUS_ARCH_SYS_MACOS 0
	%define SUS_ARCH_SYS_UNIX 1
	%define SUS_ARCH_SYS_BARE 1
%endif
	

%if SUS_ARCH_SYS_WINDOWS
	%define SUS_ASM_PFX(name) name
%elif SUS_ARCH_SYS_LINUX
	%define SUS_ASM_PFX(name) name
%elif SUS_ARCH_SYS_MACOS
	%define SUS_ASM_PFX(name) _%+name
#else 
	%define SUS_ASM_PFX(name) name
%endif

%macro SUS_PRIVATE_FUNC 1
	SUS_ASM_PFX(%1):
%endmacro
%macro SUS_PUBLIC_FUNC 1
	global SUS_ASM_PFX(%1)
	SUS_ASM_PFX(%1):
%endmacro

%define rq_uni rax
%define rd_uni eax
%define rw_uni ax
%define rb_uni al
%define rh_uni ah

%define rq_temp1 r10
%define rd_temp1 r10d
%define rw_temp1 r10w
%define rb_temp1 r10b
%define rh_temp1 r10h

%define rq_temp2 r11
%define rd_temp2 r11d
%define rw_temp2 r11w
%define rb_temp2 r11b
%define rh_temp2 r11h

%define rq_save1 rbx
%define rd_save1 ebx
%define rw_save1 bx
%define rb_save1 bl

%define rq_save2 r12
%define rd_save2 r12d
%define rw_save2 r12w
%define rb_save2 r12b

%define rq_save3 r13
%define rd_save3 r13d
%define rw_save3 r13w
%define rb_save3 r13b

%define rq_save4 r14
%define rd_save4 r14d
%define rw_save4 r14w
%define rb_save4 r14b

%define rq_save5 r15
%define rd_save5 r15d
%define rw_save5 r15w
%define rb_save5 r15b

%if SUS_ARCH_SYS_WINDOWS
	%define ri_v1 rcx
	%define ri_v1b cl
	%define ri_v1w cx
	%define ri_v1d ecx

	%define ri_v2 rdx
	%define ri_v2b dl
	%define ri_v2w dx
	%define ri_v2d edx

	%define ri_v3 r8
	%define ri_v3b r8b
	%define ri_v3w r8w
	%define ri_v3d r8d

	%define ri_v4 r9
	%define ri_v4b r9b
	%define ri_v4w r9w
	%define ri_v4d r9d
%else
	%define ri_v1 rdi
	%define ri_v1b dil
	%define ri_v1w di
	%define ri_v1d edi

	%define ri_v2 rsi
	%define ri_v2b sil
	%define ri_v2w si
	%define ri_v2d esi

	%define ri_v3 rdx
	%define ri_v3b dl
	%define ri_v3w dx
	%define ri_v3d edx

	%define ri_v4 rcx
	%define ri_v4b cl
	%define ri_v4w cx
	%define ri_v4d ecx
%endif

; ---------------------------------------------------------------------

%endif ; !_SUSFWK_ARCH_ASMFWK_INC_