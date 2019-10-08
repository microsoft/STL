// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stljobs.h"
#include <algorithm>
#include <assert.h>
#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <limits.h>
#include <memory>
#include <mutex>
#include <optional>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include <Windows.h>

class tp_wait {
public:
    explicit tp_wait(PTP_WAIT_CALLBACK pfnwa, PVOID pv, PTP_CALLBACK_ENVIRON pcbe)
        : wait(CreateThreadpoolWait(pfnwa, pv, pcbe)) {
        if (!wait) {
            api_failure("CreateThreadpoolWait");
        }
    }

    tp_wait(const tp_wait&) = delete;
    tp_wait& operator=(const tp_wait&) = delete;

    void wait_for(const HANDLE waitOn) noexcept {
        SetThreadpoolWait(wait, waitOn, nullptr);
    }

    ~tp_wait() {
        WaitForThreadpoolWaitCallbacks(wait, TRUE);
        CloseThreadpoolWait(wait);
    }

private:
    PTP_WAIT wait{};
};

class parallelizer {
public:
    parallelizer() : subprocesses(std::make_unique<entry[]>(availableConcurrency)) {
        for (size_t idx = 0; idx < availableConcurrency; ++idx) {
            subprocesses[idx].parent = this;
        }
    }

    void add_command(std::wstring&& toRun) {
        std::lock_guard lck(mtx);
        const auto oldCommandsSize = commands.size();
        commands.emplace_back(std::move(toRun), std::nullopt);
        const auto localConcurrency = runningConcurrency.load(std::memory_order_relaxed);
        if (availableConcurrency == localConcurrency) {
            return;
        }

        runningConcurrency.store(localConcurrency + 1, std::memory_order_relaxed);
        size_t scheduledOn = 0;
        while (subprocesses[scheduledOn].commandRunning != SIZE_MAX) {
            ++scheduledOn;
        }

        nextCommandToRun = oldCommandsSize + 1;
        subprocesses[scheduledOn].schedule(oldCommandsSize);
        update_display();
    }

    void wait_all() noexcept {
        std::unique_lock lck{mtx};
        cv.wait(lck, [this] { return runningConcurrency.load(std::memory_order_relaxed) == 0; });
    }

    [[nodiscard]] const std::vector<std::pair<std::wstring, std::optional<execution_result>>>& results() const
        noexcept {
        assert(runningConcurrency.load(std::memory_order_relaxed) == 0);
        return commands;
    }

private:
    struct entry;
    std::mutex mtx{};
    std::condition_variable cv{};
    std::vector<std::pair<std::wstring, std::optional<execution_result>>> commands{};
    size_t nextCommandToRun{0};
    size_t availableConcurrency{std::thread::hardware_concurrency()};
    std::atomic<size_t> runningConcurrency{0};
    std::unique_ptr<entry[]> subprocesses;

    struct entry {
        parallelizer* parent;
        size_t commandRunning{SIZE_MAX}; // guarded by parent->mtx
        subprocess_executive executive;
        tp_wait tpWait{callback, this, nullptr};

        static void __stdcall callback(
            PTP_CALLBACK_INSTANCE, void* thisRaw, PTP_WAIT, [[maybe_unused]] TP_WAIT_RESULT waitDisposition) noexcept {
            assert(waitDisposition == WAIT_OBJECT_0);
            const auto this_  = static_cast<entry*>(thisRaw);
            const auto parent = this_->parent;
            auto results      = this_->executive.complete();
            std::lock_guard lck{parent->mtx};
            parent->commands[this_->commandRunning].second.emplace(std::move(results));
            if (parent->nextCommandToRun == parent->commands.size()) {
                this_->commandRunning = SIZE_MAX;
                if (parent->runningConcurrency.fetch_sub(1, std::memory_order_relaxed) == 1) {
                    parent->cv.notify_all();
                }
            } else {
                this_->schedule(parent->nextCommandToRun++);
            }

            parent->update_display();
        }

        void schedule(const size_t command) {
            commandRunning = command;
            executive.begin_execution(nullptr, parent->commands[command].first.data(), 0, nullptr);
            tpWait.wait_for(executive.get_wait_handle());
        }
    };

    void update_display() noexcept {
        const auto commandsSize = commands.size();
        const auto next         = nextCommandToRun;
        const auto running      = runningConcurrency.load(std::memory_order_relaxed);
        assert(running <= next);
        printf("%zu scheduled; %zu completed; %zu running\n", commandsSize, next - running, running);
    }
};

void schedule_command(parallelizer& p, const std::wstring_view commandPrefix, const std::wstring& native) {
    std::wstring toExecute;
    toExecute.reserve(commandPrefix.size() + 1 + native.size());
    toExecute.assign(commandPrefix);
    toExecute.push_back(L' ');
    toExecute.append(native);
    p.add_command(std::move(toExecute));
}

extern "C" int wmain(int argc, wchar_t* argv[]) {
    try {
        if (argc < 3) {
            puts("Usage: parallelize.exe commandPrefix pathRoot0 [... pathRootN]\n"
                 "The command:\n"
                 "commandPrefix file\n"
                 "will be launched in parallel for every regular file under any of pathRoots, recursively.");
            return 1;
        }

        parallelizer p;
        std::wstring_view commandPrefix(argv[1]);
        for (int idx = 2; idx < argc; ++idx) {
            std::filesystem::path thisSpec(argv[idx]);
            if (std::filesystem::is_regular_file(thisSpec)) {
                schedule_command(p, commandPrefix, thisSpec.native());
                continue;
            }

            for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(std::move(thisSpec))) {
                if (!dirEntry.is_regular_file()) {
                    printf("Skipping non-regular-file %ls\n", dirEntry.path().c_str());
                    continue;
                }

                schedule_command(p, commandPrefix, dirEntry.path().native());
            }
        }

        p.wait_all();
        bool exitSuccess    = true;
        size_t vacuousCount = 0;
        for (auto& command : p.results()) {
            auto& result = command.second.value();
            if (result.exitCode == 0) {
                if (result.output.empty()) {
                    ++vacuousCount;
                } else {
                    printf("%ls produced output:\n%s\n", command.first.c_str(), result.output.c_str());
                }
            } else {
                exitSuccess = false;
                if (result.output.empty()) {
                    printf("%ls exited with 0x%08lX and no output.\n", command.first.c_str(), result.exitCode);
                } else {
                    printf("%ls exited with 0x%08lX and output:\n%s\n", command.first.c_str(), result.exitCode,
                        result.output.c_str());
                }
            }
        }

        const auto totalCommands = p.results().size();
        printf("%zu commands ran, returned 0, and produced no output", vacuousCount);
        if (vacuousCount == totalCommands) {
            puts(".");
        } else {
            printf(" (out of %zu).", totalCommands);
        }

        if (exitSuccess) {
            return 0;
        } else {
            return 1;
        }
    } catch (std::filesystem::filesystem_error& err) {
        fputs(err.what(), stderr);
        abort();
    } catch (api_exception& api) {
        api.give_up();
    }
}
