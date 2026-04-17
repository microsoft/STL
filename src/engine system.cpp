#include "engine.hpp"
#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>

// forward declaration
float call_python_model(const std::vector<float>& input);

// =========================
// Thread-Safe Queue
// =========================
template<typename T>
class SafeQueue {
public:
    void push(T value) {
        std::lock_guard<std::mutex> lock(mu);
        q.push(std::move(value));
        cv.notify_one();
    }

    bool pop(T& value) {
        std::unique_lock<std::mutex> lock(mu);
        cv.wait(lock, [this]() { return !q.empty() || stop; });

        if (q.empty()) return false;

        value = std::move(q.front());
        q.pop();
        return true;
    }

    void shutdown() {
        stop = true;
        cv.notify_all();
    }

private:
    std::queue<T> q;
    std::mutex mu;
    std::condition_variable cv;
    bool stop = false;
};

// =========================
// Logger (minimal, fast)
// =========================
void log(const std::string& msg) {
    std::cout << "[ENGINE] " << msg << std::endl;
}

// =========================
// Worker System
// =========================
class WorkerPool {
public:
    WorkerPool(size_t num_threads) : running(true) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back(&WorkerPool::worker_loop, this, i);
        }
    }

    ~WorkerPool() {
        stop();
    }

    void submit(std::vector<float> data) {
        tasks.push(std::move(data));
    }

    void stop() {
        if (!running) return;

        running = false;
        tasks.shutdown();

        for (auto& t : workers) {
            if (t.joinable()) t.join();
        }
    }

private:
    void worker_loop(int id) {
        while (running) {
            std::vector<float> task;

            if (!tasks.pop(task)) break;

            float result = call_python_model(task);

            {
                std::lock_guard<std::mutex> lock(output_mu);
                std::cout << "[Worker " << id << "] Result: " << result << std::endl;
            }
        }
    }

    std::vector<std::thread> workers;
    SafeQueue<std::vector<float>> tasks;
    std::mutex output_mu;
    std::atomic<bool> running;
};

// =========================
// Engine Implementation
// =========================
void STLEngine::run() {
    log("🚀 Starting STL Event-Driven Engine");

    auto start_time = std::chrono::high_resolution_clock::now();

    // =========================
    // 1. Create Worker Pool
    // =========================
    const int num_threads = std::thread::hardware_concurrency();
    WorkerPool pool(num_threads > 0 ? num_threads : 4);

    log("🧵 Threads: " + std::to_string(num_threads));

    // =========================
    // 2. Generate & Submit Tasks
    // =========================
    for (int i = 1; i <= 10; ++i) {
        std::vector<float> sample;

        for (int j = 1; j <= 5; ++j) {
            sample.push_back(i * j * 1.0f);
        }

        pool.submit(sample);
    }

    log("📦 Tasks submitted");

    // =========================
    // 3. Wait before shutdown
    // =========================
    std::this_thread::sleep_for(std::chrono::seconds(2));

    pool.stop();

    // =========================
    // 4. Timing
    // =========================
    auto end_time = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(end_time - start_time).count();

    log("⏱️ Total time: " + std::to_string(elapsed) + " sec");
    log("✅ Engine shutdown complete");
}
