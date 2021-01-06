# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import itertools
import os

from stl.test.format import STLTestFormat, TestStep
from stl.test.tests import TestType


class CustomTestFormat(STLTestFormat):
    def getBuildSteps(self, test, litConfig, shared):
        stlHeaders = [
            'algorithm',
            'any',
            'array',
            'atomic',
            'barrier',
            'bit',
            'bitset',
            'charconv',
            'chrono',
            'codecvt',
            'compare',
            'complex',
            'concepts',
            'condition_variable',
            'coroutine',
            'deque',
            'exception',
            'execution',
            'filesystem',
            # 'format',
            'forward_list',
            'fstream',
            'functional',
            'future',
            'initializer_list',
            'iomanip',
            'ios',
            'iosfwd',
            'iostream',
            'istream',
            'iterator',
            'latch',
            'limits',
            'list',
            'locale',
            'map',
            'memory_resource',
            'memory',
            'mutex',
            'new',
            'numbers',
            'numeric',
            'optional',
            'ostream',
            'queue',
            'random',
            'ranges',
            'ratio',
            'regex',
            'scoped_allocator',
            'semaphore',
            'set',
            'shared_mutex',
            # 'source_location',
            'span',
            'sstream',
            'stack',
            'stdexcept',
            'stop_token',
            'streambuf',
            'string_view',
            'string',
            'strstream',
            # 'syncstream',
            'system_error',
            'thread',
            'tuple',
            'type_traits',
            'typeindex',
            'typeinfo',
            'unordered_map',
            'unordered_set',
            'utility',
            'valarray',
            'variant',
            'vector',
            'version',
        ]

        outputDir, outputBase = test.getTempPaths()
        sourcePath = test.getSourcePath()

        compileTestCppWithEdg = '/BE' in itertools.chain(test.flags, test.compileFlags)
        if compileTestCppWithEdg:
            test.flags.remove('/BE')
            test.compileFlags.remove('/BE')

        exportHeaderOptions = ['/exportHeader', '/Fo', '/MP']
        headerUnitOptions = []
        for header in stlHeaders:
            headerAbsolutePath = os.path.join(litConfig.cxx_headers, header)

            exportHeaderOptions.append(headerAbsolutePath)

            headerUnitOptions.append('/headerUnit')
            headerUnitOptions.append('{0}={1}.ifc'.format(headerAbsolutePath, header))

            if not compileTestCppWithEdg:
                headerUnitOptions.append(os.path.join(outputDir, header + '.obj'))

        cmd = [test.cxx, *test.flags, *test.compileFlags, *exportHeaderOptions]
        yield TestStep(cmd, shared.execDir, shared.env, False)

        if compileTestCppWithEdg:
            test.compileFlags.append('/BE')

        if TestType.COMPILE in test.testType:
            cmd = [test.cxx, '/c', sourcePath, *test.flags, *test.compileFlags, *headerUnitOptions]
        elif TestType.RUN in test.testType:
            shared.execFile = outputBase + '.exe'
            cmd = [test.cxx, sourcePath, *test.flags, *test.compileFlags, *headerUnitOptions, '/Fe' + shared.execFile,
                   '/link', *test.linkFlags]

        yield TestStep(cmd, shared.execDir, shared.env, False)
