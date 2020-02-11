import os

from lit.Test import Test

import stl
from stl.compiler import CXXCompiler

_compiler_path_cache = dict()


class StlTest(Test):
    def __init__(self, suite, path_in_suite, lit_config, test_config,
                 envlst_entry, env_num, default_cxx, file_path=None):
        Test.__init__(self, suite, path_in_suite, test_config, file_path)
        self.env_num = env_num
        self.envlst_entry = envlst_entry
        self.lit_config = lit_config
        self._configure_cxx(default_cxx)

        if self.getSourcePath() in test_config.expected_failures:
            self.xfails = ['*']

    def getExecPath(self):
        return '.'.join((Test.getExecPath(self), str(self.env_num)))

    def getFullName(self):
        return '.'.join((Test.getFullName(self), str(self.env_num)))

    def _configure_cxx(self, default_cxx):
        env_compiler = self.envlst_entry.getEnvVal('PM_COMPILER', 'cl')

        if not os.path.isfile(env_compiler):
            cxx = _compiler_path_cache.get(env_compiler, None)

            if cxx is None:
                search_paths = self.config.environment['PATH']
                cxx = stl.util.which(env_compiler, search_paths)
                _compiler_path_cache[env_compiler] = cxx

        if not cxx:
            self.lit_config.fatal('Could not find: %r' % env_compiler)

        flags = list()
        compile_flags = list()
        link_flags = list()

        flags.extend(self.envlst_entry.getEnvVal('PM_CL', '').split())
        link_flags.extend(self.envlst_entry.getEnvVal('PM_LINK', '').split())

        flags.extend(default_cxx.flags or [])
        compile_flags.extend(default_cxx.compile_flags or [])
        link_flags.extend(default_cxx.link_flags or [])

        self.cxx = CXXCompiler(cxx, flags, compile_flags, link_flags)
