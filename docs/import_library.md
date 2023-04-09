<!-- Copyright (c) Microsoft Corporation. -->
<!-- SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception -->

# Filenames

Mode    | Import Library | DLL (VS)        | DLL (GitHub)        |
--------|----------------|-----------------|---------------------|
Release | `msvcprt.lib`  | `msvcp140.dll`  | `msvcp140_oss.dll`  |
Debug   | `msvcprtd.lib` | `msvcp140d.dll` | `msvcp140d_oss.dll` |

# Import Libraries

An import library is a `.lib` file that defines its symbols as imported from a DLL.

Usually there is one `.lib` file for one `.dll` file, with the same name.
The names are different for MSVC because it started encoding its ABI version into the DLL's filename,
but there was no reason to change the import library's filename.

Also, an import library usually only contains references to DLL symbols and doesn't define anything on its own.
However, this is purely a convention - nothing technically stops an import library
from containing object files that are effectively statically linked.

## Advantages of Injecting Additional Code

This is what the STL's import library does - it defines some functions and variables on its own.
This allows us to:

* Extend the STL implementation without altering the DLL export surface.
  + This has been critical in allowing us to implement C++17 `<filesystem>` and much more.
* Separately compile functions and constant data for improved throughput.
  + `<charconv>`'s lookup tables are a notable example.

## Limitations

The caveats of this technique are:

* It effectively defeats the purpose of the `/MD` and `/MDd` options by embedding part of
  the STL implementation into the resulting user binaries, rather than staying in the STL's DLL.
* Due to the duplication in each user binary that links to the import library,
  variables in the import library **cannot represent shared global state**.
  + This limitation is subtle (not readily apparent from the source code) and critical.
    If shared global state is necessary, our only option while preserving bincompat is adding a satellite DLL.
* Due to having just two flavors of the import library (debug and release),
  we cannot use anything that depends on `_CONTAINER_DEBUG_LEVEL` or `_ITERATOR_DEBUG_LEVEL`.

For these reasons, especially the last one, we need to strictly control what is used by the import library.
In particular, `basic_string` must not be used there.

## Core Headers

Restricting the import library to including core headers only is an effective way to avoid problems.
`locale0.cpp`'s inclusion of `<xfacet>` is currently a special case and should be treated with extreme caution.
