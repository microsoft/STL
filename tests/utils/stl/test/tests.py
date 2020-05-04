# Copyright (c) Microsoft Corporation.
#===----------------------------------------------------------------------===##
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===##

from itertools import chain
from pathlib import Path
import enum
import os
import re
import shutil

from lit.Test import Test
import lit

from stl.compiler import CXXCompiler

_CMAKE_TARGET_NAME_REGEX = re.compile('[^a-zA-Z0-9_.+\-]+')
_name_counter = 0

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
        self.env_num = env_num
        self.skipped = False
        Test.__init__(self, suite, path_in_suite, test_config, file_path)

        self._configure_name()
        self._configure_test_type(suite, path_in_suite, lit_config,
                                  test_config)
        if self.test_type is TestType.SKIPPED:
            self.script_result = (lit.Test.SKIPPED,
                                  "Test was marked as skipped")
            return

        self._configure_cxx(lit_config, envlst_entry, default_cxx)

        # TRANSITION: These configurations should be enabled in the future.
        for flag in chain(self.cxx.flags, self.cxx.compile_flags):
            if flag.startswith('clr:pure', 1):
                self.requires.append('clr_pure')
            elif flag.startswith('BE', 1):
                self.requires.append('edg')

        self.script_result = self._get_integrated_script_result(lit_config)

    def _get_integrated_script_result(self, lit_config):
        name = self.path_in_suite[-1]
        name_root, name_ext = os.path.splitext(name)
        is_sh_self = name_root.endswith('.sh')
        is_objcxx_self = name.endswith('.mm')

        if is_sh_self:
            return (lit.Test.UNSUPPORTED,
                    "Sh selfs are currently unsupported")

        if is_objcxx_self:
            return (lit.Test.UNSUPPORTED,
                    "Objective-C selfs are unsupported")

        if self.config.unsupported:
            return (lit.Test.UNSUPPORTED,
                    "A lit.local.cfg marked this unsupported")

        if lit_config.noExecute:
            return lit.Test.Result(lit.Test.PASS)

        script = lit.TestRunner.parseIntegratedTestScript(
            self, require_script=False)

        if isinstance(script, lit.Test.Result):
            return script

        return None

    def _configure_name(self):
        global _name_counter

        self.name = _CMAKE_TARGET_NAME_REGEX.sub(
            str(_name_counter), '-'.join(self.path_in_suite[:-1])) + \
            '--' + self.env_num

        _name_counter = _name_counter + 1

    def getOutputDir(self):
        return Path(os.path.join(
            self.suite.getExecPath(self.path_in_suite[:-1]))) / self.env_num

    def getOutputBaseName(self):
        return self.path_in_suite[-2]

    def getExecDir(self):
        return self.getOutputDir()

    def getExecPath(self):
        return self.getExecDir() / (self.getOutputBaseName() + '.exe')

    def getTestFilePath(self):
        return self.getOutputDir() / 'test.cmake'

    def getTestName(self):
        return self.name

    def getFullName(self):
        return self.suite.config.name + '--' + self.getTestName()

    def _configure_test_type(self, suite, path_in_suite, lit_config,
                             test_config):
        test_name = '/'.join(path_in_suite) + ':' + self.env_num
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

    def shouldBuild(self):
        return self.test_type in (TestType.COMPILE_PASS, TestType.LINK_PASS,
                                  TestType.RUN_PASS, TestType.RUN_FAIL)

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

        self.cxx = CXXCompiler(cxx, flags, compile_flags, link_flags,
                               default_cxx.compile_env)


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

    def _configure_name(self):
        global _name_counter

        self.name = _CMAKE_TARGET_NAME_REGEX.sub(
            str(_name_counter), '-'.join(self.path_in_suite[:-1]) + '-' +
            self.getOutputBaseName()) + "--" + self.env_num

        _name_counter = _name_counter + 1
