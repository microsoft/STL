@echo off
clang-format -style=file -i stl/inc/** stl/src/** 2>&1
echo If your build fails here, the following files were changed by clang-format.
git status --porcelain 1>&2
