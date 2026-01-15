# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import os

from stl.test.format import STLTestFormat, TestStep
from stl.test.tests import TestType


class CustomTestFormat(STLTestFormat):
    def getBuildSteps(self, test, litConfig, shared):
        exeSourceDir = os.path.dirname(test.getSourcePath())
        _, outputBase = test.getTempPaths()

        sourceFiles = []
        for filename in os.listdir(exeSourceDir):
            if filename.endswith('.cpp'):
                sourceFiles.append(os.path.join(exeSourceDir, filename))

        if TestType.COMPILE in test.testType:
            cmd = [test.cxx, '/c', *sourceFiles, *test.flags, *test.compileFlags]
        elif TestType.RUN in test.testType:
            shared.execFile = outputBase + '.exe'
            cmd = [test.cxx, *sourceFiles, *test.flags, *test.compileFlags, '/Fe' + shared.execFile,
                   '/link', *test.linkFlags]

        yield TestStep(cmd, shared.execDir, shared.env, False)
