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
import os
import re
import shutil
import sys

import lit

from stl.test.tests import LibcxxTest, STLTest, TestType
import stl.test.file_parsing
import stl.util


@dataclass
class TestStep:
    cmd: List[str] = field(default_factory=list)
    workDir: os.PathLike = field(default=Path('.'))
    env: Dict[str, str] = field(default_factory=dict)
    shouldFail: bool = field(default=False)

def _mergeEnvironments(currentEnv, otherEnv):
    """Merges two execution environments.

    If both environments contain PATH variables, they are also merged
    using the proper separator.
    """
    resultEnv = dict(currentEnv)
    for k, v in otherEnv.items():
        if k == 'PATH':
            oldPath = currentEnv.get(k, '')
            if oldPath != '':
                resultEnv[k] = ';'.join((oldPath, v))
            else:
                resultEnv[k] = v
        else:
            resultEnv[k] = v

    return resultEnv

def _getEnvLst(sourcePath, localConfig):
    envlstPath = getattr(localConfig, 'envlst_path', None)
    if envlstPath is None:
        cwd = Path(sourcePath)
        if (cwd / 'env.lst').is_file():
            envlstPath = cwd / 'env.lst'
        else:
            for parent in cwd.parents:
                if (parent / 'env.lst').is_file():
                    envlstPath = parent / 'env.lst'
                    break

    return envlstPath

def _isLegalDirectory(sourcePath, test_subdirs):
    for prefix in test_subdirs:
        common = os.path.normpath(os.path.commonpath((sourcePath, prefix)))
        if common == sourcePath or common == prefix:
            return True

    return False


class STLTestFormat:
    """
    Custom test format handler to run MSVC tests.
    """
    def getTestsInDirectory(self, testSuite, pathInSuite, litConfig, localConfig, testClass=STLTest):
        sourcePath = testSuite.getSourcePath(pathInSuite)
        if not _isLegalDirectory(sourcePath, litConfig.test_subdirs[localConfig.name]):
            return

        envlstPath = _getEnvLst(sourcePath, localConfig)

        envEntries = None
        if envlstPath is not None:
            envEntries = stl.test.file_parsing.parse_env_lst_file(envlstPath)
            formatString = '{:0' + str(len(str(len(envEntries)))) + 'd}'

        sourcePath = testSuite.getSourcePath(pathInSuite)
        for filename in os.listdir(sourcePath):
            # Ignore dot files and excluded tests.
            if filename.startswith('.') or filename in localConfig.excludes:
                continue

            filepath = os.path.join(sourcePath, filename)
            if not os.path.isdir(filepath):
                if any([re.search(ext, filename) for ext in localConfig.suffixes]):
                    if envEntries is None:
                        litConfig.fatal("Could not find an env.lst file.")

                    for envEntry, envNum in zip(envEntries, itertools.count()):
                        yield testClass(testSuite, pathInSuite + (filename,),
                                        litConfig, localConfig, envEntry,
                                        formatString.format(envNum))

    def getIsenseRspFileSteps(self, test, litConfig, shared):
        if litConfig.edg_drop is not None and test.isenseRspPath is not None:
            with open(test.isenseRspPath) as f:
                cmd = [line.strip() for line in f]
            cmd[0] = litConfig.edg_drop

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

            yield TestStep(cmd, shared.execDir, shared.env, False)

    def runStep(self, testStep, litConfig):
        if str(testStep.workDir) == '.':
            testStep.workDir = os.getcwd()

        env = _mergeEnvironments(os.environ, testStep.env)

        return testStep.cmd, *stl.util.executeCommand(testStep.cmd, cwd=testStep.workDir, env=env)

    def getStages(self, test, litConfig):
        @dataclass
        class SharedState:
            execFile: Optional[os.PathLike] = field(default=None)
            execDir: os.PathLike = field(default_factory=Path)
            env: Dict[str, str] = field(default_factory=dict)

        execDir, _ = test.getTempPaths()
        shared = SharedState(None, execDir, copy.deepcopy(litConfig.test_env))
        shared.env['TMP'] = execDir
        shared.env['TEMP'] = execDir
        shared.env['TMPDIR'] = execDir
        shared.env['TEMPDIR'] = execDir

        return [
            ('Build setup', self.getBuildSetupSteps(test, litConfig, shared), True),
            ('Build', self.getBuildSteps(test, litConfig, shared), True),
            ('Intellisense response file', self.getIsenseRspFileSteps(test, litConfig, shared), False),
            ('Test setup', self.getTestSetupSteps(test, litConfig, shared), False),
            ('Test', self.getTestSteps(test, litConfig, shared), False)]

    def getBuildSetupSteps(self, test, litConfig, shared):
        shutil.rmtree(shared.execDir, ignore_errors=True)
        Path(shared.execDir).mkdir(parents=True, exist_ok=True)

        # Makes this function a generator which yields nothing
        yield from []

    def getBuildSteps(self, test, litConfig, shared):
        _, tmpBase = test.getTempPaths()

        shouldFail = TestType.FAIL in test.testType
        if TestType.COMPILE in test.testType:
            cmd = [test.cxx, '-c', test.getSourcePath(), *test.flags, *test.compileFlags]
            yield TestStep(cmd, shared.execDir, shared.env, shouldFail)
        elif TestType.LINK in test.testType or \
                ('clang' in test.config.available_features and 'asan' in test.config.available_features):
            objFile = tmpBase + '.o'
            cmd = [test.cxx, '-c', test.getSourcePath(), *test.flags, *test.compileFlags, '-Fo' + objFile]
            yield TestStep(cmd, shared.execDir, shared.env, False)

            shared.execFile = tmpBase + '.exe'
            cmd = ['link.exe', objFile, *test.flags, '-out:' + shared.execFile, *test.linkFlags]
            yield TestStep(cmd, shared.execDir, shared.env, shouldFail)
        elif TestType.RUN in test.testType:
            shared.execFile = tmpBase + '.exe'
            cmd = [test.cxx, test.getSourcePath(), *test.flags, *test.compileFlags,
                   '-Fe' + shared.execFile, '-link', *test.linkFlags]
            yield TestStep(cmd, shared.execDir, shared.env, False)

    def getTestSetupSteps(self, test, litConfig, shared):
        if TestType.RUN in test.testType:
            for dependency in test.fileDependencies:
                if not os.path.isabs(dependency):
                    dependency = os.path.join(os.path.dirname(test.getSourcePath()), dependency)
                shutil.copy2(dependency, os.path.join(shared.execDir, os.path.basename(dependency)))

        yield from []

    def getTestSteps(self, test, litConfig, shared):
        if not TestType.RUN in test.testType:
            yield from []
            return

        shouldFail = TestType.FAIL in test.testType
        yield TestStep([shared.execFile], shared.execDir, shared.env, shouldFail)

    def execute(self, test, litConfig):
        try:
            result = test.configureTest(litConfig)
            if result:
                return result

            # This test is expected to fail at some point, but we're not sure if
            # it should fail during the build phase or the test phase.
            someFail = test.expectedResult and test.expectedResult.isFailure

            stages = self.getStages(test, litConfig)

            report = ''
            for stageName, steps, isBuildStep in stages:
                if not isBuildStep and litConfig.build_only:
                    continue

                report += stageName + ' steps:\n'
                for step in steps:
                    cmd, out, err, rc = self.runStep(step, litConfig)

                    if step.shouldFail and rc == 0:
                        report += stageName + ' step succeeded unexpectedly.\n'
                    elif rc != 0:
                        report += stageName + ' step failed unexpectedly.\n'

                    report += stl.util.makeReport(cmd, out, err, rc)
                    if (step.shouldFail and rc == 0) or (not step.shouldFail and rc != 0):
                        if someFail:
                            test.xfails = ['*']
                        return (lit.Test.FAIL, report)

            return (lit.Test.PASS, '')

        except Exception as e:
            _, _, exception_traceback = sys.exc_info()
            filename = exception_traceback.tb_frame.f_code.co_filename
            line_number = exception_traceback.tb_lineno
            litConfig.error(repr(e) + ' at ' + filename + ':' + str(line_number))


class LibcxxTestFormat(STLTestFormat):
    """
    Custom test format handler to run the libcxx tests for the MSVC STL.
    """
    def getTestsInDirectory(self, testSuite, pathInSuite,
                            litConfig, localConfig, testClass=LibcxxTest):
        return super().getTestsInDirectory(testSuite, pathInSuite, litConfig,
                                           localConfig, testClass)

    def addCompileFlags(self, *args):
        # For now, this is necessary to discard the value of
        # `LIBCXX_FILESYSTEM_STATIC_TEST_ROOT` which we don't care about;
        # eventually it will probably need to be made meaningful.
        pass
