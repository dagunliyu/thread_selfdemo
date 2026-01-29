/*
 * @Description: spdlog 日志库在多线程中的使用示例
 * @Author: 
 * @Date: 2026-01-29
 * spdlog demo - 演示在多线程程序中使用spdlog进行日志记录
 */

#include <thread>
#include <iostream>
#include <spdlog/spdlog.h>

using namespace std;

// 工作线程函数
void WorkerThread(int thread_id, int num_iterations)
{
    spdlog::info("Worker thread {} started with {} iterations", thread_id, num_iterations);
    
    for (int i = 0; i < num_iterations; i++)
    {
        // 记录不同级别的日志
        if (i % 3 == 0) {
            spdlog::debug("Thread {} - Debug message {}", thread_id, i);
        } else if (i % 3 == 1) {
            spdlog::info("Thread {} - Info message {}", thread_id, i);
        } else {
            spdlog::warn("Thread {} - Warning message {}", thread_id, i);
        }
        
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    
    spdlog::info("Worker thread {} finished", thread_id);
}

int main(int argc, char* argv[])
{
    // 设置日志级别为 debug（显示所有级别的日志）
    spdlog::set_level(spdlog::level::debug);
    
    // 设置日志格式：[时间] [级别] [线程ID] 消息
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");
    
    spdlog::info("=== spdlog 多线程日志示例 ===");
    spdlog::info("Main thread started");
    
    // 创建并启动多个工作线程
    const int num_threads = 5;
    thread threads[num_threads];
    
    spdlog::info("Starting {} worker threads...", num_threads);
    
    for (int i = 0; i < num_threads; i++)
    {
        threads[i] = thread(WorkerThread, i + 1, 10);
    }
    
    spdlog::info("All threads started, waiting for completion...");
    
    // 等待所有线程完成
    for (int i = 0; i < num_threads; i++)
    {
        threads[i].join();
        spdlog::info("Thread {} joined", i + 1);
    }
    
    spdlog::info("=== All threads completed successfully ===");
    
    // 演示不同级别的日志
    spdlog::trace("This is a trace message");
    spdlog::debug("This is a debug message");
    spdlog::info("This is an info message");
    spdlog::warn("This is a warning message");
    spdlog::error("This is an error message");
    spdlog::critical("This is a critical message");
    
    return 0;
}
