/*
 * @Description: Abseil log 日志库在多线程中的使用示例
 * @Author: 
 * @Date: 2026-01-30
 * Abseil log demo - 演示在多线程程序中使用Abseil log进行日志记录
 */

#include <thread>
#include <iostream>
#include "absl/log/log.h"
#include "absl/log/initialize.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

using namespace std;

// 工作线程函数
void WorkerThread(int thread_id, int num_iterations)
{
    LOG(INFO) << "Worker thread " << thread_id << " started with " << num_iterations << " iterations";
    
    for (int i = 0; i < num_iterations; i++)
    {
        // 记录不同级别的日志
        if (i % 3 == 0) {
            VLOG(1) << "Thread " << thread_id << " - Debug message " << i;
        } else if (i % 3 == 1) {
            LOG(INFO) << "Thread " << thread_id << " - Info message " << i;
        } else {
            LOG(WARNING) << "Thread " << thread_id << " - Warning message " << i;
        }
        
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    
    LOG(INFO) << "Worker thread " << thread_id << " finished";
}

int main(int argc, char* argv[])
{
    // 初始化Abseil logging
    absl::InitializeLog();
    absl::ParseCommandLine(argc, argv);
    
    LOG(INFO) << "=== Abseil log 多线程日志示例 ===";
    LOG(INFO) << "Main thread started";
    
    // 创建并启动多个工作线程
    const int num_threads = 5;
    thread threads[num_threads];
    
    LOG(INFO) << "Starting " << num_threads << " worker threads...";
    
    for (int i = 0; i < num_threads; i++)
    {
        threads[i] = thread(WorkerThread, i + 1, 10);
    }
    
    LOG(INFO) << "All threads started, waiting for completion...";
    
    // 等待所有线程完成
    for (int i = 0; i < num_threads; i++)
    {
        threads[i].join();
        LOG(INFO) << "Thread " << i + 1 << " joined";
    }
    
    LOG(INFO) << "=== All threads completed successfully ===";
    
    // 演示不同级别的日志
    VLOG(2) << "This is a verbose level 2 message";
    VLOG(1) << "This is a verbose level 1 message";
    LOG(INFO) << "This is an info message";
    LOG(WARNING) << "This is a warning message";
    LOG(ERROR) << "This is an error message";
    // LOG(FATAL) << "This is a fatal message"; // Would terminate the program
    
    return 0;
}
