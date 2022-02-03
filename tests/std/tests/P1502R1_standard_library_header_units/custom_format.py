# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import os

from stl.test.format import STLTestFormat, TestStep
from stl.test.tests import TestType


# P1502R1_standard_library_header_units/test.cpp cites the definition of "importable C++ library headers".
def getImportableCxxLibraryHeaders():
    return [
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
        'format',
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
        'source_location',
        'span',
        'spanstream',
        'sstream',
        'stack',
        'stdexcept',
        'stop_token',
        'streambuf',
        'string_view',
        'string',
        'strstream',
        'syncstream',
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


class CustomTestFormat(STLTestFormat):
    def getBuildSteps(self, test, litConfig, shared):
        outputDir, outputBase = test.getTempPaths()
        sourcePath = test.getSourcePath()

        compileTestCppWithEdg = '/BE' in test.compileFlags
        if compileTestCppWithEdg:
            test.compileFlags.remove('/BE')

        stlHeaders = getImportableCxxLibraryHeaders()
        exportHeaderOptions = ['/exportHeader', '/headerName:angle', '/Fo', '/MP']
        consumeBuiltHeaderUnits = []
        objFilenames = []
        for hdr in stlHeaders:
            consumeBuiltHeaderUnits += ['/headerUnit:angle', f'{hdr}={hdr}.ifc']
            objFilenames.append(f'{hdr}.obj')

        cmd = [test.cxx, *test.flags, *test.compileFlags, *exportHeaderOptions, *stlHeaders]
        yield TestStep(cmd, shared.execDir, shared.env, False)

        libFilename = 'stl_header_units.lib'
        cmd = ['lib.exe', '/nologo', f'/out:{libFilename}', *objFilenames]
        yield TestStep(cmd, shared.execDir, shared.env, False)

        if compileTestCppWithEdg:
            test.compileFlags.append('/BE')

        if TestType.COMPILE in test.testType:
            cmd = [test.cxx, '/c', sourcePath, *test.flags, *test.compileFlags, *consumeBuiltHeaderUnits]
        elif TestType.RUN in test.testType:
            shared.execFile = f'{outputBase}.exe'
            cmd = [test.cxx, sourcePath, *test.flags, *test.compileFlags, *consumeBuiltHeaderUnits, libFilename,
                    f'/Fe{shared.execFile}', '/link', *test.linkFlags]

        yield TestStep(cmd, shared.execDir, shared.env, False)
