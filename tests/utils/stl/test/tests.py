# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from itertools import chain
from pathlib import Path
from xml.sax.saxutils import quoteattr
import os
import shutil

from lit.Test import FAIL, PASS, ResultCode, Test, UNSUPPORTED, XPASS, XFAIL

from stl.compiler import CXXCompiler

_compiler_path_cache = dict()

SKIP = ResultCode('SKIP', False)


class STLTest(Test):
    def __init__(self, suite, path_in_suite, lit_config, test_config,
                 envlst_entry, env_num, default_cxx, file_path=None):
        self.env_num = env_num
        self.skipped = False
        Test.__init__(self, suite, path_in_suite, test_config, file_path)

        self._configure_expected_result(suite, path_in_suite, lit_config,
                                        test_config, env_num)
        if self.skipped:
            return

        self._configure_cxx(lit_config, envlst_entry, default_cxx)

        # TRANSITION: These configurations should be enabled in the future.
        for flag in chain(self.cxx.flags, self.cxx.compile_flags):
            if flag.startswith('clr:pure', 1):
                self.requires.append('clr_pure')
            elif flag.startswith('BE', 1):
                self.requires.append('edg')

    def getOutputDir(self):
        return Path(os.path.join(
            self.suite.getExecPath(self.path_in_suite[:-1]))) / \
            str(self.env_num)

    def getOutputBaseName(self):
        return self.path_in_suite[-2]

    def getExecDir(self):
        return self.getOutputDir()

    def getExecPath(self):
        return self.getExecDir() / (self.getOutputBaseName() + '.exe')

    def getTestName(self):
        return '/'.join(self.path_in_suite[:-1]) + ":" + str(self.env_num)

    def getFullName(self):
        return self.suite.config.name + ' :: ' + self.getTestName()

    def getPassFailResultCodes(self):
        should_fail = self.isExpectedToFail()
        pass_var = XPASS if should_fail else PASS
        fail_var = XFAIL if should_fail else FAIL

        return pass_var, fail_var

    def getXMLOutputTestName(self):
        return ':'.join((self.path_in_suite[-2], str(self.env_num)))

    def getXMLOutputClassName(self):
        safe_test_path = [x.replace(".", "_") for x in self.path_in_suite[:-1]]
        safe_suite_name = self.suite.name.replace(".", "-")

        if safe_test_path:
            return safe_suite_name + "." + "/".join(safe_test_path)
        else:
            return safe_suite_name + "." + safe_suite_name

    def _configure_expected_result(self, suite, path_in_suite, lit_config,
                                   test_config, env_num):
        test_name = self.getTestName()
        self.expected_result = None

        if test_name in test_config.expected_results:
            self.expected_result = test_config.expected_results[test_name]
        elif test_name in lit_config.expected_results:
            self.expected_result = lit_config.expected_results[test_name]

        if self.expected_result is not None:
            if self.expected_result == SKIP:
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

        self.cxx = CXXCompiler(cxx, flags, compile_flags, link_flags,
                               default_cxx.compile_env)

    # This is partially lifted from lit's Test class. The changes here are to
    # handle skipped tests, our env.lst format, and different naming schemes.
    def writeJUnitXML(self, fil):
        """Write the test's report xml representation to a file handle."""
        test_name = quoteattr(self.getXMLOutputTestName())
        class_name = quoteattr(self.getXMLOutputClassName())

        testcase_template = \
            '<testcase classname={class_name} name={test_name} ' \
            'time="{time:.2f}"'
        elapsed_time = self.result.elapsed if self.result.elapsed else 0.0
        testcase_xml = \
            testcase_template.format(class_name=class_name,
                                     test_name=test_name,
                                     time=elapsed_time)
        fil.write(testcase_xml)

        if self.result.code.isFailure:
            fil.write(">\n\t<failure><![CDATA[")
            if isinstance(self.result.output, str):
                encoded_output = self.result.output
            elif isinstance(self.result.output, bytes):
                encoded_output = self.result.output.decode("utf-8", 'replace')
            # In the unlikely case that the output contains the CDATA
            # terminator we wrap it by creating a new CDATA block
            fil.write(encoded_output.replace("]]>", "]]]]><![CDATA[>"))
            fil.write("]]></failure>\n</testcase>")
        elif self.result.code == UNSUPPORTED:
            unsupported_features = self.getMissingRequiredFeatures()
            if unsupported_features:
                skip_message = \
                    "Skipping because of: " + ", ".join(unsupported_features)
            else:
                skip_message = "Skipping because of configuration."
            skip_message = quoteattr(skip_message)

            fil.write(
                ">\n\t<skipped message={} />\n</testcase>".format(
                    skip_message))
        elif self.result.code == SKIP:
            message = quoteattr('Test is explicitly marked as skipped')
            fil.write(">\n\t<skipped message={} />\n</testcase>".format(
                message))
        else:
            fil.write("/>")


class LibcxxTest(STLTest):
    def getOutputDir(self):
        return Path(
            os.path.join(self.suite.getExecPath(self.path_in_suite))) / \
            str(self.env_num)

    def getOutputBaseName(self):
        output_base = self.path_in_suite[-1]

        if output_base.endswith('.cpp'):
            return output_base[:-4]
        else:
            return output_base

    def getXMLOutputTestName(self):
        return ':'.join((self.path_in_suite[-1], str(self.env_num)))

    def getTestName(self):
        return '/'.join(self.path_in_suite) + ':' + str(self.env_num)
