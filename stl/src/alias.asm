; Copyright (c) Microsoft Corporation.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

; weak symbol aliases

; Note that while this is a masm file, it contains no code.
; masm does generate a .text section with zero size.

; Mangle identifiers for stdcall calling convention.
; On x64 this does nothing, on x86 it adds a leading
; underscore and trailing @sz, where sz should be the size of the parameters.
mangle MACRO name, sz
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
        EXITM <name>
    ELSE
        EXITM <_&name&@&sz&>
    ENDIF
ENDM

; Generate the name of the imp symbol for a function.
imp_name MACRO name, sz
    EXITM @CatStr(__imp_, mangle(name, sz))
ENDM

create_alias MACRO oldname, newname, size
    ; Use "EXTERN name : PROC" instead of "EXTERN name : PROTO ..."
    ; to avoid masm adding underscores to the names of (object) symbols.
    ; Note that the MASM symbol always has the name that appears in
    ; the source file (after macro expansion), but when _using_ the symbol
    ; masm will add underscores.
    EXTERN imp_name(newname, size) : PROC
    ; The documentation for "ALIAS <alias> = <actual-name>" says angle brackets
    ; are required around the parameters, but this is not the case; it seems to just require
    ; a single "text item", which both angle brackets and this macro call create.
    ALIAS imp_name(oldname, size) = imp_name(newname, size)
ENDM

create_alias __std_init_once_begin_initialize, InitOnceBeginInitialize, 16
create_alias __std_init_once_complete, InitOnceComplete, 12

END
