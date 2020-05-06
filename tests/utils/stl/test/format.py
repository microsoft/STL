#===----------------------------------------------------------------------===##
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===##

from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List
import copy
import itertools
import os
import shutil

import lit.Test        # pylint: disable=import-error
import lit.TestRunner  # pylint: disable=import-error

from stl.test.tests import STLTest, LibcxxTest, TestType
import stl.test.file_parsing
import stl.util


@dataclass
class TestStep:
    cmd: List[str] = field(default_factory=list)
    dependencies: List[os.PathLike] = field(default_factory=list)
    env: Dict[str, str] = field(default_factory=dict)
    num: int = field(default=0)
    out_files: List[os.PathLike] = field(default_factory=list)
    should_fail: bool = field(default=False)
    work_dir: os.PathLike = field(default=Path('.'))


_test_suite_file_handles = {}


class STLTestFormat:
    """
    Custom test format handler to run MSVC tests.
    """

    def __init__(self, default_cxx, execute_external,
                 build_step_writer, test_step_writer):
        self.cxx = default_cxx
        self.execute_external = execute_external
        self.build_step_writer = build_step_writer
        self.test_step_writer = test_step_writer

    def getSteps(self, test, lit_config):
        class SharedState:
            def __init__(self, test):
                self.exec_dir = test.getExecDir()
                self.exec_env = test.cxx.compile_env
                self.exec_env['TMP'] = str(self.exec_dir)
                self.exec_env['TEMP'] = str(self.exec_dir)
                self.exec_env['TMPDIR'] = str(self.exec_dir)
                self.exec_env['TEMPDIR'] = str(self.exec_dir)
                self.exec_file = None

        shared = SharedState(test)
        return self.getBuildSteps(test, lit_config, shared), \
            self.getTestSteps(test, lit_config, shared)

    def getBuildSteps(self, test, lit_config, shared):
        output_base = test.getOutputBaseName()
        output_dir = test.getOutputDir()
        source_path = Path(test.getSourcePath())

        cmd, out_files, shared.exec_file = \
            test.cxx.executeBasedOnFlagsCmd([source_path], output_dir,
                                            shared.exec_dir, output_base,
                                            [], [], [])

        yield TestStep(cmd=cmd, dependencies=[source_path],
                       env=shared.exec_env, out_files=out_files,
                       should_fail=not test.shouldBuild(),
                       work_dir=shared.exec_dir)

    def getTestSteps(self, test, lit_config, shared):
        if shared.exec_file is None:
            return

        if test.test_type in (TestType.RUN_PASS, TestType.RUN_FAIL):
            yield TestStep(cmd=[str(shared.exec_file)],
                           dependencies=[shared.exec_file],
                           env=shared.exec_env,
                           should_fail=test.test_type is TestType.RUN_FAIL,
                           work_dir=shared.exec_dir)

    def isLegalDirectory(self, source_path, litConfig):
        found = False
        for prefix in getattr(litConfig, 'test_subdirs', []):
            if os.path.commonpath((source_path, prefix)) == prefix or \
                    os.path.commonpath((prefix, source_path)) == source_path:
                found = True
                break

        return found

    def getEnvLst(self, source_path, localConfig):
        envlst_path = getattr(localConfig, 'envlst_path', None)
        if envlst_path is None:
            cwd = Path(source_path)
            if (cwd / 'env.lst').is_file():
                envlst_path = cwd / 'env.lst'
            else:
                for parent in cwd.parents:
                    if (parent / 'env.lst').is_file():
                        envlst_path = parent / 'env.lst'
                        break

        return envlst_path

    def getTestsInDirectory(self, testSuite, path_in_suite,
                            litConfig, localConfig, test_class=STLTest):
        source_path = testSuite.getSourcePath(path_in_suite)

        if not self.isLegalDirectory(source_path, litConfig):
            return

        file_handle = None
        mangled_dir_name = stl.util.mangleCMakeTarget(testSuite.name + '--' + '-'.join(path_in_suite))

        if testSuite.name not in _test_suite_file_handles:
            test_list = Path(testSuite.exec_root) / 'tests.cmake'
            file_handle = test_list.open('w')
            _test_suite_file_handles[testSuite.name] = file_handle

            dep_string = 'add_custom_target({dir_name})\nadd_dependencies({suite_name} {dir_name})'
            print(dep_string.format(suite_name=testSuite.name,
                                    dir_name=mangled_dir_name),
                  file=file_handle)
        else:
            dir_path = Path(testSuite.exec_root) / os.path.join(path_in_suite)
            dir_path.mkdir(parents=True, exist_ok=True)
            dir_file = dir_path / 'CMakeLists.txt'
            file_handle = dir_file.open('w')
            dep_string = 'add_custom_target({dir_name})\nadd_dependencies({updir_name} {dir_name})\nadd_custom_target(check-{dir_name} COMMAND -I \\"${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake\\"" DEPENDS {dir_name} COMMENT "Running {dir_name} tests" USES_TERMINAL)'
            print(dep_string.format(updir_name=stl.util.mangleCMakeTarget(testSuite.name + '--' + '-'.join(path_in_suite[:-1])),
                                    dir_name=mangled_dir_name),
                  file=file_handle)

        envlst_path = self.getEnvLst(source_path, localConfig)
        exec_path = Path(os.path.join(testSuite.getExecPath(path_in_suite)))

        for filename in os.listdir(source_path):
            # Ignore dot files and excluded tests.
            filepath = os.path.join(source_path, filename)
            if filename.startswith('.'):
                continue

            if not os.path.isdir(filepath):
                if any([filename.endswith(ext)
                        for ext in localConfig.suffixes]):

                    if envlst_path is None:
                        litConfig.fatal("Could not find an env.lst file.")

                    env_entries = \
                        stl.test.file_parsing.parse_env_lst_file(envlst_path)
                    env_num_string = "{:0" + str(len(str(len(env_entries)))) + \
                                     "d}"

                    is_first = True
                    top_level_test_file_handle = None
                    for env_entry, env_num \
                            in zip(env_entries, itertools.count()):
                        test_config = copy.deepcopy(localConfig)
                        test_path_in_suite = path_in_suite + (filename,)

                        test = test_class(testSuite, test_path_in_suite,
                                          litConfig, test_config, env_entry,
                                          env_num_string.format(env_num),
                                          self.cxx)

                        if test.script_result is None:
                            if is_first:
                                top_level_test_dir = test.getOutputDir().parent
                                top_level_test_file = \
                                    top_level_test_dir / 'CMakeLists.txt'
                                top_level_test_file_handle = \
                                    top_level_test_file.open('w')
                                mangled_test_dir_name = \
                                    stl.util.mangleCMakeTarget(
                                        testSuite.name + '--' + '-'.join(top_level_test_dir.relative_to(Path(
                                print(

                                is_first = False

                            out_string = \
                                per_test_string.format(test_file=test_file,
                                                       dir_name=dir_name,
                                                       test_name=test.mangled_name)
                            print(out_string, file=file_handle)

                        yield test
            elif self.isLegalDirectory(filepath, litConfig):
                per_subdir_string = 'add_subdirectory({subdir_name})'

        file_handle.close()

    def setup(self, test):
        exec_dir = test.getExecDir()
        output_dir = test.getOutputDir()
        source_dir = Path(test.getSourcePath()).parent

        shutil.rmtree(exec_dir, ignore_errors=True)
        shutil.rmtree(output_dir, ignore_errors=True)
        exec_dir.mkdir(parents=True, exist_ok=True)
        output_dir.mkdir(parents=True, exist_ok=True)

        # TRANSITION: This should be handled by a TestStep with a dependency
        # on the .dat files the test requires.
        for path in source_dir.iterdir():
            if path.is_file() and path.name.endswith('.dat'):
                shutil.copy2(path, exec_dir / path.name)

    def execute(self, test, lit_config):
        try:
            if test.script_result is not None:
                return test.script_result

            self.setup(test)
            buildSteps, testSteps = self.getSteps(test, lit_config)

            test_file = test.getTestFilePath()
            test_target = 'add_custom_target(' + test.mangled_name + ')'

            with test_file.open('w') as f:
                print(test_target, file=f)

                for step in buildSteps:
                    self.build_step_writer.write(test, step, f)
                for step in testSteps:
                    self.test_step_writer.write(test, step, f)
        except Exception as e:
            lit_config.warning(str(e))
            raise e

        return lit.Test.Result(lit.Test.PASS,
                               'Command file was succesfully written')


class LibcxxTestFormat(STLTestFormat):
    """
    Custom test format handler to run the libcxx tests for the MSVC STL.
    """
    def getTestsInDirectory(self, testSuite, path_in_suite,
                            litConfig, localConfig, test_class=LibcxxTest):
        return super().getTestsInDirectory(testSuite, path_in_suite, litConfig,
                                           localConfig, test_class)

    def addCompileFlags(self, *args):
        # For now, this is necessary to discard the value of
        # `LIBCXX_FILESYSTEM_STATIC_TEST_ROOT` which we don't care about;
        # eventually it will probably need to be made meaningful.
        pass
