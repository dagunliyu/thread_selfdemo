 // 37call_once 多线程调用函数只进入一次
#include <thread>
#include <iostream>
#include <string>
#include <mutex>
//Linux -lpthread
using namespace std;

void SystemInit()
{
    cout << "Call SystemInit" << endl;
}

void SystemInitOne()
{
    // 一个控制标志，用于配合 std::call_once
    static std::once_flag flag; 
    
    // 保证 func 在多个线程中只被执行一次，无论有多少线程调用它。
    std::call_once(flag, SystemInit);
}

int main(int argc, char* argv[])
{
    // flag 是首次使用，SystemInit() 被调用，
    SystemInitOne(); // 主线程调用一次

    // flag 已标记为“已执行”，SystemInit() 不再调用，无输出。
    SystemInitOne(); // 主线程再次调用（不会重复执行）

    for (int i = 0; i < 3; i++)
    {
        // 每个线程都会执行 std::call_once(flag, SystemInit)
        // 但由于 flag 已被主线程“完成”，所以这 3 个线程不会执行 SystemInit，而是直接返回。

        thread th(SystemInitOne);   // 创建3个线程，每个都调用 SystemInitOne
        th.detach();                // 分离线程，不等待
    }
    getchar();  // 等待用户输入，防止程序提前退出
    return 0;
}