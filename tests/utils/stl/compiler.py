#===----------------------------------------------------------------------===##
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===##

import os

import stl.util


class CXXCompiler(object):
    CM_Default = 0
    CM_PreProcess = 1
    CM_Compile = 2
    CM_Link = 3

    def __init__(self, path, flags=None, compile_flags=None,
                 link_flags=None, compile_env=None):
        self.path = path
        if path:
            self.name = os.path.basename(path).split('.')[0]
        else:
            self.name = None

        self.compile_flags = compile_flags or []
        self.flags = flags or []
        self.link_flags = link_flags or []

        if compile_env is not None:
            self.compile_env = dict(compile_env)
        else:
            self.compile_env = None

    def _basicCmd(self, source_files, out, mode=CM_Default, flags=[]):
        cmd = []
        cmd += [self.path]
        if mode == self.CM_PreProcess:
            if out is not None:
                cmd += ['/P', '/Fi' + out]
            else:
                cmd += ['/EP']
        elif mode == self.CM_Compile:
            cmd += ['/c']
            if out is not None:
                cmd += ['/Fo' + out]
        elif out is not None:
            cmd += ['/Fe' + out]

        if mode != self.CM_Link:
            cmd += self.compile_flags
        cmd += self.flags
        cmd += flags
        if isinstance(source_files, list):
            cmd += source_files
        elif isinstance(source_files, str):
            cmd += [source_files]
        else:
            raise TypeError('source_files must be a string or list')

        if mode != self.CM_PreProcess and mode != self.CM_Compile:
            cmd += ['/link']
            cmd += self.link_flags
        return cmd

    def preprocessCmd(self, source_files, out=None, flags=[]):
        return self._basicCmd(source_files, out, flags=flags,
                              mode=self.CM_PreProcess)

    def compileCmd(self, source_files, out=None, flags=[]):
        return self._basicCmd(source_files, out, flags=flags,
                              mode=self.CM_Compile)

    def linkCmd(self, source_files, out=None, flags=[]):
        return self._basicCmd(source_files, out, flags=flags,
                              mode=self.CM_Link)

    def compileLinkCmd(self, source_files, out=None, flags=[]):
        return self._basicCmd(source_files, out, flags=flags)

    def preprocess(self, source_files, out=None, flags=[], cwd=None):
        cmd = self.preprocessCmd(source_files, out, flags)
        out, err, rc = stl.util.executeCommand(cmd, env=self.compile_env,
                                               cwd=cwd)
        return cmd, out, err, rc

    def compile(self, source_files, out=None, flags=[], cwd=None):
        cmd = self.compileCmd(source_files, out, flags)
        out, err, rc = stl.util.executeCommand(cmd, env=self.compile_env,
                                               cwd=cwd)
        return cmd, out, err, rc

    def link(self, source_files, exec_path=None, flags=[], cwd=None):
        cmd = self.linkCmd(source_files, exec_path, flags)
        out, err, rc = stl.util.executeCommand(cmd, env=self.compile_env,
                                               cwd=cwd)
        return cmd, out, err, rc

    def compileLink(self, source_files, exec_path=None, flags=[],
                    cwd=None):
        cmd = self.compileLinkCmd(source_files, exec_path, flags)
        out, err, rc = stl.util.executeCommand(cmd, env=self.compile_env,
                                               cwd=cwd)
        return cmd, out, err, rc

    def compileLinkTwoSteps(self, source_file, out=None, object_file=None,
                            flags=[], cwd=None):
        if not isinstance(source_file, str):
            raise TypeError('This function only accepts a single input file')
        if object_file is None:
            # Create, use and delete a temporary object file if none is given.
            def with_fn(): return stl.util.guardedTempFilename(suffix='.o')
        else:
            # Otherwise wrap the filename in a context manager function.
            def with_fn(): return stl.util.nullContext(object_file)
        with with_fn() as object_file:
            cc_cmd, cc_stdout, cc_stderr, rc = self.compile(
                source_file, object_file, flags=flags, cwd=cwd)
            if rc != 0:
                return cc_cmd, cc_stdout, cc_stderr, rc

            link_cmd, link_stdout, link_stderr, rc = self.link(
                object_file, exec_path=out, flags=flags, cwd=cwd)
            return (cc_cmd + ['&'] + link_cmd, cc_stdout + link_stdout,
                    cc_stderr + link_stderr, rc)
