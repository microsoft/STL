# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import os

from stl.test.format import STLTestFormat, TestStep
from stl.test.tests import STLTest, TestType


class CustomTest(STLTest):
    def __init__(self, suite, pathInSuite, litConfig, testConfig, envlstEntry, envNum):
        STLTest.__init__(self, suite, pathInSuite, litConfig, testConfig, envlstEntry, envNum)
        self.callingConventionA = envlstEntry.getEnvVal('CALLING_CONVENTION_A')
        self.callingConventionB = envlstEntry.getEnvVal('CALLING_CONVENTION_B')


class CustomTestFormat(STLTestFormat):
    def getTestsInDirectory(self, testSuite, pathInSuite, litConfig, localConfig, testClass=CustomTest):
        return super().getTestsInDirectory(testSuite, pathInSuite, litConfig, localConfig, testClass)

    def getBuildSteps(self, test, litConfig, shared):
        exeSource = test.getSourcePath()
        aSource = os.path.join(os.path.dirname(exeSource), 'a.cpp')

        outputDir, outputBase = test.getTempPaths()
        aObj = os.path.join(outputDir, 'a.obj')

        cmd = [test.cxx, aSource, test.callingConventionA, *test.flags, '/c', *test.compileFlags, '/Fo' + aObj]

        yield TestStep(cmd, shared.execDir, shared.env, False)

        shared.execFile = outputBase + '.exe'
        cmd = [test.cxx, exeSource, aObj, test.callingConventionB, *test.flags, *test.compileFlags,
               '/Fe' + shared.execFile, '/link', *test.linkFlags]

        if TestType.COMPILE in test.testType:
            cmd = [test.cxx, '/c', exeSource, aObj, test.callingConventionB, *test.flags, *test.compileFlags]
        elif TestType.RUN in test.testType:
            shared.execFile = outputBase + '.exe'
            cmd = [test.cxx, exeSource, aObj, test.callingConventionB, *test.flags, *test.compileFlags,
                   '/Fe' + shared.execFile, '/link', *test.linkFlags]

        yield TestStep(cmd, shared.execDir, shared.env, False)
