/*
 * @Description: 
 * @Author: lhc
 * @Date: 2021-01-06 20:25:02
 * @LastEditTime: 2024-01-23 23:50:29
 * @LastEditors: lhc
 * @Reference: 
 */

// 1为什么要使用多线程_第一个线程代码示例

#include <thread>
#include <iostream>
#include <spdlog/spdlog.h>
//Linux -lpthread
using namespace std;
void ThreadMain()
{
    spdlog::info("begin sub thread main");
    cout << "begin sub thread main " << this_thread::get_id() << endl;
    for (int i = 0; i < 10; i++)
    {
        spdlog::debug("in thread {}", i);
        cout << "in thread " << i << endl;
        this_thread::sleep_for(chrono::seconds(1));//1000ms
    }
    spdlog::info("end sub thread main");
    cout << "end sub thread main " << this_thread::get_id() << endl;
}
int main(int argc, char* argv[])
{
    // 设置日志级别和格式
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");
    
    spdlog::info("main thread started");
    cout << "main thread ID " << this_thread::get_id() << endl;
    //线程创建启动
    thread th(ThreadMain);
    spdlog::info("begin wait sub thread");
    cout << "begin wait sub thread  "<< endl;
    //阻塞等待子线程退出
    th.join();
    spdlog::info("end wait sub thread");
    cout << "end wait sub thread  " << endl;
    return 0;
}