// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <cstdio>
#include <fcntl.h>
#include <io.h>
#include <string>
#include <vector>

#include <Windows.h>

using namespace std;

namespace test {
    class win_console {
    public:
        win_console() {
            // There's a trick you can do to create a FILE stream for a Win32 console:
            //
            //   1. Call _open_osfhandle() to create a file descriptor for the console screen
            //      buffer handle.
            //
            //   2. Call _fdopen() to get a FILE* for the file descriptor.
            constexpr DWORD screen_buffer_access = (GENERIC_READ | GENERIC_WRITE);
            console_handle =
                CreateConsoleScreenBuffer(screen_buffer_access, 0, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);

            if (console_handle == INVALID_HANDLE_VALUE) [[unlikely]] {
                return;
            }

            const int console_fd = _open_osfhandle(reinterpret_cast<intptr_t>(console_handle), _O_TEXT);

            if (console_fd == -1) [[unlikely]] {
                return;
            }

            file_stream_ptr = _fdopen(console_fd, "w");
        }

        ~win_console() {
            delete_console();
        }

        win_console(const win_console&) = delete;
        win_console& operator=(const win_console&) = delete;

        win_console(win_console&& rhs) noexcept
            : console_handle(rhs.console_handle), file_stream_ptr(rhs.file_stream_ptr)
        {
            rhs.console_handle = nullptr;
            rhs.file_stream_ptr = nullptr;
        }

        win_console& operator=(win_console&& rhs) noexcept
        {
            delete_console();

            console_handle = rhs.console_handle;
            rhs.console_handle = nullptr;

            file_stream_ptr = rhs.file_stream_ptr;
            rhs.file_stream_ptr = nullptr;

            return *this;
        }

        FILE* get_file_stream() const
        {
            assert(is_console_valid());
            return file_stream_ptr;
        }

        wstring get_console_line(const size_t line_number) const
        {
            // We use the ReadConsoleOutputW() function to read lines of text which were written to the console.
            // The neat thing here is that if we write to the console using the FILE* returned by
            // win_console::get_file_stream() with std::print(), we can still use ReadConsoleOutputW() to get
            // that text! This allows us to verify the output being written to the console.
            CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;

            {
                const BOOL get_screen_buffer_info_result =
                    GetConsoleScreenBufferInfo(console_handle, &screen_buffer_info);
                assert(get_screen_buffer_info_result && "ERROR: GetConsoleScreenBufferInfo() failed!");
            }

            const size_t num_columns = static_cast<size_t>(screen_buffer_info.dwSize.X);
            vector<CHAR_INFO> char_info_arr{num_columns};

            // Send the data to (0, 0) within char_info_arr (i.e., starting from the first element).
            constexpr COORD destination_coords{};

            const COORD dest_buffer_size{.X = static_cast<SHORT>(num_columns), .Y = 1};
            SMALL_RECT read_region_rect{.Left = 0,
                .Top                          = static_cast<SHORT>(line_number),
                .Right                        = static_cast<SHORT>(num_columns),
                .Bottom                       = static_cast<SHORT>(line_number + 1)};

            {
                const BOOL read_console_output_result = ReadConsoleOutputW(
                    console_handle, char_info_arr.data(), dest_buffer_size, destination_coords, &read_region_rect);
                assert(read_console_output_result && "ERROR: ReadConsoleOutputW() failed!");
            }

            wstring console_line_str{};
            console_line_str.reserve(num_columns);

            for (const auto& char_info : char_info_arr) {
                console_line_str.push_back(char_info.Char.UnicodeChar);
            }

            // By default, Windows fills console character cells with space characters. We want to remove
            // those space characters which appear after the user's text.
            const size_t last_valid_char = console_line_str.find_last_not_of(' ');

            if (last_valid_char == wstring::npos) [[unlikely]] {
                console_line_str.clear();
            } else [[likely]] {
                console_line_str = console_line_str.substr(0, last_valid_char + 1);
            }

            return console_line_str;
        }

    private:
        void delete_console() {
            if (is_console_valid()) [[likely]] {
                // According to the MSDN, we don't call CloseHandle() on handles passed to _open_osfhandle(),
                // and we don't call _close() on file descriptors passed to _fdopen(). So, our only clean-up
                // task is to call fclose().
                fclose(file_stream_ptr);

                console_handle  = nullptr;
                file_stream_ptr = nullptr;
            }
        }

        bool is_console_valid() const {
            return (console_handle != nullptr && console_handle != INVALID_HANDLE_VALUE && file_stream_ptr != nullptr);
        }

    private:
        HANDLE console_handle;
        FILE* file_stream_ptr;
    };
} // namespace test