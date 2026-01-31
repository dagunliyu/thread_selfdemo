 
///30完成线程池退出并等待任务处理结束利用lambada表达式///////////////////////////////////// COMMENT ///////////////////////////////////////////////

#include "xthread_pool.h"
#include <iostream>
#include <spdlog/spdlog.h>
using namespace std;
class MyTask :public XTask
{
public:
    int Run()
    {
        spdlog::info("MyTask '{}' started", name);
        cout << "================================================" << endl;
        cout << this_thread::get_id()<<" Mytask " << name << endl;
        cout << "================================================" << endl;
        for (int i = 0; i < 10; i++)
        {
            if (is_exit())
            {
                spdlog::warn("MyTask '{}' received exit signal, stopping early", name);
                break;
            }
            cout << "." << flush;
            this_thread::sleep_for(500ms);
        }
        spdlog::info("MyTask '{}' completed", name);
        return 0;
    }
    std::string name = "";
};
int main(int argc, char* argv[])
{
    // 设置日志级别和格式
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");
    
    spdlog::info("=== Thread Pool 2.0 demo with spdlog ===");
    
    XThreadPool pool;
    pool.Init(16);
    spdlog::info("Thread pool initialized with 16 threads");
    pool.Start();
    spdlog::info("Thread pool started");

    MyTask task1;
    task1.name = "test name 001";
    pool.AddTask(&task1);
    spdlog::info("Added task '{}'", task1.name);
    this_thread::sleep_for(1s);
    spdlog::info("Stopping thread pool...");
    pool.Stop();
    spdlog::info("Thread pool stopped");
    getchar();
    return 0;
}