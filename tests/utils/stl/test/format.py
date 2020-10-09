#===----------------------------------------------------------------------===##
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===##

from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, Optional
import copy
import itertools
import errno
import os
import shutil
import time

import lit.Test        # pylint: disable=import-error
import lit.TestRunner  # pylint: disable=import-error

from stl.test.tests import STLTest, LibcxxTest
import stl.test.file_parsing
import stl.util


@dataclass
class TestStep:
    cmd: List[str] = field(default_factory=list)
    work_dir: os.PathLike = field(default=Path('.'))
    file_deps: List[os.PathLike] = field(default_factory=list)
    env: Dict[str, str] = field(default_factory=dict)
    should_fail: bool = field(default=False)


class STLTestFormat:
    """
    Custom test format handler to run MSVC tests.
    """

    def __init__(self, default_cxx, execute_external,
                 build_executor, test_executor):
        self.cxx = default_cxx
        self.execute_external = execute_external
        self.build_executor = build_executor
        self.test_executor = test_executor

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

        envlst_path = self.getEnvLst(source_path, localConfig)

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
                    format_string = "{:0" + str(len(str(len(env_entries)))) + \
                                    "d}"
                    for env_entry, env_num \
                            in zip(env_entries, itertools.count()):
                        test_config = copy.deepcopy(localConfig)
                        test_path_in_suite = path_in_suite + (filename,)

                        yield test_class(testSuite,
                                         test_path_in_suite,
                                         litConfig, test_config,
                                         env_entry,
                                         format_string.format(env_num),
                                         self.cxx)

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

    def cleanup(self, test):
        shutil.rmtree(test.getExecDir(), ignore_errors=True)
        shutil.rmtree(test.getOutputDir(), ignore_errors=True)

    def getIntegratedScriptResult(self, test, lit_config):
        if test.skipped:
            return (lit.Test.SKIPPED, "Test was marked as skipped")

        name = test.path_in_suite[-1]
        name_root, name_ext = os.path.splitext(name)
        is_sh_test = name_root.endswith('.sh')
        is_objcxx_test = name.endswith('.mm')

        if is_sh_test:
            return (lit.Test.UNSUPPORTED,
                    "Sh tests are currently unsupported")

        if is_objcxx_test:
            return (lit.Test.UNSUPPORTED,
                    "Objective-C tests are unsupported")

        if test.config.unsupported:
            return (lit.Test.UNSUPPORTED,
                    "A lit.local.cfg marked this unsupported")

        if lit_config.noExecute:
            return lit.Test.Result(lit.Test.PASS)

        if test.expected_result is None:
            script = lit.TestRunner.parseIntegratedTestScript(
                test, require_script=False)

            if isinstance(script, lit.Test.Result):
                return script

        return None

    def execute(self, test, lit_config):
        result = None
        while True:
            try:
                result = self._execute(test, lit_config)
                break
            except OSError as oe:
                if oe.errno != errno.ETXTBSY:
                    raise
                time.sleep(0.1)

        return result

    def _execute(self, test, lit_config):
        # TRANSITION: It is potentially wasteful that all the skipping and
        # unsupported logic lives here when it is known at time of test
        # discovery. Investigate
        script_result = self.getIntegratedScriptResult(test, lit_config)
        if script_result is not None:
            return script_result

        try:
            self.setup(test)
            pass_var, fail_var = test.getPassFailResultCodes()
            buildSteps, testSteps = self.getSteps(test, lit_config)

            report = ""
            for step in buildSteps:
                cmd, out, err, rc = \
                    self.build_executor.run(step.cmd, step.work_dir,
                                            step.file_deps, step.env)

                if step.should_fail and rc == 0:
                    report += "Build step succeeded unexpectedly.\n"
                elif rc != 0:
                    report += "Build step failed unexpectedly.\n"

                report += stl.util.makeReport(cmd, out, err, rc)
                if (step.should_fail and rc == 0) or \
                        (not step.should_fail and rc != 0):
                    lit_config.note(report)
                    return lit.Test.Result(fail_var, report)

            for step in testSteps:
                cmd, out, err, rc = \
                    self.test_executor.run(step.cmd, step.work_dir,
                                           step.file_deps, step.env)

                if step.should_fail and rc == 0:
                    report += "Test step succeeded unexpectedly.\n"
                elif rc != 0:
                    report += "Test step failed unexpectedly.\n"

                report += stl.util.makeReport(cmd, out, err, rc)
                if (step.should_fail and rc == 0) or \
                        (not step.should_fail and rc != 0):
                    lit_config.note(report)
                    return lit.Test.Result(fail_var, report)

            return lit.Test.Result(pass_var, report)

        except Exception as e:
            lit_config.warning(str(e))
            raise e
        finally:
            self.cleanup(test)

    def getSteps(self, test, lit_config):
        @dataclass
        class SharedState:
            exec_file: Optional[os.PathLike] = field(default=None)
            exec_dir: os.PathLike = field(default_factory=Path)

        shared = SharedState()
        return self.getBuildSteps(test, lit_config, shared), \
            self.getTestSteps(test, lit_config, shared)

    def getBuildSteps(self, test, lit_config, shared):
        if not test.path_in_suite[-1].endswith('.fail.cpp'):
            shared.exec_dir = test.getExecDir()
            output_base = test.getOutputBaseName()
            output_dir = test.getOutputDir()
            source_path = Path(test.getSourcePath())

            flags = []
            isense_rsp_path = None
            if test.cxx.edg_drop is not None:
                isense_rsp_path = output_dir / (output_base + '.isense.rsp')
                flags.extend(['/dE--write-isense-rsp', '/dE' + str(isense_rsp_path)])

            cmd, out_files, shared.exec_file = \
                test.cxx.executeBasedOnFlagsCmd([source_path], output_dir,
                                                shared.exec_dir, output_base,
                                                flags, [], [])

            yield TestStep(cmd, shared.exec_dir, [source_path],
                           test.cxx.compile_env)

            if isense_rsp_path is not None:
                with open(isense_rsp_path) as f:
                    cmd = [line.strip() for line in f]
                cmd[0] = test.cxx.edg_drop

                # cpfecl translates /Fo into --import_dir, but that is not
                # used in the same way by upstream EDG.
                try:
                    index = cmd.index('--import_dir')
                    cmd.pop(index)
                    cmd.pop(index)
                except ValueError:
                    pass

                # --print_diagnostics is not recognized by upstream EDG.
                try:
                    index = cmd.index('--print_diagnostics')
                    cmd.pop(index)
                except ValueError:
                    pass

                yield TestStep(cmd, shared.exec_dir, [source_path],
                               test.cxx.compile_env)

    def getTestSteps(self, test, lit_config, shared):
        if shared.exec_file is not None:
            exec_env = test.cxx.compile_env
            exec_env['TMP'] = str(shared.exec_dir)

            yield TestStep([str(shared.exec_file)], shared.exec_dir,
                           [shared.exec_file], exec_env)
        elif test.path_in_suite[-1].endswith('.fail.cpp'):
            exec_dir = test.getExecDir()
            source_path = Path(test.getSourcePath())

            flags = []
            if test.cxx.name == 'cl' and \
                    ('/analyze' in test.cxx.flags or
                     '/analyze' in test.cxx.compile_flags):
                output_base = test.getOutputBaseName()
                output_dir = test.getOutputDir()
                analyze_path = output_dir / (output_base +
                                             '.nativecodeanalysis.xml')
                flags.append('/analyze:log' + str(analyze_path))

            cmd, _ = test.cxx.compileCmd([source_path], os.devnull, flags)
            yield TestStep(cmd, exec_dir, [source_path],
                           test.cxx.compile_env, True)


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
