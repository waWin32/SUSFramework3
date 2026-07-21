; susfwk-arch-global_init.asm
;
; =====------------------- global_init.asm - calling constructors and destructors from the loader -------------------=====
;
; Part of the SUSFramework3 Project, under the MIT License
; See LICENSE.txt for license information.
; SPDX-License-Identifier: MIT
;
; =====--------------------------------------------------------------------------------------------------------------=====

%include "x86_64/asmfwk.inc"

default rel

; ---------------------------------------------------------------------

%if SUS_ARCH_SYS_WINDOWS

    ; ---------------------------------------------------------------------

    section .text
    SUS_PUBLIC_FUNC __chkstk
        ret
    SUS_PUBLIC_FUNC __dyn_tls_init
        ret

    ; ---------------------------------------------------------------------

    ; Constructors of global classes
    section .CRT$RGC0 rdata align=8
    global __global_init_a
    __global_init_a:  dq  0
    section .CRT$RGCzzz rdata align=8
    global __global_init_z
    __global_init_z:  dq  0
    ; Destructors of global classes 
    section .CRT$RGD0 rdata align=8
    global __global_cleanup_a
    __global_cleanup_a:  dq  0
    section .CRT$RGDzzz rdata align=8
    global __global_cleanup_z
    __global_cleanup_z:  dq  0

    ; Constructors of TLS objects
    section .CRT$RTC0 rdata align=8
    global __tls_init_a
    __tls_init_a:  dq  0
    section .CRT$RTCzzz rdata align=8
    global __tls_init_z
    __tls_init_z:  dq  0
    ; Destructors of TLS objects
    section .CRT$RTD0 rdata align=8
    global __tls_cleanup_a
    __tls_cleanup_a:  dq  0
    section .CRT$RTDzzz rdata align=8
    global __tls_cleanup_z
    __tls_cleanup_z:  dq  0
    
    ; ---------------------------------------------------------------------

    section .tls$ data align=8
    global _tls_start
    _tls_start: dq 0
    section .tls$ZZZ data align=8
    global _tls_end
    _tls_end: dq 0
    section .rdata$T rdata align=8
    global _tls_callbacks
    _tls_callbacks:
        dq  SystemTlsCallback
        dq  0
    section .data data read write align=8
    global _tls_index
    _tls_index:
        dd  0
    section .rdata$zzz rdata align=8
    global _tls_used
    _tls_used:
        dq  _tls_start      ; StartAddressOfRawData
        dq  _tls_end        ; EndAddressOfRawData
        dq  _tls_index      ; AddressOfIndex
        dq  _tls_callbacks  ; AddressOfCallBacks
        dd  0               ; SizeOfZeroFill
        dd  0               ; Characteristics
        
    ; ---------------------------------------------------------------------

    section .text
    SUS_PUBLIC_FUNC __call_global_initializers
        push rq_save1
        sub rsp, 32 + 8
        lea rq_save1, [__global_init_a + 8]
        mov rq_uni, [rq_save1]
        test rq_uni, rq_uni
        jz .end
    .ctor_loop:
        call rq_uni
        add rq_save1, 8
        mov rq_uni, [rq_save1]
        test rq_uni, rq_uni
        jnz .ctor_loop
    .end:
        add rsp, 32 + 8
        pop rq_save1
        ret

    SUS_PUBLIC_FUNC __call_global_cleanup
        push rq_save1
        sub rsp, 32 + 8
        lea rq_save1, [__global_cleanup_z - 8]
        mov rq_uni, [rq_save1]
        test rq_uni, rq_uni
        jz .end
    .dtor_loop:
        call rq_uni
        sub rq_save1, 8
        mov rq_uni, [rq_save1]
        test rq_uni, rq_uni
        jnz .dtor_loop
    .end:
        add rsp, 32 + 8
        pop rq_save1
        ret
    
    SUS_PUBLIC_FUNC __call_tls_initializers
        push rq_save1
        sub rsp, 32 + 8
        lea rq_save1, [__tls_init_a + 8]
        mov rq_uni, [rq_save1]
        test rq_uni, rq_uni
        jz .end
    .ctor_loop:
        call rq_uni
        add rq_save1, 8
        mov rq_uni, [rq_save1]
        test rq_uni, rq_uni
        jnz .ctor_loop
    .end:
        add rsp, 32 + 8
        pop rq_save1
        ret

    SUS_PUBLIC_FUNC __call_tls_cleanup
        push rq_save1
        sub rsp, 32 + 8
        lea rq_save1, [__tls_cleanup_z - 8]
        mov rq_uni, [rq_save1]
        test rq_uni, rq_uni
        jz .end
    .dtor_loop:
        call rq_uni
        sub rq_save1, 8
        mov rq_uni, [rq_save1]
        test rq_uni, rq_uni
        jnz .dtor_loop
    .end:
        add rsp, 32 + 8
        pop rq_save1
        ret
        
    ; ---------------------------------------------------------------------

    SUS_PUBLIC_FUNC SystemTlsCallback
        push rq_save1
        mov rq_save1, rsp
        sub rsp, 32
        cmp ri_v2d, 1               ; DLL_PROCESS_ATTACH
        je .call_proc_ctors
        cmp ri_v2d, 2               ; DLL_THREAD_ATTACH
        je .call_tls_ctors
        cmp ri_v2d, 0               ; DLL_PROCESS_DETACH
        je .call_proc_dtors
        cmp ri_v2d, 3               ; DLL_THREAD_DETACH
        je .call_tls_dtors
    .done:
        add rsp, 32
        pop rq_save1
        ret
    .call_proc_ctors:
        call __call_global_initializers
        call __call_tls_initializers
        jmp .done
    .call_tls_ctors:
        call __call_tls_initializers
        jmp .done
    .call_proc_dtors:
        call __call_tls_cleanup
        call __call_global_cleanup
        jmp .done
    .call_tls_dtors:
        call __call_tls_cleanup
        jmp .done
        
    ; ---------------------------------------------------------------------

%else
    ; TODO: Linux & Macos
%endif

; ---------------------------------------------------------------------
