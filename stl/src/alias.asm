; weak symbol aliases

; Copyright (c) Microsoft Corporation.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

; Note that while this is a masm file, it contains no code
; masm does generate a .text section with zero size

mangle MACRO name, sz
    IFDEF RAX
        EXITM <name>
    ELSE
        EXITM <_&name&@&sz&>
    ENDIF
ENDM

imp_name MACRO name, sz
    EXITM @CatStr(__imp_, mangle(name, sz))
ENDM

create_alias MACRO oldname, newname, size
    EXTERN imp_name(newname, size) : PROC
    ALIAS imp_name(oldname, size)=imp_name(newname,size)
ENDM

create_alias __std_init_once_begin_initialize, InitOnceBeginInitialize, 16
create_alias __std_init_once_complete, InitOnceComplete, 12

END
