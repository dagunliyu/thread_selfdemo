 

//11共享锁shared_mutex解决读写问题

#include <thread>
#include <iostream>
#include <string>
#include <mutex>
#include <shared_mutex>
//Linux -lpthread
using namespace std;

//c++17  共享锁
//shared_mutex smux;  

// 不带定时的shared_mutex, 在某些平台上可能比shared_timed_mutex更有效；
// 在shared_time_mutex上删除了定时功能
// 保护共享数据不被多个线程同时访问, 与便于独占访问的其他互斥类型不同，shared_mutex 拥有二个访问级别：
// 共享 --- 多个线程能共享同一互斥的所有权。其对应的就是读的访问权限。
// 独占性 --- 仅一个线程能占有互斥。其对应的就是写的访问权限。

// 多个线程共享这一个锁，这就是读权限；
// 仅1个线程能占用该锁，这就是写权限；

//c++14 
shared_timed_mutex stmux; // 带定时的shared_mutex

// 对于多个线程进行读且不涉及写操作时，不存在数据竞争的问题。面对多线程涉及多访问，少读取的场景

// 如果频繁读数据，偶尔写数据. 在同一时刻允许多个线程读，在需要写的时候再加相应的互斥量. 可以用
// shared_mutex和shared_time_mutex

void ThreadRead(int i)
{
    for (;;)
    {
        stmux.lock_shared();
        cout << i << " Read" << endl;
        this_thread::sleep_for(500ms);
        stmux.unlock_shared();
        this_thread::sleep_for(1ms);
    }
}
void ThreadWrite(int i)
{
    for (;;)
    {
        stmux.lock_shared();
        //读取数据
        stmux.unlock_shared();
        stmux.lock(); //互斥锁 写入
        cout << i << " Write" << endl;
        this_thread::sleep_for(300ms);
        stmux.unlock();
        this_thread::sleep_for(1ms);
    }
}
int main(int argc, char* argv[])
{
    for (int i = 0; i < 3; i++)
    {
        thread th(ThreadWrite, i + 1);
        th.detach();
    }
    for (int i = 0; i < 3; i++)
    {
        thread th(ThreadRead, i + 1);
        th.detach();
    }
    getchar();
    return 0;
}