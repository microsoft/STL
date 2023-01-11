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

        win_console(const win_console&)            = delete;
        win_console& operator=(const win_console&) = delete;

        win_console(win_console&& rhs) noexcept
            : console_handle(rhs.console_handle), file_stream_ptr(rhs.file_stream_ptr) {
            rhs.console_handle  = nullptr;
            rhs.file_stream_ptr = nullptr;
        }

        win_console& operator=(win_console&& rhs) noexcept {
            delete_console();

            console_handle     = rhs.console_handle;
            rhs.console_handle = nullptr;

            file_stream_ptr     = rhs.file_stream_ptr;
            rhs.file_stream_ptr = nullptr;

            return *this;
        }

        FILE* get_file_stream() const {
            assert(is_console_valid());
            return file_stream_ptr;
        }

        wstring get_console_line(const size_t line_number) const {
            // We use the ReadConsoleOutputCharacterW() function to read lines of text which were written to 
            // the console. The neat thing here is that if we write to the console using the FILE* returned by
            // win_console::get_file_stream() with std::print(), we can still use ReadConsoleOutputCharacterW() 
            // to get that text! This allows us to verify the output being written to the console and check for,
            // e.g., invalid code point replacement.
            const size_t line_char_width = get_line_character_width();
            const COORD read_coords{.X = 0, .Y = static_cast<SHORT>(line_number)};

            wstring output_str;
            output_str.resize_and_overwrite(
                line_char_width, [this, read_coords](wchar_t* const dest_ptr, const size_t allocated_size) {
                    DWORD num_chars_read;
                    const BOOL read_output_result = ReadConsoleOutputCharacterW(
                        console_handle, dest_ptr, static_cast<DWORD>(allocated_size), read_coords, &num_chars_read);

                    assert(read_output_result && "ERROR: ReadConsoleOutputCharacterW() failed!");
                    return num_chars_read;
                });

            // By default, Windows fills console character cells with space characters. We want to remove
            // those space characters which appear after the user's text.
            const std::size_t lastValidChar = output_str.find_last_not_of(' ');

            if (lastValidChar == std::wstring::npos) [[unlikely]] {
                output_str.clear();
            } else [[likely]] {
                output_str = output_str.substr(0, lastValidChar + 1);
            }

            return output_str;
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

        std::size_t get_line_character_width() const {
            CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;

            {
                const BOOL get_screen_buffer_info_result =
                    GetConsoleScreenBufferInfo(console_handle, &screen_buffer_info);
                assert(get_screen_buffer_info_result && "ERROR: GetConsoleScreenBufferInfo() failed!");
            }

            return static_cast<size_t>(screen_buffer_info.dwSize.X);
        }

        bool is_console_valid() const {
            return (console_handle != nullptr && console_handle != INVALID_HANDLE_VALUE && file_stream_ptr != nullptr);
        }

    private:
        HANDLE console_handle;
        FILE* file_stream_ptr;
    };
} // namespace test