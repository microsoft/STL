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
    def __init__(self, suite, pathInSuite, litConfig, testConfig, envlstEntry, envNum):
        self.envNum = envNum
        self.envlstEntry = envlstEntry
        Test.__init__(self, suite, pathInSuite, testConfig, None)

    def configureTest(self, litConfig):
        self.compileFlags = []
        self.cxx = None
        self.fileDependencies = []
        self.flags = []
        self.isenseRspPath = None
        self.linkFlags = []
        self.testType = None

        result = self._configureExpectedResult(litConfig)
        if result:
            return result

        result = self._handleEnvlst(litConfig)
        if result:
            return result

        self._parseTest()
        self._parseFlags()

        missing_required_features = self.getMissingRequiredFeatures()
        if missing_required_features:
            msg = ', '.join(missing_required_features)
            return Result(UNSUPPORTED, "Test requires the following unavailable features: %s" % msg)

        unsupported_features = self.getUnsupportedFeatures()
        if unsupported_features:
            msg = ', '.join(unsupported_features)
            return Result(UNSUPPORTED, "Test does not support the following features and/or targets: %s" % msg)

        if not self.isWithinFeatureLimits():
            msg = ', '.join(self.config.limit_to_features)
            return Result(UNSUPPORTED, "Test does not require any of the features specified in limit_to_features: %s" %
                          msg)

        if 'edg_drop' in self.config.available_features:
            if not 'edg' in self.requires:
                return Result(UNSUPPORTED, 'We only run /BE tests with the edg drop')

            _, tmpBase = self.getTempPaths()
            self.isenseRspPath = tmpBase + '.isense.rsp'
            self.compileFlags.extend(['/dE--write-isense-rsp', '/dE' + self.isenseRspPath])

        self._configureTestType()
        return None

    def _parseTest(self):
        additionalCompileFlags = []
        fileDependencies = []
        parsers = [
            lit.TestRunner.IntegratedTestKeywordParser('FILE_DEPENDENCIES:',
                                                       lit.TestRunner.ParserKind.LIST,
                                                       initial_value=fileDependencies),
            lit.TestRunner.IntegratedTestKeywordParser('ADDITIONAL_COMPILE_FLAGS:',
                                                       lit.TestRunner.ParserKind.LIST,
                                                       initial_value=additionalCompileFlags)
        ]

        lit.TestRunner.parseIntegratedTestScript(self, additional_parsers=parsers, require_script=False)
        self.compileFlags.extend(additionalCompileFlags)
        self.fileDependencies.extend(fileDependencies)

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
                filename.endswith(('.compile.pass.cpp', '.compile.fail.cpp')):
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

    def _configureExpectedResult(self, litConfig):
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
                return Result(SKIPPED, 'This test was explicitly marked as skipped')
            elif self.expectedResult.isFailure:
                self.xfails = ['*']
        elif self.config.unsupported:
            return Result(UNSUPPORTED, 'This test was marked as unsupported by a lit.cfg')

        return None

    def _handleEnvlst(self, litConfig):
        envCompiler = self.envlstEntry.getEnvVal('PM_COMPILER', 'cl')

        cxx = None
        if os.path.isfile(envCompiler):
            cxx = envCompiler
        else:
            cxx = _compilerPathCache.get(envCompiler, None)

            if not cxx:
                searchPaths = self.config.environment['PATH']
                cxx = shutil.which(envCompiler, path=searchPaths)
                _compilerPathCache[envCompiler] = cxx

        if not cxx:
            litConfig.warning('Could not find: %r' % envCompiler)
            return Result(SKIPPED, 'This test was skipped because the compiler, "' +
                                   envCompiler + '", could not be found')

        self.flags = copy.deepcopy(litConfig.flags[self.config.name])
        self.compileFlags = copy.deepcopy(litConfig.compile_flags[self.config.name])
        self.linkFlags = copy.deepcopy(litConfig.link_flags[self.config.name])

        self.compileFlags.extend(self.envlstEntry.getEnvVal('PM_CL', '').split())
        self.linkFlags.extend(self.envlstEntry.getEnvVal('PM_LINK', '').split())

        if ('clang'.casefold() in os.path.basename(cxx).casefold()):
            targetArch = litConfig.target_arch.casefold()
            if (targetArch == 'x64'.casefold()):
                self.compileFlags.append('-m64')
            elif (targetArch == 'x86'.casefold()):
                self.compileFlags.append('-m32')

        if ('nvcc'.casefold() in os.path.basename(cxx).casefold()):
            # nvcc only supports targeting x64
            self.requires.append('x64')

        self.cxx = os.path.normpath(cxx)
        return None

    def _parseFlags(self):
        foundStd = False
        for flag in chain(self.flags, self.compileFlags, self.linkFlags):
            if flag[1:5] == 'std:':
                foundStd = True
                if flag[5:] == 'c++latest':
                    Feature('c++2a').enableIn(self.config)
                elif flag[5:] == 'c++17':
                    Feature('c++17').enableIn(self.config)
                elif flag[5:] == 'c++14':
                    Feature('c++14').enableIn(self.config)
            elif flag[1:] == 'clr:pure':
                self.requires.append('clr_pure') # TRANSITION, GH-798
            elif flag[1:] == 'clr':
                self.requires.append('clr') # TRANSITION, GH-797
            elif flag[1:] == 'BE':
                self.requires.append('edg') # available for x86, see features.py
            elif flag[1:] == 'arch:AVX2':
                self.requires.append('arch_avx2') # available for x86 and x64, see features.py
            elif flag[1:] == 'arch:IA32':
                self.requires.append('arch_ia32') # available for x86, see features.py
            elif flag[1:] == 'arch:VFPv4':
                self.requires.append('arch_vfpv4') # available for arm, see features.py

        if not foundStd:
            Feature('c++14').enableIn(self.config)


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
