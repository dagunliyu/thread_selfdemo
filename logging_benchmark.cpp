/*
 * @Description: Logging performance benchmark - spdlog vs Abseil log
 * @Author: 
 * @Date: 2026-01-30
 * 性能基准测试 - 比较spdlog和Abseil log在同步和异步模式下的性能
 */

#include <benchmark/benchmark.h>
#include <thread>
#include <vector>
#include <sstream>

// spdlog includes
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/async.h>

// Abseil includes
#include "absl/log/log.h"
#include "absl/log/initialize.h"
#include "absl/log/log_sink.h"
#include "absl/log/log_sink_registry.h"

// ============================================================================
// Null Sink for Abseil (to discard logs for benchmarking)
// ============================================================================
class NullLogSink : public absl::LogSink {
public:
    void Send(const absl::LogEntry& entry) override {
        // Do nothing - discard the log
    }
};

// ============================================================================
// Setup/Teardown helpers
// ============================================================================
static NullLogSink null_sink;
static bool abseil_initialized = false;

void InitializeAbseilForBenchmark() {
    if (!abseil_initialized) {
        absl::InitializeLog();
        absl::AddLogSink(&null_sink);
        abseil_initialized = true;
    }
}

// ============================================================================
// Benchmark: spdlog synchronous logging
// ============================================================================
static void BM_Spdlog_Sync_SingleThread(benchmark::State& state) {
    auto logger = spdlog::create<spdlog::sinks::null_sink_mt>("sync_bench");
    spdlog::set_default_logger(logger);
    
    for (auto _ : state) {
        spdlog::info("Benchmark message: iteration {}", state.iterations());
    }
    
    spdlog::drop("sync_bench");
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Spdlog_Sync_SingleThread);

// ============================================================================
// Benchmark: spdlog asynchronous logging
// ============================================================================
static void BM_Spdlog_Async_SingleThread(benchmark::State& state) {
    spdlog::init_thread_pool(8192, 1);
    auto logger = spdlog::create_async<spdlog::sinks::null_sink_mt>("async_bench");
    spdlog::set_default_logger(logger);
    
    for (auto _ : state) {
        spdlog::info("Benchmark message: iteration {}", state.iterations());
    }
    
    spdlog::drop("async_bench");
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Spdlog_Async_SingleThread);

// ============================================================================
// Benchmark: Abseil log synchronous logging
// ============================================================================
static void BM_Abseil_Sync_SingleThread(benchmark::State& state) {
    InitializeAbseilForBenchmark();
    
    for (auto _ : state) {
        LOG(INFO) << "Benchmark message: iteration " << state.iterations();
    }
    
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Abseil_Sync_SingleThread);

// ============================================================================
// Multi-threaded benchmarks
// ============================================================================
static void BM_Spdlog_Sync_MultiThread(benchmark::State& state) {
    if (state.thread_index() == 0) {
        // Drop existing logger if it exists
        spdlog::drop("sync_mt_bench");
        auto logger = spdlog::create<spdlog::sinks::null_sink_mt>("sync_mt_bench");
        spdlog::set_default_logger(logger);
    }
    
    for (auto _ : state) {
        spdlog::info("Thread - Benchmark message: iteration {}", state.iterations());
    }
    
    if (state.thread_index() == 0) {
        spdlog::drop("sync_mt_bench");
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Spdlog_Sync_MultiThread)->Threads(4)->Threads(8);

static void BM_Spdlog_Async_MultiThread(benchmark::State& state) {
    if (state.thread_index() == 0) {
        spdlog::drop("async_mt_bench");
        spdlog::init_thread_pool(8192, 2);
        auto logger = spdlog::create_async<spdlog::sinks::null_sink_mt>("async_mt_bench");
        spdlog::set_default_logger(logger);
    }
    
    for (auto _ : state) {
        spdlog::info("Thread - Benchmark message: iteration {}", state.iterations());
    }
    
    if (state.thread_index() == 0) {
        spdlog::drop("async_mt_bench");
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Spdlog_Async_MultiThread)->Threads(4)->Threads(8);

static void BM_Abseil_MultiThread(benchmark::State& state) {
    InitializeAbseilForBenchmark();
    
    for (auto _ : state) {
        LOG(INFO) << "Thread ID - Benchmark message: iteration " << state.iterations();
    }
    
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_Abseil_MultiThread)->Threads(4)->Threads(8);

// ============================================================================
// Benchmarks with varying message sizes
// ============================================================================
static void BM_Spdlog_Sync_VaryingSize(benchmark::State& state) {
    auto logger = spdlog::create<spdlog::sinks::null_sink_mt>("sync_size_bench");
    spdlog::set_default_logger(logger);
    
    std::string message(state.range(0), 'x');
    
    for (auto _ : state) {
        spdlog::info("Message: {}", message);
    }
    
    spdlog::drop("sync_size_bench");
    state.SetItemsProcessed(state.iterations());
    state.SetBytesProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_Spdlog_Sync_VaryingSize)->Range(8, 8192);

static void BM_Spdlog_Async_VaryingSize(benchmark::State& state) {
    spdlog::init_thread_pool(8192, 1);
    auto logger = spdlog::create_async<spdlog::sinks::null_sink_mt>("async_size_bench");
    spdlog::set_default_logger(logger);
    
    std::string message(state.range(0), 'x');
    
    for (auto _ : state) {
        spdlog::info("Message: {}", message);
    }
    
    spdlog::drop("async_size_bench");
    state.SetItemsProcessed(state.iterations());
    state.SetBytesProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_Spdlog_Async_VaryingSize)->Range(8, 8192);

static void BM_Abseil_VaryingSize(benchmark::State& state) {
    InitializeAbseilForBenchmark();
    
    std::string message(state.range(0), 'x');
    
    for (auto _ : state) {
        LOG(INFO) << "Message: " << message;
    }
    
    state.SetItemsProcessed(state.iterations());
    state.SetBytesProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_Abseil_VaryingSize)->Range(8, 8192);

BENCHMARK_MAIN();
