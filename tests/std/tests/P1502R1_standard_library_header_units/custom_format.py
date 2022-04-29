# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import json
import os
import re

from stl.test.format import STLTestFormat, TestStep
from stl.test.tests import TestType


# Print noisy progress messages that are useful when working on this script.
noisyProgress = False


def loadJsonWithComments(filename):
    # This is far from a general-purpose solution (it doesn't attempt to handle block comments like /**/
    # and comments appearing within strings like "cats // dogs"), but it's sufficient for our purposes.
    comment = re.compile('//.*')
    with open(filename) as file:
        replacedLines = [re.sub(comment, '', line) for line in file]
        return json.loads(''.join(replacedLines))


def getAllHeaders(headerUnitsJsonFilename):
    buildAsHeaderUnits = loadJsonWithComments(headerUnitsJsonFilename)['BuildAsHeaderUnits']

    # We want to build everything that's mentioned in header-units.json, plus all of the
    # headers that were commented out for providing macros that control header inclusion.
    return buildAsHeaderUnits + ['version', 'yvals.h', 'yvals_core.h']


def getImportableCxxLibraryHeaders(sourcePath):
    # This JSON With Comments file is shared between Python and Perl,
    # reducing the number of things we need to update when adding new Standard headers.
    jsonFilename = os.path.join(os.path.dirname(sourcePath), 'importable_cxx_library_headers.jsonc')
    return loadJsonWithComments(jsonFilename)


class CustomTestFormat(STLTestFormat):
    def getBuildSteps(self, test, litConfig, shared):
        outputDir, outputBase = test.getTempPaths()
        sourcePath = test.getSourcePath()

        compileTestCppWithEdg = '/BE' in test.compileFlags
        if compileTestCppWithEdg:
            test.compileFlags.remove('/BE')

        # This is a list of compiler options to consume the header units that we've built so far.
        consumeBuiltHeaderUnits = []

        # Output files:
        objFilenames = []

        if '/DTEST_TOPO_SORT' in test.compileFlags: # Build deduplicated header units:
            # Compiler options, common to both scanning dependencies and building header units.
            clOptions = ['/exportHeader', '/headerName:angle', '/translateInclude', '/Fo', '/MP']

            # Store the list of headers to build.
            allHeaders = getAllHeaders(os.path.join(litConfig.cxx_headers, 'header-units.json'))

            # Generate JSON files that record how these headers depend on one another.
            if noisyProgress:
                print('Scanning dependencies...')
            cmd = [test.cxx, *test.flags, *test.compileFlags, *clOptions, '/scanDependencies', '.\\', *allHeaders]
            yield TestStep(cmd, shared.execDir, shared.env, False)

            # The JSON files also record what object files will be produced.
            # IFC filenames and OBJ filenames follow different patterns. For example:
            # <filesystem> produces filesystem.ifc and filesystem.obj
            # <xbit_ops.h> produces xbit_ops.h.ifc and xbit_ops.obj
            # We can easily synthesize IFC filenames, but it's easier to get the OBJ filenames from the JSON files.

            # This dictionary powers the topological sort.
            # Key: Header name, e.g. 'bitset'.
            # Value: List of dependencies that remain to be built, e.g. ['iosfwd', 'limits', 'xstring'].
            remainingDependencies = {}

            # Read the JSON files, storing the results in objFilenames and remainingDependencies.
            for hdr in allHeaders:
                with open(os.path.join(outputDir, f'{hdr}.module.json')) as file:
                    jsonObject = json.load(file)
                    objFilenames.append(jsonObject['rules'][0]['primary-output'])
                    remainingDependencies[hdr] = [req['logical-name'] for req in jsonObject['rules'][0]['requires']]

            # Build header units in topologically sorted order.
            while len(remainingDependencies) > 0:
                # When a header has no remaining dependencies, it is ready to be built.
                readyToBuild = [hdr for hdr, dep in remainingDependencies.items() if len(dep) == 0]

                # Each layer can be built in parallel.
                if noisyProgress:
                    print('Building deduplicated header units:', ' '.join(readyToBuild))
                cmd = [test.cxx, *test.flags, *test.compileFlags, *clOptions, *consumeBuiltHeaderUnits, *readyToBuild]
                yield TestStep(cmd, shared.execDir, shared.env, False)

                # Update remainingDependencies by doing two things.
                # hdr, dep is the current key-value pair.
                # First, keep `if len(dep) > 0`. (Otherwise, we just built hdr.)
                # Second, filter dep, eliminating anything that appears in readyToBuild. (If we're left with
                # an empty list, then hdr will be ready to build in the next iteration.)
                remainingDependencies = {
                    hdr: [d for d in dep if d not in readyToBuild]
                    for hdr, dep in remainingDependencies.items() if len(dep) > 0
                }

                # Add compiler options to consume the header units that we just built.
                for hdr in readyToBuild:
                    consumeBuiltHeaderUnits += ['/headerUnit:angle', f'{hdr}={hdr}.ifc']
        else: # Build independent header units:
            stlHeaders = getImportableCxxLibraryHeaders(sourcePath)
            exportHeaderOptions = ['/exportHeader', '/headerName:angle', '/Fo', '/MP']
            for hdr in stlHeaders:
                consumeBuiltHeaderUnits += ['/headerUnit:angle', f'{hdr}={hdr}.ifc']
                objFilenames.append(f'{hdr}.obj')

            if noisyProgress:
                print('Building independent header units...')
            cmd = [test.cxx, *test.flags, *test.compileFlags, *exportHeaderOptions, *stlHeaders]
            yield TestStep(cmd, shared.execDir, shared.env, False)

        # For convenience, create a library file containing all of the object files that were produced.
        libFilename = 'stl_header_units.lib'
        if noisyProgress:
            print('Creating library...')
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

        if noisyProgress:
            print('Compiling and running test...')
        yield TestStep(cmd, shared.execDir, shared.env, False)
