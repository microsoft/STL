# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import os

from stl.test.format import STLTestFormat, TestStep


class CustomTestFormat(STLTestFormat):
    def getBuildSteps(self, test, litConfig, shared):
        exeSource = test.getSourcePath()
        dllSource = os.path.join(os.path.dirname(exeSource), 'TestDll.cpp')

        outputDir, _ = test.getTempPaths()
        dllOutput = os.path.join(outputDir, 'TestDll.DLL')

        cmd = [test.cxx, dllSource, *test.flags, *test.compileFlags, '/Fe' + dllOutput,
               '/link', '/DLL', *test.linkFlags]

        yield TestStep(cmd, shared.execDir, shared.env, False)

        for step in super().getBuildSteps(test, litConfig, shared):
            yield step
