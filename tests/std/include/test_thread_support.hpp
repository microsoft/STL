// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <thread>

#include <Windows.h>

class one_shot {
    bool unblock = false;
    std::mutex mtx;
    std::condition_variable cv;

public:
    template <typename Rep, typename Period>
    bool wait_for(const std::chrono::duration<Rep, Period>& dur) {
        std::unique_lock<std::mutex> lck(this->mtx);
        return this->cv.wait_for(lck, dur, [this] { return this->unblock; });
    }

    void fire() {
        {
            std::lock_guard<std::mutex> lck(this->mtx);
            if (this->unblock) {
                return;
            }

            this->unblock = true;
        }

        this->cv.notify_all();
    }
};

class watchdog {
    one_shot os;
    std::thread t;

    static void kill() {
        fprintf(stdout, "Terminating test due to watchdog timeout.\n");
        fflush(stdout);
        _Exit(1);
    }

    static void kill_if_not_debugging() {
        if (::IsDebuggerPresent()) {
            fprintf(stdout, "Watchdog not terminating because debugger is connected.\n");
            fflush(stdout);
        } else {
            kill();
        }
    }

public:
    template <typename Rep, typename Period>
    explicit watchdog(const std::chrono::duration<Rep, Period>& dur)
        : os(), t([dur, this]() {
              if (!this->os.wait_for(dur)) {
                  kill_if_not_debugging();
              }
          }) {}

    void muzzle() {
        this->os.fire();
    }

    ~watchdog() {
        this->muzzle();
        this->t.join();
    }
};
