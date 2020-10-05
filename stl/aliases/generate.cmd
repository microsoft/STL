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
rmdir /s /q chpe
rmdir /s /q arm64ec

mkdir i386
mkdir amd64
mkdir arm
mkdir arm64
mkdir chpe
mkdir arm64ec

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
..\..\..\..\..\tools\amd64\aliasobj.exe ^
  /machine:arm64 ^
  __imp___std_init_once_begin_initialize ^
  __imp_InitOnceBeginInitialize ^
  arm64\std_init_once_begin_initialize.obj
..\..\..\..\..\tools\x86\aliasobj.exe ^
  __imp_#__std_init_once_begin_initialize@16 ^
  __imp_#InitOnceBeginInitialize@16 ^
  chpe\std_init_once_begin_initialize.obj
..\..\..\..\..\tools\amd64\aliasobj.exe ^
  /machine:arm64ec ^
  __imp___std_init_once_begin_initialize ^
  __imp_InitOnceBeginInitialize ^
  arm64ec\std_init_once_begin_initialize.obj

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
..\..\..\..\..\tools\amd64\aliasobj.exe ^
  /machine:arm64 ^
  __imp___std_init_once_complete ^
  __imp_InitOnceComplete ^
  arm64\std_init_once_complete.obj
..\..\..\..\..\tools\x86\aliasobj.exe ^
  __imp_#__std_init_once_complete@12 ^
  __imp_#InitOnceComplete@12 ^
  chpe\std_init_once_complete.obj
..\..\..\..\..\tools\amd64\aliasobj.exe ^
  /machine:arm64ec ^
  __imp___std_init_once_complete ^
  __imp_InitOnceComplete ^
  arm64ec\std_init_once_complete.obj
