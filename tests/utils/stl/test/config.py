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


# Extract the value of a numeric macro such as __cplusplus or a feature-test
# macro.
def intMacroValue(token):
    return int(token.rstrip('LlUu'))


class Configuration:
    # pylint: disable=redefined-outer-name
    def __init__(self, lit_config, config):
        self.build_executor = None
        self.config = config
        self.cxx_headers = None
        self.cxx_library_root = None
        self.cxx_runtime_root = None
        self.default_compiler = None
        self.execute_external = False
        self.expected_results_list_path = None
        self.expected_results_list_root = None
        self.format_name = None
        self.lit_config = lit_config
        self.link_shared = True
        self.long_tests = None
        self.msvc_toolset_libs_root = None
        self.stl_build_root = None
        self.stl_path_env_var = None
        self.stl_src_root = None
        self.stl_test_env = None
        self.target_arch = None
        self.target_info = stl.test.target_info.WindowsLocalTI(lit_config)
        self.test_executor = None
        self.test_source_root = None

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
        self.configure_executors()
        self.configure_expected_results()
        self.configure_test_dirs()
        self.configure_test_format()

    def configure_test_format(self):
        format_name = self.get_lit_conf('format_name', None)

        if format_name is None:
            format_name = 'STLTestFormat'

        self.format_name = format_name

    def configure_test_dirs(self):
        test_subdirs = list()

        test_subdirs_file = self.get_lit_conf('test_subdirs_file', None)

        if test_subdirs_file is not None:
            test_subdirs_file = Path(test_subdirs_file)
            test_subdirs += \
                stl.test.file_parsing.parse_commented_file(test_subdirs_file)

        test_subdirs += self.get_lit_conf('test_subdirs', [])

        test_subdirs_root = Path(self.get_lit_conf('test_subdirs_root', ''))
        self.lit_config.test_subdirs = \
            list(map(lambda x: str((test_subdirs_root / x).resolve()),
                     test_subdirs))

    # TRANSITION: Don't hard-code features.
    def configure_features(self):
        self.config.available_features.add('long_tests')
        self.config.available_features.add('c++2a')
        self.config.available_features.add('msvc')
        self.config.available_features.update(self.target_info.features)

        if self.target_arch is None:
            self.configure_target_architecture()

        if self.target_arch == 'x86':
            self.config.available_features.add('edg')

    def configure_test_source_root(self):
        test_source_root = self.get_lit_conf('test_source_root', None)

        if test_source_root is not None:
            self.test_source_root = Path(test_source_root)

    def configure_src_root(self):
        stl_src_root = self.get_lit_conf('stl_src_root', None)

        if stl_src_root is None:
            if self.test_source_root is None:
                self.configure_test_source_root()

                if self.test_source_root is None:
                    self.lit_config.fatal(
                        "Could not infer stl_src_root from test_source_root. "
                        "test_source_root is None")

            if self.config.name == 'libc++':
                stl_src_root = self.test_source_root.parents[2]
            else:
                stl_src_root = self.test_source_root.parents[1]

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
                stl_build_root = \
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
                cxx_library_root = \
                    self.stl_build_root / 'lib' / subfolder_name

                self.lit_config.note(
                    "cxx_library_root was not specified. Defaulting to: %s."
                    % str(cxx_library_root))

        self.cxx_runtime_root = Path(cxx_runtime_root)
        self.cxx_library_root = Path(cxx_library_root)

    def configure_path_env_var(self):
        stl_path_env_var = self.get_lit_conf('stl_path_env_var', None)

        if stl_path_env_var is None:
            path_list = list()

            msvc_bin_dir = self.get_lit_conf('msvc_bin_dir', None)
            if msvc_bin_dir is not None:
                path_list.append(msvc_bin_dir)

            if self.cxx_runtime_root is None:
                self.configure_library_roots()
            path_list.append(str(self.cxx_runtime_root))

            config_env = self.config.environment.get('PATH', None)
            if config_env is not None:
                path_list.append(config_env)

            stl_path_env_var = ';'.join(path_list)

        self.stl_path_env_var = stl_path_env_var

    def configure_test_env(self):
        if self.stl_path_env_var is None:
            self.configure_path_env_var()

        stl_test_env = {}
        stl_test_env['PATH'] = self.stl_path_env_var

        self.config.environment = stl_test_env

    def configure_expected_results_list_location(self):
        expected_results_list_path = self.get_lit_conf(
            'expected_results_list_path', None)

        if expected_results_list_path is not None:
            self.expected_results_list_path = Path(
                expected_results_list_path)
        else:
            self.expected_results_list_path = Path(os.devnull)

    def configure_expected_results(self):
        expected_results = getattr(self.lit_config, 'expected_results', dict())

        if self.expected_results_list_path is None:
            self.configure_expected_results_list_location()

        expected_results[self.config.name] = \
            stl.test.file_parsing.parse_result_file(
                self.expected_results_list_path)

        self.lit_config.expected_results = expected_results
        self.config.expected_results = \
            getattr(self.config, 'expected_results', dict())

    def configure_default_compiler(self):
        self.default_compiler = CXXCompiler(None)
        self.configure_compile_flags()
        self.configure_link_flags()
        self.configure_test_env()

        self.default_compiler.compile_env = self.config.environment

        env_var = 'STL_EDG_DROP'
        if env_var in os.environ and os.environ[env_var] is not None:
            self.default_compiler.edg_drop = os.environ[env_var]

    # TRANSITION: Investigate using SSHExecutor for ARM
    def configure_executors(self):
        self.build_executor = LocalExecutor()
        self.test_executor = LocalExecutor()

    def configure_compile_flags(self):
        self.configure_compile_flags_header_includes()
        # Configure extra flags
        self.default_compiler.compile_flags += \
            self.get_lit_conf('compile_flags', [])
        additional_flags_str = self.get_lit_conf('additional_compiler_flags')
        if additional_flags_str:
            self.default_compiler.compile_flags += \
                shlex.split(additional_flags_str)

    def configure_compile_flags_header_includes(self):
        if self.cxx_headers is None:
            self.configure_cxx_headers()

        self.default_compiler.compile_flags += ['/I' + str(self.cxx_headers)]

        include_dirs = self.get_lit_conf('include_dirs', [])

        for directory in include_dirs:
            self.default_compiler.compile_flags.append('/I' + directory)

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

        additional_flags_str = self.get_lit_conf('additional_link_flags')
        if additional_flags_str:
            self.default_compiler.link_flags += \
                shlex.split(additional_flags_str)

    def configure_msvc_toolset_libs_root(self):
        msvc_toolset_libs_root = self.get_lit_conf('msvc_toolset_libs_root')

        if msvc_toolset_libs_root is None:
            self.lit_config.fatal('msvc_toolset_libs_root must be specified')

        self.msvc_toolset_libs_root = Path(msvc_toolset_libs_root)

    def get_test_format(self):
        import stl.test.format

        return getattr(stl.test.format, self.format_name)(
            self.default_compiler,
            self.execute_external,
            self.build_executor,
            self.test_executor)

    # TRANSITION: Might be nice to actually print something
    def print_config_info(self):
        pass
