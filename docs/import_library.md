Copyright (c) Microsoft Corporation.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception


Import library is a .lib file that defines its symbols as imported from a DLL.

Usually there is one .lib file for one .dll file, with the same name,
and import library only contains references to DLL symbols, and doesn't define anything on its own.

The STL import library is called msvcprt.lib for release, msvcprtd.lib for debug,
and it links to corresponding msvcp140.dll and msvcp140d.dll DLLs.

However, the STL import library defines some functions and variables on its own.
This allows to:
 * Extend the STL implementation without altering the DLL export surface;
 * Efficiently and conveniently export variables and constants.

The caveats of this technique are:
 * Sort of defeat the purpose of /MD or /MDd options by making part of the STL implementation
   embedded into the resulting binaries, rather than stay in the STL DLL;
 * Due to the duplication in each binary that links to the import library,
   variables in the import library cannot represent a global state;
 * Due to having just two flavors of the import library (debug and release),
   cannot use anything that depends on _CONTAINER_DEBUG_LEVEL or _ITERATOR_DEBUG_LEVEL value.

For these reasons, especially the last one, there should be a strict control
of what is used by the import library. In particular, basic_string must not be there.
