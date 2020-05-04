from pathlib import Path
import os


def get_working_directory(step):
    work_dir = Path()
    if str(step.work_dir) == '.':
        work_dir = Path(os.getcwd())
    else:
        work_dir = step.work_dir

    return work_dir


def merge_environments(current_env, updated_env, appended_vars={'PATH'}):
    result_env = dict(current_env)
    for k, v in updated_env.items():
        if k in appended_vars:
            current_v = result_env.get(k)
            if current_v:
                result_env[k] = v + ';' + current_v
            else:
                result_env[k] = v
        else:
            result_env[k] = v
    return result_env


class BuildStepWriter:
    def write(self, test, step, test_file_handle):
        work_dir = get_working_directory(step)

        build_cmd = '"' + step.cmd[0] + '"'
        args = '"' + '" "'.join(step.cmd[1:]) + '"'
        build_cmd = build_cmd.replace('\\', '/')
        args = args.replace('\\', '/')

        pass_string = \
            'add_custom_command(OUTPUT {out} COMMAND {cmd} ARGS {args} DEPENDS msvcpd_implib msvcp_implib libcpmt libcpmt1 libcpmtd libcpmtd1 libcpmtd0 {deps} WORKING_DIRECTORY "{cwd}")\nadd_custom_target({name} ALL DEPENDS {out})'
        fail_string = \
            'add_test(NAME {name} COMMAND {cmd} WORKING_DIRECTORY "{cwd}")\nset_property(TEST {name} PROPERTY WILL_FAIL TRUE)'
        env_prop = \
            'set_property(TEST {name} PROPERTY ENVIRONMENT {env})'

        if not step.should_fail:
            name = test.getFullName() + '_' + str(step.num)
            print(pass_string.format(out=' '.join(map(lambda dep: dep.as_posix(), step.out_files)),
                                     cmd=build_cmd,
                                     args=args,
                                     deps='"' + '" "'.join(map(lambda dep: dep.as_posix(), step.dependencies)) + '"',
                                     cwd=work_dir.as_posix(),
                                     name=name),
                  file=test_file_handle)
        else:
            print(fail_string.format(name=test.getFullName(),
                                     cmd=build_cmd + ' ' + args,
                                     cwd=work_dir.as_posix()),
                  file=test_file_handle)

            if len(step.env):
                env_list = []
                for k, v in step.env.items():
                    env_list.append(k + '=' + v)
                cmake_env_list = \
                    '"' + \
                    '" "'.join(env_list).replace('\\', '/').replace(';', '\\;') + \
                    '"'

                print(env_prop.format(name=test.getFullName(),
                                      env=cmake_env_list),
                      file=test_file_handle)


class LocalTestStepWriter:
    def write(self, test, step, test_file_handle):
        work_dir = get_working_directory(step)

        cmd = '"' + '" "'.join(step.cmd).replace('\\', '/') + '"'

        test_string = \
            'add_test(NAME {name} COMMAND {cmd} WORKING_DIRECTORY "{cwd}")'
        env_prop = \
            'set_property(TEST {name} PROPERTY ENVIRONMENT {env})'
        fail_string = \
            'set_property(TEST {name} PROPERTY WILL_FAIL TRUE)'
        depends_string = \
            'set_property(TEST {name} PROPERTY DEPENDS {prev_test})'

        test_name = test.getFullName() + '_' + str(step.num)
        print(test_string.format(name=test_name,
                                 cmd=cmd,
                                 cwd=work_dir.as_posix()),
              file=test_file_handle)

        if len(step.env):
            env_list = []
            for k, v in step.env.items():
                env_list.append(k + '=' + v)
            cmake_env_list = \
                '"' + \
                '" "'.join(env_list).replace('\\', '/').replace(';', '\\;') + \
                '"'

            print(env_prop.format(name=test_name, env=cmake_env_list),
                  file=test_file_handle)

        if step.should_fail:
            print(fail_string.format(name=test_name),
                  file=test_file_handle)

        if step.num != 0:
            prev_test = \
                test.getFullName() + '_' + str(step.num - 1)
            print(depends_string.format(name=test_name,
                                        prev_test=prev_test),
                  file=test_file_handle)
