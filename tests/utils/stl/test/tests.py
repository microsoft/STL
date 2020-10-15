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
from xml.sax.saxutils import quoteattr
import os
import shutil

from lit.Test import FAIL, PASS, SKIPPED, Test, UNSUPPORTED, XPASS, XFAIL

from stl.compiler import CXXCompiler

_compiler_path_cache = dict()


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

        use_edg = False
        for flag in chain(self.cxx.flags, self.cxx.compile_flags):
            if flag[1:] == 'clr:pure':
                self.requires.append('clr_pure') # TRANSITION, GH-798
            elif flag[1:] == 'clr':
                self.requires.append('clr') # TRANSITION, GH-797
            elif flag[1:] == 'BE':
                self.requires.append('edg') # available for x86, see config.py
                use_edg = True

        if not use_edg and self.cxx.edg_drop is not None:
            self.skipped = True

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

    def getXMLOutputTestName(self):
        return ':'.join((self.path_in_suite[-2], self.env_num))

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

        self.cxx = CXXCompiler(cxx, flags, compile_flags, link_flags,
                               default_cxx.compile_env, default_cxx.edg_drop)

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
        elif self.result.code == SKIPPED:
            message = quoteattr('Test is explicitly marked as skipped')
            fil.write(">\n\t<skipped message={} />\n</testcase>".format(
                message))
        else:
            fil.write("/>")


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

    def getXMLOutputTestName(self):
        return ':'.join((self.path_in_suite[-1], self.env_num))

    def getTestName(self):
        return '/'.join(self.path_in_suite) + ':' + self.env_num
