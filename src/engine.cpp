#include "engine.hpp"
#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <chrono>
#include <mutex>
#include <numeric>

// forward declaration (binding ke Python)
float call_python_model(const std::vector<float>& input);

// =========================
// Logger (thread-safe)
// =========================
class Logger {
public:
    static void info(const std::string& msg) {
        std::lock_guard<std::mutex> lock(mu);
        std::cout << "[INFO] " << msg << std::endl;
    }

private:
    static std::mutex mu;
};

std::mutex Logger::mu;

// =========================
// Timer Utility
// =========================
class Timer {
public:
    Timer() {
        start = std::chrono::high_resolution_clock::now();
    }

    double elapsed() {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start).count();
    }

private:
    std::chrono::high_resolution_clock::time_point start;
};

// =========================
// Core Engine Implementation
// =========================
void STLEngine::run() {
    Logger::info("🔥 STL Advanced Engine Running...");

    Timer total_timer;

    // =========================
    // 1. Generate Batch Data
    // =========================
    std::vector<std::vector<float>> batch_data;

    for (int i = 0; i < 5; ++i) {
        std::vector<float> sample;
        for (int j = 1; j <= 5; ++j) {
            sample.push_back(i * j * 1.0f);
        }
        batch_data.push_back(sample);
    }

    Logger::info("📦 Batch size: " + std::to_string(batch_data.size()));

    // =========================
    // 2. Parallel Inference
    // =========================
    Timer inference_timer;

    std::vector<std::future<float>> futures;

    for (const auto& sample : batch_data) {
        futures.push_back(std::async(std::launch::async, [sample]() {
            return call_python_model(sample);
        }));
    }

    std::vector<float> results;
    for (auto& f : futures) {
        results.push_back(f.get());
    }

    double inference_time = inference_timer.elapsed();

    Logger::info("⚡ Inference done in " + std::to_string(inference_time) + " sec");

    // =========================
    // 3. Post-processing
    // =========================
    float sum = std::accumulate(results.begin(), results.end(), 0.0f);
    float avg = sum / results.size();

    Logger::info("📊 Results:");
    for (size_t i = 0; i < results.size(); ++i) {
        Logger::info("  Sample " + std::to_string(i) + " -> " + std::to_string(results[i]));
    }

    Logger::info("📈 Average result: " + std::to_string(avg));

    // =========================
    // 4. Total Execution Time
    // =========================
    double total_time = total_timer.elapsed();
    Logger::info("⏱️ Total execution time: " + std::to_string(total_time) + " sec");

    Logger::info("✅ STL Engine Finished");
}
