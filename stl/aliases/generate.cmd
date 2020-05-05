:: Copyright (c) Microsoft Corporation.
:: SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
::
:: Generates aliases for Windows API functions called by headers.
:: TRANSITION, VSO-1116868 "'aliasobj.exe' should be available with Visual Studio"

cd %~dp0

rmdir /s /q i386
rmdir /s /q amd64
rmdir /s /q arm
rmdir /s /q arm64

mkdir i386
mkdir amd64
mkdir arm
mkdir arm64

:: __std_init_once_begin_initialize
..\..\..\..\..\tools\x86\aliasobj.exe ^
  __imp____std_init_once_begin_initialize@16 ^
  __imp__InitOnceBeginInitialize@16 ^
  i386\std_init_once_begin_initialize.obj
..\..\..\..\..\tools\amd64\aliasobj.exe ^
  __imp___std_init_once_begin_initialize ^
  __imp_InitOnceBeginInitialize ^
  amd64\std_init_once_begin_initialize.obj
..\..\..\..\..\tools\x86\aliasobj.exe ^
  __imp___std_init_once_begin_initialize ^
  __imp_InitOnceBeginInitialize ^
  arm\std_init_once_begin_initialize.obj
copy amd64\std_init_once_begin_initialize.obj arm64\std_init_once_begin_initialize.obj

:: __std_init_once_complete
..\..\..\..\..\tools\x86\aliasobj.exe ^
  __imp____std_init_once_complete@12 ^
  __imp__InitOnceComplete@12 ^
  i386\std_init_once_complete.obj
..\..\..\..\..\tools\amd64\aliasobj.exe ^
  __imp___std_init_once_complete ^
  __imp_InitOnceComplete ^
  amd64\std_init_once_complete.obj
..\..\..\..\..\tools\x86\aliasobj.exe ^
  __imp___std_init_once_complete ^
  __imp_InitOnceComplete ^
  arm\std_init_once_complete.obj
copy amd64\std_init_once_complete.obj arm64\std_init_once_complete.obj
