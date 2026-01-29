 
//8竞争状态和临界区介绍_互斥锁mutex代码演示////////////////////////////////////// COMMENT ///////////////////////////////////////////////

#include <thread>
#include <iostream>
#include <string>
#include <mutex>
#include <spdlog/spdlog.h>
//Linux -lpthread
using namespace std;
static mutex mux;
void TestThread()
{
    for(;;)
    {
        //获取锁资源，如果没有则阻塞等待
        //mux.lock(); //
        if (!mux.try_lock())
        {
            spdlog::debug("Failed to acquire lock, retrying...");
            cout << "." << flush;
            this_thread::sleep_for(100ms);
            continue;
        }
        spdlog::info("Acquired lock, executing critical section");
        cout << "==============================" << endl;
        cout << "test 001" << endl;
        cout << "test 002" << endl;
        cout << "test 003" << endl;
        cout << "==============================" << endl;
        spdlog::info("Releasing lock");
        mux.unlock();
        this_thread::sleep_for(1000ms);
    }
}
int main(int argc, char* argv[])
{
    // 设置日志级别和格式
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");
    
    spdlog::info("=== Mutex demo with spdlog ===");
    for (int i = 0; i < 10; i++)
    {
        thread th(TestThread);
        th.detach();
        spdlog::info("Started thread {}", i + 1);
    }
    getchar();
    return 0;
}