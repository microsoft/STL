// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// test <system_error>
#define TEST_NAME "<system_error>"

#include "tdefs.h"
#include <stdexcept>
#include <system_error>

#define ERR STD errc::

#define ERR_ENUM STD errc

struct error_pair {
    ERR_ENUM ec;
    int val;
};

static const error_pair errs[] = {
    // enumeration constants
    {ERR address_family_not_supported, EAFNOSUPPORT},
    {ERR address_in_use, EADDRINUSE},
    {ERR address_not_available, EADDRNOTAVAIL},
    {ERR already_connected, EISCONN},
    {ERR argument_list_too_long, E2BIG},
    {ERR argument_out_of_domain, EDOM},
    {ERR bad_address, EFAULT},
    {ERR bad_file_descriptor, EBADF},
    {ERR bad_message, EBADMSG},
    {ERR broken_pipe, EPIPE},
    {ERR connection_aborted, ECONNABORTED},
    {ERR connection_already_in_progress, EALREADY},
    {ERR connection_refused, ECONNREFUSED},
    {ERR connection_reset, ECONNRESET},
    {ERR cross_device_link, EXDEV},
    {ERR destination_address_required, EDESTADDRREQ},
    {ERR device_or_resource_busy, EBUSY},
    {ERR directory_not_empty, ENOTEMPTY},
    {ERR executable_format_error, ENOEXEC},
    {ERR file_exists, EEXIST},
    {ERR file_too_large, EFBIG},
    {ERR filename_too_long, ENAMETOOLONG},
    {ERR function_not_supported, ENOSYS},
    {ERR host_unreachable, EHOSTUNREACH},
    {ERR identifier_removed, EIDRM},
    {ERR illegal_byte_sequence, EILSEQ},
    {ERR inappropriate_io_control_operation, ENOTTY},
    {ERR interrupted, EINTR},
    {ERR invalid_argument, EINVAL},
    {ERR invalid_seek, ESPIPE},
    {ERR io_error, EIO},
    {ERR is_a_directory, EISDIR},
    {ERR message_size, EMSGSIZE},
    {ERR network_down, ENETDOWN},
    {ERR network_reset, ENETRESET},
    {ERR network_unreachable, ENETUNREACH},
    {ERR no_buffer_space, ENOBUFS},
    {ERR no_child_process, ECHILD},
    {ERR no_link, ENOLINK},
    {ERR no_lock_available, ENOLCK},
    {ERR no_message_available, ENODATA},
    {ERR no_message, ENOMSG},
    {ERR no_protocol_option, ENOPROTOOPT},
    {ERR no_space_on_device, ENOSPC},
    {ERR no_stream_resources, ENOSR},
    {ERR no_such_device_or_address, ENXIO},
    {ERR no_such_device, ENODEV},
    {ERR no_such_file_or_directory, ENOENT},
    {ERR no_such_process, ESRCH},
    {ERR not_a_directory, ENOTDIR},
    {ERR not_a_socket, ENOTSOCK},
    {ERR not_a_stream, ENOSTR},
    {ERR not_connected, ENOTCONN},
    {ERR not_enough_memory, ENOMEM},
    {ERR not_supported, ENOTSUP},
    {ERR operation_canceled, ECANCELED},
    {ERR operation_in_progress, EINPROGRESS},
    {ERR operation_not_permitted, EPERM},
    {ERR operation_not_supported, EOPNOTSUPP},
    {ERR operation_would_block, EWOULDBLOCK},
    {ERR owner_dead, EOWNERDEAD},
    {ERR permission_denied, EACCES},
    {ERR protocol_error, EPROTO},
    {ERR protocol_not_supported, EPROTONOSUPPORT},
    {ERR read_only_file_system, EROFS},
    {ERR resource_deadlock_would_occur, EDEADLK},
    {ERR resource_unavailable_try_again, EAGAIN},
    {ERR result_out_of_range, ERANGE},
    {ERR state_not_recoverable, ENOTRECOVERABLE},
    {ERR stream_timeout, ETIME},
    {ERR text_file_busy, ETXTBSY},
    {ERR timed_out, ETIMEDOUT},
    {ERR too_many_files_open_in_system, ENFILE},
    {ERR too_many_files_open, EMFILE},
    {ERR too_many_links, EMLINK},
    {ERR too_many_symbolic_link_levels, ELOOP},
    {ERR value_too_large, EOVERFLOW},
    {ERR wrong_protocol_type, EPROTOTYPE},
};

void test_main() { // run tests
    // test generic errors
    for (const auto& p : errs) {
        CHECK_INT((int) p.ec, p.val);
    }

    // test error_code
    {
        STD error_code ec0;
        CSTD size_t hash_val = STD hash<STD error_code>()(ec0);
        CHECK_INT(hash_val, STD hash<STD error_code>()(ec0));

        CHECK_INT(ec0.value(), 0);
        CHECK(ec0.category() == STD system_category());
        CHECK(!ec0);

        STD error_code ec1 = STD make_error_code(ERR too_many_links);
        CHECK_INT(ec1.value(), ERR too_many_links);
        CHECK(ec1.category() == STD generic_category());
        CHECK(ec1.value() != 0);

        STD error_code ec2(EDOM, STD system_category());
        CHECK_INT(ec2.value(), EDOM);
        CHECK(ec2.category() == STD system_category());
        CHECK(ec2.value() != 0);

        STD string str = ec2.message();
        CHECK(!str.empty());

        CHECK(ec0 != ec1);
        CHECK(ec1 == ec1);
        CHECK(!(ec1 < ec1));

        STD error_condition ec0x, ec1x(ERR too_many_links);
        CHECK(ec0 == ec0x);
        CHECK(ec0 != ec1x);
        CHECK(!STD is_error_code_enum<ERR_ENUM>::value);
        CHECK(ec1 == STD make_error_code(ERR too_many_links));

        CHECK(ec1.default_error_condition() == ec1.category().default_error_condition(ec1.value()));
    }

    // test error_condition
    {
        STD error_condition ec0;

        CHECK_INT(ec0.value(), 0);
        CHECK(ec0.category() == STD generic_category());
        CHECK(!ec0);

        STD error_code ec1 = STD make_error_code(ERR too_many_links);
        CHECK_INT(ec1.value(), ERR too_many_links);
        CHECK(ec1.category() == STD generic_category());
        CHECK(ec1.value() != 0);

        STD error_code ec2(EDOM, STD system_category());
        CHECK_INT(ec2.value(), EDOM);
        CHECK(ec2.category() == STD system_category());
        CHECK(ec2.value() != 0);

        STD string str = ec2.message();
        CHECK(!str.empty());

        CHECK(ec0 != ec1);
        CHECK(ec1 == ec1);
        CHECK(!(ec1 < ec1));

        STD error_code ec0x, ec1x = STD make_error_code(ERR too_many_links);
        CHECK(ec0 == ec0x);
        CHECK(ec0 != ec1x);
        CHECK(STD is_error_condition_enum<ERR_ENUM>::value);
        CHECK(ec1 == STD make_error_condition(ERR too_many_links));
    }

    // test error_category
    const STD error_category* pcat = &STD generic_category();

    CHECK(pcat != &STD system_category());
    CHECK(STD generic_category().name() != nullptr);
    CHECK(!STD generic_category().message(EDOM).empty());
    CHECK(STD generic_category() != STD system_category());
    CHECK(STD generic_category() == STD generic_category());

    // test system_error
    STD system_error syserr1((int) ERR io_error, STD generic_category(), STD string("help"));
    STD system_error syserr2((int) ERR io_error, STD system_category(), STD string("help"));
    STD system_error syserr3((int) ERR io_error, STD generic_category(), "help");
    STD system_error syserr4((int) ERR io_error, STD system_category(), "help");
    STD runtime_error* pre = &syserr1;

    pre = &syserr2;
    pre = pre; // to quiet diagnostics
    CHECK_INT(syserr1.code().value(), ERR io_error);
    CHECK(*syserr2.what() != '\0');
    CHECK_INT(syserr3.code().value(), ERR io_error);
    CHECK(*syserr4.what() != '\0');
}
