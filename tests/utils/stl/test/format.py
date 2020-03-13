#===----------------------------------------------------------------------===##
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===##

from pathlib import Path
import copy
import itertools
import errno
import os
import time

import lit.Test        # pylint: disable=import-error
import lit.TestRunner  # pylint: disable=import-error

from stl.test.Test import STLTest, LibcxxTest
import stl.test.file_parsing
import stl.util


class STLTestFormat:
    """
    Custom test format handler to run MSVC tests.
    """

    def __init__(self, default_cxx, execute_external, executor):
        self.cxx = default_cxx
        self.execute_external = execute_external
        self.executor = executor

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

                    for env_entry, env_num \
                            in zip(stl.test.file_parsing.parse_env_lst_file(
                                   envlst_path), itertools.count()):
                        # TRANSITION: Get rid of this copy
                        test_config = copy.deepcopy(localConfig)
                        test_path_in_suite = path_in_suite + (filename,)

                        yield test_class(testSuite,
                                         test_path_in_suite,
                                         litConfig, test_config,
                                         env_entry, env_num,
                                         self.cxx)

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
        # discovery. Investigate.
        if test.skipped:
            return (stl.test.Test.SKIP, "Test was marked as skipped")

        name = test.path_in_suite[-1]
        name_root, name_ext = os.path.splitext(name)
        is_sh_test = name_root.endswith('.sh')
        is_fail_test = name.endswith('.fail.cpp')
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
            if lit_config.noExecute:
                return lit.Test.Result(lit.Test.PASS)

        try:
            test.setup()

            if is_fail_test:
                return self._evaluate_fail_test(test, lit_config)
            else:
                return self._evaluate_pass_test(test, lit_config)
        except Exception as e:
            lit_config.warning(e.strerror)
            raise e
        finally:
            test.cleanup()

    def _evaluate_pass_test(self, test, lit_config):
        exec_dir = test.getExecDir()
        output_base = test.getOutputBaseName()
        output_dir = test.getOutputDir()
        source_path = Path(test.getSourcePath())
        pass_var, fail_var = test.getPassFailResultCodes()

        compile_cmd, out, err, rc, out_files, exec_file = \
            test.cxx.executeBasedOnFlags([source_path], output_dir, exec_dir,
                                         output_base, [], [], [])

        if rc != 0:
            report = stl.util.makeReport(compile_cmd, out, err, rc)

            if not test.isExpectedToFail():
                report += "Compilation failed unexpectedly!"
            lit_config.note(report)
            return lit.Test.Result(fail_var, report)
        elif exec_file is None:
            report = stl.util.makeReport(compile_cmd, out, err, rc)
            return lit.Test.Result(pass_var, report)

        cmd, out, err, rc = self.executor.run(str(exec_file), [str(exec_file)],
                                              str(exec_dir), [],
                                              test.config.environment)

        report = "Compiled With: '%s'\n" % ' '.join(compile_cmd)
        report += stl.util.makeReport(cmd, out, err, rc)

        if rc == 0:
            return lit.Test.Result(pass_var, report)
        else:
            if not test.isExpectedToFail():
                report += "Compiled test failed unexpectedly!"
            return lit.Test.Result(fail_var, report)

    def _evaluate_fail_test(self, test, lit_config):
        output_base = test.getOutputBaseName()
        output_dir = test.getOutputDir()
        source_path = Path(test.getSourcePath())
        pass_var, fail_var = test.getPassFailResultCodes()

        flags = []
        if test.cxx.name == 'cl' and \
                ('/analyze' in test.cxx.flags or
                 '/analyze' in test.cxx.compile_flags):
            analyze_path = output_dir / (output_base +
                                         '.nativecodeanalysis.xml')
            flags.append('/analyze:log' + str(analyze_path))

        cmd, out, err, rc = test.cxx.compile([source_path], os.devnull, flags)
        report = stl.util.makeReport(cmd, out, err, rc)

        if rc != 0:
            return lit.Test.Result(pass_var, report)
        else:
            if test.isExpectedToFail():
                report += 'Expected compilation to fail!\n'
            return lit.Test.Result(fail_var, report)


class LibcxxTestFormat(STLTestFormat):
    """
    Custom test format handler tor run the libcxx tests for the MSVC STL.
    """
    def getTestsInDirectory(self, testSuite, path_in_suite,
                            litConfig, localConfig, test_class=LibcxxTest):
        return super().getTestsInDirectory(testSuite, path_in_suite, litConfig,
                                           localConfig, test_class)
