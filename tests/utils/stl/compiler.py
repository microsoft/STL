#===----------------------------------------------------------------------===##
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===##

from itertools import chain
from pathlib import Path
from typing import List
import os


class CXXCompiler:
    CM_Default = 0
    CM_PreProcess = 1
    CM_Compile = 2
    CM_Link = 3
    CM_Analyze = 4

    def __init__(self, path, flags=None, compile_flags=None,
                 link_flags=None, compile_env=None):
        self.path = path
        if path is not None:
            self.name = os.path.basename(path).split('.')[0]
        else:
            self.name = None

        self.compile_flags = compile_flags or []
        self.flags = flags or []
        self.link_flags = link_flags or []

        self.compile_env = compile_env

    def _basicCmd(self, source_files: List[Path], out: Path, cwd: Path,
                  mode=CM_Default, flags=[], compile_flags=[], link_flags=[],
                  skip_mode_flags=False):
        out_files = []
        cmd = []

        if out is not None:
            out_files.append(out)

        cmd.append(self.path)

        if mode == self.CM_PreProcess:
            if out is not None:
                cmd.extend(('/P', '/Fi' + str(out)))
            else:
                cmd.append('/EP')
        elif mode == self.CM_Compile:
            if not skip_mode_flags:
                cmd.append('/c')
            if out is not None and len(source_files) <= 1:
                cmd.append('/Fo' + str(out))
            else:
                for source_file in source_files:
                    out_files.append(
                        cwd / (source_file.name.rsplit('.', 1)[0] + '.obj'))
        elif mode == self.CM_Analyze:
            if not skip_mode_flags:
                cmd.append('/analyze:only')
            if out is not None:
                cmd.append('/analyze:log' + str(out))
            else:
                for source_file in source_files:
                    out_files.append(
                        cwd / (source_file.name.rsplit('.', 1)[0] +
                               '.nativecodeanalysis.xml'))
        elif out is not None:
            cmd.append('/Fe' + str(out))

            if len(source_files) <= 1:
                out_obj = str(out).rsplit('.', 1)[0] + '.obj'
                cmd.append('/Fo' + out_obj)
                out_files.append(Path(out_obj))
            else:
                for source_file in source_files:
                    out_files.append(
                        cwd / (source_file.name.rsplit('.', 1)[0] + '.obj'))

        if mode in (self.CM_Analyze, self.CM_Compile, self.CM_Default):
            cmd.extend(self.compile_flags)
            cmd.extend(compile_flags)

        cmd.extend(self.flags)
        cmd.extend(flags)
        cmd.extend([str(file) for file in source_files])

        if mode in (self.CM_Default, self.CM_Link):
            cmd.append('/link')
            cmd.extend(self.link_flags)
            cmd.extend(link_flags)

        return cmd, out_files

    def executeBasedOnFlagsCmd(self, source_files, out_dir, cwd,
                               out_base=None, flags=[], compile_flags=[],
                               link_flags=[]):
        mode = self.CM_Default
        exec_file = None
        output_file = None

        if out_base is None:
            out_base = source_files[0].name.rsplit('.', 1)[0]

        add_analyze_output = False
        for flag in chain(flags, self.flags):
            flag = flag[1:]

            if flag == 'c':
                mode = self.CM_Compile
                exec_file = None
                output_file = out_dir / (out_base + '.obj')
            elif flag.startswith('Fe'):
                output_file = Path(flag[2:])
                exec_file = output_file
            elif flag == 'analyze:only':
                mode = self.CM_Analyze
                output_file = out_dir / (out_base + '.nativecodeanalysis.xml')
                exec_file = None
            elif flag.startswith('analyze') and flag[-1] != '-':
                add_analyze_output = True

        if mode is self.CM_Default and output_file is None:
            output_file = out_dir / (out_base + '.exe')
            exec_file = output_file

        if add_analyze_output and mode != self.CM_Analyze:
            flags.append('/analyze:log' +
                         str(out_dir / (out_base + '.nativecodeanalysis.xml')))

        cmd, out_files = self._basicCmd(source_files, output_file, cwd, mode,
                                        flags, compile_flags, link_flags, True)
        return cmd, out_files, exec_file
