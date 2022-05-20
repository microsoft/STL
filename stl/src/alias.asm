; weak symbol aliases

; Copyright (c) Microsoft Corporation.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

; Note that while this is a masm file, it contains no code
; masm does generate a .text section with zero size

IFDEF RAX ; note: this only works with ifdef, not ifndef
ELSE
.MODEL FLAT, STDCALL
ENDIF

EXTERN __imp_InitOnceBeginInitialize :PROTO :PTR PTR VOID, :DWORD, :PTR DWORD, :PTR PTR VOID
EXTERN __imp_InitOnceComplete :PROTO :PTR PTR VOID, :DWORD, :PTR VOID

IFDEF RAX
ALIAS <__imp___std_init_once_begin_initialize>=<__imp_InitOnceBeginInitialize>
ALIAS <__imp___std_init_once_complete>=<__imp_InitOnceComplete>
ELSE
ALIAS <__imp____std_init_once_begin_initialize@16>=<__imp_InitOnceBeginInitialize>
ALIAS <__imp____std_init_once_complete@12>=<__imp_InitOnceComplete>
ENDIF
END
