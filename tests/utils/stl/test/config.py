#===----------------------------------------------------------------------===##
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===##

from pathlib import Path
import os
import platform
import shlex

from stl.test.executor import LocalExecutor
from stl.compiler import CXXCompiler
import stl.util
import stl.test.file_parsing
import stl.test.target_info


def loadSiteConfig(lit_config, config, param_name, env_name):
    # We haven't loaded the site specific configuration (the user is
    # probably trying to run on a test file directly, and either the site
    # configuration hasn't been created by the build system, or we are in an
    # out-of-tree build situation).
    site_cfg = lit_config.params.get(param_name,
                                     os.environ.get(env_name))
    if not site_cfg:
        lit_config.warning('No site specific configuration file found!'
                           ' Running the tests in the default configuration.')
    elif not os.path.isfile(site_cfg):
        lit_config.fatal(
            "Specified site configuration file does not exist: '%s'" %
            site_cfg)
    else:
        lit_config.note('using site specific configuration at %s' % site_cfg)
        ld_fn = lit_config.load_config

        # Null out the load_config function so that lit.site.cfg doesn't
        # recursively load a config even if it tries.
        # TODO: This is one hell of a hack. Fix it.
        def prevent_reload_fn(*args, **kwargs):
            pass
        lit_config.load_config = prevent_reload_fn
        ld_fn(config, site_cfg)
        lit_config.load_config = ld_fn


# Extract the value of a numeric macro such as __cplusplus or a feature-test
# macro.
def intMacroValue(token):
    return int(token.rstrip('LlUu'))


class Configuration(object):
    # pylint: disable=redefined-outer-name
    def __init__(self, lit_config, config):
        self.config = config
        self.cxx_headers = None
        self.cxx_library_root = None
        self.cxx_runtime_root = None
        self.default_compiler = None
        self.execute_external = False
        self.executor = None
        self.expected_failures_list_path = None
        self.expected_failures_list_root = None
        self.lit_config = lit_config
        self.link_shared = True
        self.long_tests = None
        self.msvc_toolset_libs_root = None
        self.skip_list_path = None
        self.skip_list_root = None
        self.stl_build_root = None
        self.stl_compilation_env = None
        self.stl_inc_env_var = None
        self.stl_lib_env_var = None
        self.stl_path_env_var = None
        self.stl_src_root = None
        self.target_arch = None
        self.target_info = stl.test.target_info.WindowsLocalTI(lit_config)

        # TODO: Move this into configure like everything else
        self.config.test_source_root =\
            getattr(self.config, 'libcxx_test_source_root',
                    self.config.test_source_root)

    def get_lit_conf(self, name, default=None):
        val = self.lit_config.params.get(name, None)
        if val is None:
            val = getattr(self.config, name, None)
            if val is None:
                val = default
        return val

    def get_lit_bool(self, name, default=None, env_var=None):
        def check_value(value, var_name):
            if value is None:
                return default
            if isinstance(value, bool):
                return value
            if not isinstance(value, str):
                raise TypeError('expected bool or string')
            if value.lower() in ('1', 'true'):
                return True
            if value.lower() in ('', '0', 'false'):
                return False
            self.lit_config.fatal(
                "parameter '{}' should be true or false".format(var_name))

        conf_val = self.get_lit_conf(name)
        if env_var is not None and env_var in os.environ and \
                os.environ[env_var] is not None:
            val = os.environ[env_var]
            if conf_val is not None:
                self.lit_config.warning(
                    'Environment variable %s=%s is overriding explicit '
                    '--param=%s=%s' % (env_var, val, name, conf_val))
            return check_value(val, env_var)
        return check_value(conf_val, name)

    def configure(self):
        self.configure_features()
        self.configure_default_compiler()
        self.configure_excludes()
        self.configure_executor()
        self.configure_expected_failures()

    # TODO: Don't hard-code features.
    # TODO: Confirm theses are the only features we need to run the tests we
    # want.
    def configure_features(self):
        self.config.available_features.add('long_tests')
        self.config.available_features.add('c++2a')
        self.config.available_features.update(
                self.target_info.features)

    def configure_test_src_root(self):
        test_src_root = self.get_lit_conf('test_source_root', None)

        if test_src_root is not None:
            self.test_src_root = Path(test_src_root)
            return True

    def configure_src_root(self):
        stl_src_root = self.get_lit_conf('stl_src_root', None)

        if stl_src_root is None:
            if self.test_src_root is None:
                if not self.configure_test_src_root():
                    self.lit_config.fatal(
                        "Could not infer stl_src_root from test_source_root. "
                        "test_source_root is None")

            if self.name == 'libc++':
                stl_src_root = self.test_src_root.parents[2]
            else:
                stl_src_root = self.test_src_root.parents[1]

            self.lit_config.note(
                    "stl_src_root was not specified. Defaulting to: %s." %
                    str(stl_src_root))

        self.stl_src_root = Path(stl_src_root)

    def configure_target_architecture(self):
        target_arch = self.get_lit_conf('target_arch', None)

        if target_arch is None:
            target_arch = platform.machine()

            self.lit_config.note(
                    "target_arch was not specified. Defaulting to %s." %
                    target_arch)

        self.target_arch = target_arch
        self.config.target_arch = target_arch

    def configure_build_root(self):
        stl_build_root = self.get_lit_conf('stl_build_root', None)

        if stl_build_root is None:
            if self.stl_src_root is None:
                self.configure_src_root()

            if self.target_arch is None:
                self.configure_target_architecture()

            stl_build_root = self.stl_src_root / 'out' / 'build'

            if self.target_arch.casefold() == 'AMD64'.casefold():
                stl_build_root = stl_build_root / 'x64' / 'out'
            elif self.target_arch.casefold() == 'X86'.casefold():
                stl_build_root = stl_build_root / 'i386' / 'out'
            else:
                stl_build_root =\
                    stl_build_root / self.target_arch.upper() / 'out'

            self.lit_config.note(
                    "stl_build_root was not specified. Defaulting to: %s." %
                    str(stl_build_root))

        self.stl_build_root = Path(stl_build_root)

    def configure_library_roots(self):
        cxx_runtime_root = self.get_lit_conf('cxx_runtime_root', None)
        cxx_library_root = self.get_lit_conf('cxx_library_root', None)

        if cxx_runtime_root is None or cxx_library_root is None:
            if self.stl_build_root is None:
                self.configure_build_root()

            if self.target_arch is None:
                self.configure_target_architecture()

            subfolder_name = None
            if self.target_arch.casefold() == 'AMD64'.casefold():
                subfolder_name = 'amd64'
            elif self.target_arch.casefold() == 'X86'.casefold():
                subfolder_name = 'i386'
            else:
                subfolder_name = self.target_arch.lower()

            if cxx_runtime_root is None:
                cxx_runtime_root = self.stl_build_root / 'bin' / subfolder_name

                self.lit_config.note(
                    "cxx_runtime_root was not specified. Defaulting to: %s." %
                    str(cxx_runtime_root))

            if cxx_library_root is None:
                cxx_library_root =\
                    self.stl_build_root / 'lib' / subfolder_name

                self.lit_config.note(
                    "cxx_library_root was not specified. Defaulting to: %s."
                    % str(cxx_library_root))

        self.cxx_runtime_root = cxx_runtime_root
        self.cxx_library_root = cxx_library_root

    # We only do this because of a bug with force includes not respecting /I
    def configure_inc_env_var(self):
        stl_inc_env_var = self.get_lit_conf('stl_inc_env_var', None)

        if stl_inc_env_var is None:
            stl_inc_env_var = self.config.environment.get('INCLUDE', '')

            if self.cxx_headers is None:
                self.configure_cxx_headers()

            if stl_inc_env_var != '':
                stl_inc_env_var = ';'.join((str(self.cxx_headers),
                                            stl_inc_env_var))
            else:
                stl_inc_env_var = self.stl_inc_env_var

        self.stl_inc_env_var = stl_inc_env_var

    # Note: This relies on kernel32.lib and ucrt.lib being in the LIB env var
    def configure_lib_env_var(self):
        stl_lib_env_var = self.get_lit_conf('stl_lib_env_var', None)

        if stl_lib_env_var is None:
            stl_lib_env_var = self.config.environment.get('LIB', '')

            if stl_lib_env_var != '':
                stl_lib_env_var = ';'.join((str(self.cxx_library_root),
                                            stl_lib_env_var))
            else:
                stl_lib_env_var = self.cxx_library_root

        self.stl_lib_env_var = stl_lib_env_var

    def configure_path_env_var(self):
        stl_path_env_var = self.get_lit_conf('stl_path_env_var', None)

        if stl_path_env_var is None:
            stl_path_env_var = self.config.environment.get('PATH', '')

            if self.cxx_runtime_root is None:
                self.configure_library_roots()

            if stl_path_env_var != '':
                stl_path_env_var = ';'.join((str(self.cxx_runtime_root),
                                            stl_path_env_var))
            else:
                stl_path_env_var = self.cxx_runtime_root

        self.stl_path_env_var = stl_path_env_var

    def configure_compilation_env(self):
        stl_compilation_env = self.get_lit_conf('stl_compilation_env', None)

        if stl_compilation_env is None:
            stl_compilation_env = dict(self.config.environment)

        if self.stl_lib_env_var is None:
            self.configure_inc_env_var()

        if self.stl_lib_env_var is None:
            self.configure_lib_env_var()

        if self.stl_path_env_var is None:
            self.configure_path_env_var()

        stl_compilation_env['INCLUDE'] = self.stl_inc_env_var
        stl_compilation_env['LIB'] = self.stl_lib_env_var
        stl_compilation_env['PATH'] = self.stl_path_env_var

        self.stl_compilation_env = stl_compilation_env

    def configure_skip_list_location(self):
        skip_list_path = self.get_lit_conf('skip_list_path', None)

        if skip_list_path is not None:
            self.skip_list_path = Path(skip_list_path)
        else:
            self.skip_list_path = Path(os.devnull)

    def configure_skip_list_root(self):
        skip_list_root = self.get_lit_conf('skip_list_root', None)

        if skip_list_root is not None:
            self.skip_list_root = Path(skip_list_root)
        else:
            self.skip_list_root = Path('')

    def configure_excludes(self):
        excludes = self.get_lit_conf('excludes', set())
        additional_excludes = self.get_lit_conf('additional_excludes', '')

        excludes.update(additional_excludes.split(','))

        if self.skip_list_path is None:
            self.configure_skip_list_location()

        if self.skip_list_root is None:
            self.configure_skip_list_root()

        excludes.update(
            map(lambda x: os.path.join(str(self.skip_list_root), Path(x)),
                stl.test.file_parsing.parse_commented_file(
                    self.skip_list_path)))

        self.lit_config.excludes = excludes

    def configure_expected_failures_list_location(self):
        expected_failures_list_path = self.get_lit_conf(
            'expected_failures_list_path', None)

        if expected_failures_list_path is not None:
            self.expected_failures_list_path = Path(
                expected_failures_list_path)
        else:
            self.expected_failures_list_path = Path(os.devnull)

    def configure_expected_failures_list_root(self):
        expected_failures_list_root = self.get_lit_conf(
            'expected_failures_list_root', None)

        if expected_failures_list_root is not None:
            self.expected_failures_list_root = Path(
                expected_failures_list_root)
        else:
            self.expected_failures_list_root = Path('')

    def configure_expected_failures(self):
        expected_failures = self.get_lit_conf('expected_failures', set())
        additional_expected_failures = self.get_lit_conf(
            'additional_expected_failures', '')

        expected_failures.update(additional_expected_failures.split(','))

        if self.expected_failures_list_path is None:
            self.configure_expected_failures_list_location()

        if self.expected_failures_list_root is None:
            self.configure_expected_failures_list_root()

        expected_failures.update(
            map(lambda x: os.path.join(
                str(self.expected_failures_list_root), Path(x)),
                stl.test.file_parsing.parse_commented_file(
                    self.expected_failures_list_path)))

        self.lit_config.expected_failures = expected_failures
        self.config.expected_failures =\
            getattr(self.config, 'expected_failures', set())

    # TODO: Have configuring the compiler have the same flow as everything
    # else.
    def configure_default_compiler(self):
        self.default_compiler = CXXCompiler(None)
        self.configure_compile_flags()
        self.configure_link_flags()

        if self.stl_compilation_env is None:
            self.configure_compilation_env()

        self.default_compiler.compile_env = self.stl_compilation_env

    # TODO: Create an interface which allows remote building of a test.
    def configure_builder(self):
        pass

    # TODO: Create an interface which allows remote execution of a test.
    def configure_executor(self):
        self.executor = LocalExecutor()

    def configure_compile_flags(self):
        self.configure_compile_flags_header_includes()
        # Configure extra flags
        compile_flags_str = self.get_lit_conf('compile_flags', '')
        self.default_compiler.compile_flags += shlex.split(compile_flags_str)
        additional_flags = self.get_lit_conf('test_compiler_flags')
        if additional_flags:
            self.default_compiler.compile_flags +=\
                    shlex.split(additional_flags)

    # This is redundant
    def configure_compile_flags_header_includes(self):
        if self.cxx_headers is None:
            self.configure_cxx_headers()

        # self.default_compiler.compile_flags += ['/X']
        self.default_compiler.compile_flags += ['/I' + str(self.cxx_headers)]

        if self.config.name == 'libc++':
            if self.stl_src_root is None:
                self.configure_src_root()

            libcxx_support_path =\
                self.stl_src_root / 'llvm-project' / 'libcxx'\
                                  / 'test' / 'support'

            self.default_compiler.compile_flags.append(
                '/I' + str(libcxx_support_path))

    def configure_cxx_headers(self):
        cxx_headers = self.get_lit_conf('cxx_headers')

        if cxx_headers is None:
            if self.stl_build_root is None:
                self.configure_build_root()

            cxx_headers = self.stl_build_root / 'inc'

            if not os.path.isdir(cxx_headers):
                self.lit_config.fatal("cxx_headers='%s' is not a directory."
                                      % str(cxx_headers))
        else:
            cxx_headers = Path(cxx_headers)

        self.cxx_headers = cxx_headers

    def configure_link_flags(self):
        if self.cxx_library_root is None:
            self.configure_library_roots()

        if self.msvc_toolset_libs_root is None:
            self.configure_msvc_toolset_libs_root()

        self.default_compiler.link_flags.append(
            '/LIBPATH:' + str(self.cxx_library_root))

        self.default_compiler.link_flags.append(
            '/LIBPATH:' + str(self.msvc_toolset_libs_root))

    def configure_msvc_toolset_libs_root(self):
        msvc_toolset_libs_root = self.get_lit_conf('msvc_toolset_libs_root')

        if msvc_toolset_libs_root is None:
            self.lit_config.fatal('msvc_toolset_libs_root must be specified')

        self.msvc_toolset_libs_root = Path(msvc_toolset_libs_root)

    def get_test_format(self):
        from stl.test.format import StlTestFormat
        return StlTestFormat(
            self.default_compiler,
            self.execute_external,
            self.executor)

    # TODO: Might be nice to actually print something
    def print_config_info(self):
        pass
