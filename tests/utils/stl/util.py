#===----------------------------------------------------------------------===##
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===##

from contextlib import contextmanager
from pathlib import Path
import os
import platform
import signal
import subprocess
import sys
import tempfile
import threading


@contextmanager
def guardedTempFilename(suffix='', prefix='', dir=None):
    # Creates and yields a temporary filename within a with statement. The file
    # is removed upon scope exit.
    handle, name = tempfile.mkstemp(suffix=suffix, prefix=prefix, dir=dir)
    os.close(handle)
    yield name
    Path(name).unlink(True)


@contextmanager
def guardedFilename(name):
    # Yields a filename within a with statement. The file is removed upon scope
    # exit.
    yield name
    Path(name).unlink(True)


@contextmanager
def nullContext(value):
    # Yields a variable within a with statement. No action is taken upon scope
    # exit.
    yield value


def makeReport(cmd, out, err, rc):
    report = "Command: \"%s\"\n" % "\" \"".join(cmd)
    report += "Exit Code: %d\n" % rc
    # Replacing CRLFs with LFs avoids ugly double newlines when this is displayed in Azure Pipelines.
    if out:
        report += "Standard Output:\n--\n%s--\n" % out.replace("\r\n", "\n")
    if err:
        report += "Standard Error:\n--\n%s--\n" % err.replace("\r\n", "\n")
    report += '\n'
    return report


class ExecuteCommandTimeoutException(Exception):
    def __init__(self, msg, out, err, exitCode):
        assert isinstance(msg, str)
        assert isinstance(out, str)
        assert isinstance(err, str)
        assert isinstance(exitCode, int)
        self.msg = msg
        self.out = out
        self.err = err
        self.exitCode = exitCode


# Close extra file handles on UNIX (on Windows this cannot be done while
# also redirecting input).
kUseCloseFDs = not (platform.system() == 'Windows')


def decodeOutput(bytes):
    # MSVC's output is encoded in the active code page
    # EDG's (`cl /BE`) output is encoded in UTF-8
    try:
        return bytes.decode()
    except UnicodeError:
        import locale
        return bytes.decode(locale.getpreferredencoding(do_setlocale=False))


def executeCommand(command, cwd=None, env=None, input=None, timeout=0):
    """
        Execute command ``command`` (list of arguments or string)
        with
        * working directory ``cwd`` (str), use None to use the current
          working directory
        * environment ``env`` (dict), use None for none
        * Input to the command ``input`` (str), use string to pass
          no input.
        * Max execution time ``timeout`` (int) seconds. Use 0 for no timeout.

        Returns a tuple (out, err, exitCode) where
        * ``out`` (str) is the standard output of running the command
        * ``err`` (str) is the standard error of running the command
        * ``exitCode`` (int) is the exitCode of running the command

        If the timeout is hit an ``ExecuteCommandTimeoutException``
        is raised.
    """
    if input is not None:
        input = input.encode()

    p = subprocess.Popen(command, cwd=cwd,
                         stdin=subprocess.PIPE,
                         stdout=subprocess.PIPE,
                         stderr=subprocess.PIPE,
                         env=env, close_fds=kUseCloseFDs)
    timerObject = None
    hitTimeOut = False
    try:
        if timeout > 0:
            def killProcess():
                # We may be invoking a shell so we need to kill the
                # process and all its children.
                nonlocal hitTimeOut
                hitTimeOut = True
                killProcessAndChildren(p.pid)

            timerObject = threading.Timer(timeout, killProcess)
            timerObject.start()

        out, err = p.communicate(input=input)
        exitCode = p.wait()
    finally:
        if timerObject is not None:
            timerObject.cancel()

    # Ensure the resulting output is always of string type.
    out = decodeOutput(out)
    err = decodeOutput(err)

    if hitTimeOut:
        raise ExecuteCommandTimeoutException(
            msg='Reached timeout of {} seconds'.format(timeout),
            out=out,
            err=err,
            exitCode=exitCode
            )

    # Detect Ctrl-C in subprocess.
    if exitCode == -signal.SIGINT:
        raise KeyboardInterrupt

    return out, err, exitCode


def killProcessAndChildren(pid):
    """
    This function kills a process with ``pid`` and all its
    running children (recursively). It is currently implemented
    using the psutil module which provides a simple platform
    neutral implementation.
    """
    if platform.system() == 'AIX':
        subprocess.call('kill -kill $(ps -o pid= -L{})'.format(pid),
                        shell=True)
    else:
        import psutil
        try:
            psutilProc = psutil.Process(pid)
            # Handle the different psutil API versions
            try:
                # psutil >= 2.x
                children_iterator = psutilProc.children(recursive=True)
            except AttributeError:
                # psutil 1.x
                children_iterator = psutilProc.get_children(recursive=True)
            for child in children_iterator:
                try:
                    child.kill()
                except psutil.NoSuchProcess:
                    pass
            psutilProc.kill()
        except psutil.NoSuchProcess:
            pass


def executeCommandVerbose(cmd, *args, **kwargs):
    """
    Execute a command and print its output on failure.
    """
    out, err, exitCode = executeCommand(cmd, *args, **kwargs)
    if exitCode != 0:
        report = makeReport(cmd, out, err, exitCode)
        report += "\n\nFailed!"
        sys.stderr.write('%s\n' % report)
    return out, err, exitCode
