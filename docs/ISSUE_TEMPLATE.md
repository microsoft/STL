---
name: Bug Report
about: Report a bug in the STL
title: "<header>: Problem"
labels: ''
assignees: ''

---

**Describe the bug**
A clear and concise description of what the bug is. Please check that you've
read the guidelines for submitting STL bug reports in `README.md`. If you are
having problems with any component that is not the STL, instructions to get
to the right place are there.

**Command-line test case**
STL version (git commit or Visual Studio version):

```
C:\Users\billy\Desktop>type repro.cpp
#include <iostream>

int main() {
    // Replace this program with one demonstrating your actual bug report,
    // along with the following compilation command. Please leave compiler
    // version banners in the output (don't use /nologo), and include output of
    // your test program, if any.
    std::cout << "test failure\n";
}

C:\Users\billy\Desktop>cl /EHsc /W4 /WX .\repro.cpp
Microsoft (R) C/C++ Optimizing Compiler Version 19.23.28019.1 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

repro.cpp
Microsoft (R) Incremental Linker Version 14.23.28019.1
Copyright (C) Microsoft Corporation.  All rights reserved.

/out:repro.exe
repro.obj

C:\Users\billy\Desktop>.\repro.exe
test failure
```

**Expected behavior**
A clear and concise description of what you expected to happen. Alternately,
include `static_assert`s or `assert`s in your test case above whose failure clearly indicates the problem.

**Additional context**
Add any other context about the problem here.
