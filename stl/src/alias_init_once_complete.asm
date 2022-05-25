; Copyright (c) Microsoft Corporation.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

; weak symbol aliases

; This checks if we're in x64 mode (using ml64).
; Note that RAX isn't really an assembler symbol,
; and this check only works in the IFDEF direction.
; That is:
; IFDEF RAX
;   meow
; ENDIF
; will expand to meow on only x64 systems, however:
; IFNDEF RAX
;   meow
; ENDIF
; will expand to meow on _BOTH_ x64 and x86 systems.
IFDEF RAX
    EXTERN __imp_InitOnceComplete : PROC
    ALIAS <__imp___std_init_once_complete> = <__imp_InitOnceComplete>
ELSE
    EXTERN __imp__InitOnceComplete@12 : PROC
    ALIAS <__imp____std_init_once_complete@12> = <__imp__InitOnceComplete@12>
ENDIF

END
