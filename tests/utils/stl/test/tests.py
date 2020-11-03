# Copyright (c) Microsoft Corporation.
#===----------------------------------------------------------------------===##
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===##

from enum import Flag, auto
from itertools import chain
import copy
import os
import shutil

from lit.Test import SKIPPED, Result, Test, UNRESOLVED, UNSUPPORTED
from libcxx.test.dsl import Feature
import lit

_compilerPathCache = dict()


class TestType(Flag):
    UNKNOWN = auto()
    COMPILE = auto()
    LINK = auto()
    RUN = auto()
    PASS = auto()
    FAIL = auto()


class STLTest(Test):
    # TRANSITION: A lot of the members of this class could be computed at execution time to minimize the transmitted
    # size of the class.
    def __init__(self, suite, pathInSuite, litConfig, testConfig,
                 envlstEntry, envNum):
        self.compileFlags = []
        self.cxx = None
        self.envNum = envNum
        self.fileDependencies = []
        self.flags = []
        self.isenseRspPath = None
        self.linkFlags = []
        self.testType = None
        Test.__init__(self, suite, pathInSuite, copy.deepcopy(testConfig), None)

        self._configureExpectedResult(suite, litConfig)
        if self.result:
            return

        self._handleEnvlst(litConfig, envlstEntry)
        self._configureTestType()
        _, tmpBase = self.getTempPaths()

    def _configureTestType(self):
        self.testType = TestType.UNKNOWN
        filename = self.path_in_suite[-1]

        if filename.endswith('.fail.cpp'):
            self.testType = self.testType | TestType.FAIL
        elif filename.endswith('.pass.cpp'):
            self.testType = self.testType | TestType.PASS
        else:
            self.testType = self.testType | TestType.PASS

        shortenedFlags = [flag[1:] for flag in chain(self.flags, self.compileFlags, self.linkFlags)]
        if 'analyze:only' in shortenedFlags or 'c' in shortenedFlags or \
                filename.endswith(('.compile.pass.cpp', 'compile.fail.cpp')):
            self.testType = self.testType | TestType.COMPILE
        elif filename.endswith(('.link.pass.cpp', '.link.fail.cpp')):
            self.testType = self.testType | TestType.LINK
        elif filename.endswith(('run.fail.cpp','run.pass.cpp')):
            self.testType = self.testType | TestType.RUN
        elif filename.endswith('.fail.cpp'):
            self.testType = self.testType | TestType.COMPILE
        elif filename.endswith('.pass.cpp'):
            self.testType = self.testType | TestType.RUN
        else:
            self.testType = self.testType | TestType.RUN

        self.testType = self.testType & ~TestType.UNKNOWN

    def getTestName(self):
        return '/'.join(self.path_in_suite[:-1]) + ":" + self.envNum

    def getFullName(self):
        return self.suite.config.name + ' :: ' + self.getTestName()

    def getSourcePath(self):
        return os.path.normpath(super().getSourcePath())

    def getExecDir(self):
        execDir, _ = lit.TestRunner.getTempPaths(self)
        execDir = os.path.join(execDir, self.envNum)
        return os.path.normpath(execDir)

    def getTempPaths(self):
        tmpDir = self.getExecDir()
        tmpBase = os.path.join(tmpDir, self.path_in_suite[-2])
        return tmpDir, os.path.normpath(tmpBase)

    def _configureExpectedResult(self, suite, litConfig):
        testName = self.getTestName()
        self.expectedResult = None

        if testName in litConfig.expected_results.get(self.config.name, dict()):
            self.expectedResult = litConfig.expected_results[self.config.name][testName]
        else:
          currentPrefix = ""
          for prefix, result in litConfig.expected_results.get(self.config.name, dict()).items():
              if testName == prefix:
                  self.expectedResult = result
                  break
              elif testName.startswith(prefix) and len(prefix) > len(currentPrefix):
                  currentPrefix = prefix
                  self.expectedResult = result

        if self.expectedResult is not None:
            if self.expectedResult == SKIPPED:
                self.result = Result(SKIPPED, 'This test was explicitly marked as skipped')
            elif self.expectedResult.isFailure:
                self.xfails = ['*']
        elif self.config.unsupported:
            self.result = Result(UNSUPPORTED, 'This test was marked as unsupported by a lit.cfg')

    def _handleEnvlst(self, litConfig, envlstEntry):
        envCompiler = envlstEntry.getEnvVal('PM_COMPILER', 'cl')

        cxx = None
        if not os.path.isfile(envCompiler):
            cxx = _compilerPathCache.get(envCompiler, None)

            if cxx is None:
                searchPaths = self.config.environment['PATH']
                cxx = shutil.which(envCompiler, path=searchPaths)
                _compilerPathCache[envCompiler] = cxx
        else:
            cxx = envCompiler

        if not cxx:
            litConfig.fatal('Could not find: %r' % envCompiler)

        self.flags = copy.deepcopy(litConfig.flags[self.config.name])
        self.compileFlags = copy.deepcopy(litConfig.compile_flags[self.config.name])
        self.linkFlags = copy.deepcopy(litConfig.link_flags[self.config.name])

        self.compileFlags.extend(envlstEntry.getEnvVal('PM_CL', '').split())
        self.linkFlags.extend(envlstEntry.getEnvVal('PM_LINK', '').split())

        if ('clang'.casefold() in os.path.basename(cxx).casefold()):
            targetArch = litConfig.target_arch.casefold()
            if (targetArch == 'x64'.casefold()):
                self.compileFlags.append('-m64')
            elif (targetArch == 'x86'.casefold()):
                self.compileFlags.append('-m32')

        self.cxx = os.path.normpath(cxx)

        foundStd = False
        for flag in chain(self.flags, self.compileFlags, self.linkFlags):
            if flag[1:5] == 'std:':
                foundStd = True
                if flag[5:] == 'c++latest':
                    Feature('c++2a').enableIn(self.config)
                    self.compileFlags.append('/D_LIBCPP_CONSTEXPR_AFTER_CXX17=constexpr')
                elif flag[5:] == 'c++17':
                    Feature('c++17').enableIn(self.config)
                    self.compileFlags.append('/D_LIBCPP_CONSTEXPR_AFTER_CXX17=constexpr')
                elif flag[5:] == 'c++14':
                    Feature('c++14').enableIn(self.config)
                    self.compileFlags.append('/D_LIBCPP_CONSTEXPR_AFTER_CXX17= ')
            elif flag[1:] == 'clr:pure':
                self.requires.append('clr_pure') # TRANSITION, GH-798
            elif flag[1:] == 'clr':
                self.requires.append('clr') # TRANSITION, GH-797
            elif flag[1:] == 'BE':
                self.requires.append('edg') # available for x86, see features.py
            elif flag[1:] == 'kernel':
                self.requires.append('kernel')

        if not foundStd:
            Feature('c++14').enableIn(self.config)
            self.compileFlags.append('/D_LIBCPP_CONSTEXPR_AFTER_CXX17= ')

        if 'edg_drop' in self.config.available_features:
            if not 'edg' in self.requires:
                self.result = Result(UNSUPPORTED, 'We only run /BE tests with the edg drop')
            else:
                _, tmpBase = self.getTempPaths()
                self.isenseRspPath = tmpBase + '.isense.rsp'
                self.compileFlags.extend(['/dE--write-isense-rsp', '/dE' + self.isenseRspPath])


class LibcxxTest(STLTest):
    def getTestName(self):
        return '/'.join(self.path_in_suite) + ':' + self.envNum

    def getExecDir(self):
        execDir, _ = lit.TestRunner.getTempPaths(self)
        execDir = os.path.join(execDir, self.path_in_suite[-1] + '.dir')
        execDir = os.path.join(execDir, self.envNum)
        return os.path.normpath(execDir)

    def getTempPaths(self):
        tmpDir = self.getExecDir()
        tmpBase = os.path.join(tmpDir, self.path_in_suite[-1])
        if tmpBase.endswith('.cpp'):
            tmpBase = tmpBase[:-4]
        return tmpDir, os.path.normpath(tmpBase)
