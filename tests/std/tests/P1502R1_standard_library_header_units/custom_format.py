# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from pathlib import Path

from stl.test.format import STLTestFormat, TestStep


class CustomTestFormat(STLTestFormat):
    def getBuildSteps(self, test, lit_config, shared):
        shared.exec_dir = test.getExecDir()
        output_base = test.getOutputBaseName()
        output_dir = test.getOutputDir()
        source_path = Path(test.getSourcePath())

        stl_headers = [
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

        compile_test_cpp_with_edg = test.cxx.flags.count('/BE') == 1

        if compile_test_cpp_with_edg:
            test.cxx.flags.remove('/BE')

        header_unit_options = []

        for header in stl_headers:
            header_obj_path = output_dir / (header + '.obj')

            header_unit_options.append('/headerUnit')
            header_unit_options.append('{0}/{1}={1}.ifc'.format(test.config.cxx_headers, header))

            if not compile_test_cpp_with_edg:
                header_unit_options.append(str(header_obj_path))

            cmd, out_files = test.cxx._basicCmd(source_files = [], out = None,
                flags = ['/exportHeader', '<{}>'.format(header), '/Fo{}'.format(str(header_obj_path))],
                skip_mode_flags = True)
            yield TestStep(cmd, shared.exec_dir, [], test.cxx.compile_env)

        if compile_test_cpp_with_edg:
            test.cxx.flags.append('/BE')

        cmd, out_files, shared.exec_file = \
            test.cxx.executeBasedOnFlagsCmd([source_path],
                                            output_dir, shared.exec_dir,
                                            output_base, header_unit_options, [], [])

        yield TestStep(cmd, shared.exec_dir, [source_path],
                       test.cxx.compile_env)
