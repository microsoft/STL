# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from itertools import chain
from pathlib import Path
import enum
import os
import shutil

from lit.Test import FAIL, PASS, SKIPPED, Test, XPASS, XFAIL

from stl.compiler import CXXCompiler

_compiler_path_cache = dict()


class TestType(enum.Enum):
    COMPILE_PASS = enum.auto()
    LINK_PASS = enum.auto()
    RUN_PASS = enum.auto()

    COMPILE_FAIL = enum.auto()
    LINK_FAIL = enum.auto()
    RUN_FAIL = enum.auto()

    SKIPPED = enum.auto()


class STLTest(Test):
    def __init__(self, suite, path_in_suite, lit_config, test_config,
                 envlst_entry, env_num, default_cxx, file_path=None):
        self.build_only = False
        self.env_num = env_num
        self.skipped = False
        Test.__init__(self, suite, path_in_suite, test_config, file_path)

        self._configure_test_type(suite, path_in_suite, lit_config,
                                  test_config, env_num)
        if self.test_type is TestType.SKIPPED:
            self.script_result = (SKIPPED, 'Test was marked as skipped')
            return

        self._configure_cxx(lit_config, envlst_entry, default_cxx)

        for flag in chain(self.cxx.flags, self.cxx.compile_flags):
            if flag[1:] == 'clr:pure':
                self.requires.append('clr_pure')  # TRANSITION, GH-798
            elif flag[1:] == 'clr':
                self.requires.append('clr')  # TRANSITION, GH-797
            elif flag[1:] == 'BE':
                self.requires.append('edg')  # available for x86, see config.py

        if test_config.target_arch.startswith('arm'):
            self.build_only = True  # TRANSITION, GH-820

    def getIntegratedScriptResult(self):
        pass

    def getOutputDir(self):
        return Path(os.path.join(
            self.suite.getExecPath(self.path_in_suite[:-1]))) / self.env_num

    def getOutputBaseName(self):
        return self.path_in_suite[-2]

    def getExecDir(self):
        return self.getOutputDir()

    def getExecPath(self):
        return self.getExecDir() / (self.getOutputBaseName() + '.exe')

    def getTestName(self):
        return '/'.join(self.path_in_suite[:-1]) + ":" + self.env_num

    def getFullName(self):
        return self.suite.config.name + ' :: ' + self.getTestName()

    def getPassFailResultCodes(self):
        should_fail = self.isExpectedToFail()
        pass_var = XPASS if should_fail else PASS
        fail_var = XFAIL if should_fail else FAIL

        return pass_var, fail_var

    def _configure_expected_result(self, suite, path_in_suite, lit_config,
                                   test_config, env_num):
        test_name = self.getTestName()
        self.expected_result = None

        current_prefix = ""
        for prefix, result in \
                chain(test_config.expected_results.items(),
                      lit_config.expected_results.get(test_config.name,
                                                      dict()).items()):
            if test_name == prefix:
                self.expected_result = result
                break
            elif test_name.startswith(prefix) and \
                    len(prefix) > len(current_prefix):
                current_prefix = prefix
                self.expected_result = result

        if test_name in test_config.expected_results:
            self.expected_result = test_config.expected_results[test_name]
        elif test_name in lit_config.expected_results:
            self.expected_result = lit_config.expected_results[test_name]

        if self.expected_result is not None:
            if self.expected_result == SKIPPED:
                self.skipped = True
            elif self.expected_result.isFailure:
                self.xfails = ['*']

    def _configure_cxx(self, lit_config, envlst_entry, default_cxx):
        env_compiler = envlst_entry.getEnvVal('PM_COMPILER', 'cl')

        if not os.path.isfile(env_compiler):
            cxx = _compiler_path_cache.get(env_compiler, None)

            if cxx is None:
                search_paths = self.config.environment['PATH']
                cxx = shutil.which(env_compiler, path=search_paths)
                _compiler_path_cache[env_compiler] = cxx

        if not cxx:
            lit_config.fatal('Could not find: %r' % env_compiler)

        flags = list()
        compile_flags = list()
        link_flags = list()

        flags.extend(default_cxx.flags or [])
        compile_flags.extend(default_cxx.compile_flags or [])
        link_flags.extend(default_cxx.link_flags or [])

        flags.extend(envlst_entry.getEnvVal('PM_CL', '').split())
        link_flags.extend(envlst_entry.getEnvVal('PM_LINK', '').split())

        if ('clang'.casefold() in os.path.basename(cxx).casefold()):
            target_arch = self.config.target_arch.casefold()
            if (target_arch == 'x64'.casefold()):
                compile_flags.append('-m64')
            elif (target_arch == 'x86'.casefold()):
                compile_flags.append('-m32')
            elif (target_arch == 'arm'.casefold()):
                self.requires.append('clang-cl_arm_backend')
            elif (target_arch == 'arm64'.casefold()):
                self.requires.append('clang-cl_arm64_backend')

        self.cxx = CXXCompiler(cxx, flags, compile_flags, link_flags,
                               default_cxx.compile_env)

    def _configure_test_type(self, suite, path_in_suite, lit_config,
                             test_config):
        test_name = self.getTestName()
        self.test_type = None

        current_prefix = ""
        for prefix, type in \
                chain(test_config.test_type_overrides.items(),
                      lit_config.test_type_overrides.get(test_config.name,
                                                         dict()).items()):
            if test_name == prefix:
                self.test_type = type
                return
            elif test_name.startswith(prefix) and \
                    len(prefix) > len(current_prefix):
                current_prefix = prefix
                self.test_type = type

        if self.test_type is not None:
            return

        filename = path_in_suite[-1]
        if filename.endswith('.compile.pass.cpp'):
            self.test_type = TestType.COMPILE_PASS
        elif filename.endswith('.link.pass.cpp'):
            self.test_type = TestType.LINK_PASS
        elif filename.endswith('.pass.cpp'):
            self.test_type = TestType.RUN_PASS
        elif filename.endswith('.compile.fail.cpp'):
            self.test_type = TestType.COMPILE_FAIL
        elif filename.endswith('.link.fail.cpp'):
            self.test_type = TestType.LINK_FAIL
        elif filename.endswith('.run.fail.cpp'):
            self.test_type = TestType.RUN_FAIL
        elif filename.endswith('.fail.cpp'):
            self.test_type = TestType.COMPILE_FAIL
        else:
            self.test_type = TestType.RUN_PASS


class LibcxxTest(STLTest):
    def getOutputBaseName(self):
        output_base = self.path_in_suite[-1]

        if output_base.endswith('.cpp'):
            return output_base[:-4]
        else:
            return output_base

    def getOutputDir(self):
        dir_name = self.path_in_suite[-1]
        if dir_name.endswith('.cpp'):
            dir_name = dir_name[:-4]

        return Path(os.path.join(
            self.suite.getExecPath(self.path_in_suite[:-1]))) / dir_name / \
            self.env_num

    def getTestName(self):
        return '/'.join(self.path_in_suite) + ':' + self.env_num
