 
///15 C++14shared_lock共享锁包装器

#include <thread>
#include <iostream>
#include <string>
#include <mutex>
#include <shared_mutex>
//Linux -lpthread
using namespace std;
// RAII
class XMutex
{
public:
    XMutex(mutex& mux):mux_(mux)
    {
        cout << "Lock" << endl;
        mux.lock();
    }
    ~XMutex()
    {
        cout << "Unlock" << endl;
        mux_.unlock();
    }
private:
    mutex& mux_;
};
static mutex mux;
void TestMutex(int status)
{
    XMutex lock(mux);
    if (status == 1)
    {
        cout << "=1" << endl;
        return;
    }
    else
    {
        cout << "!=1" << endl;
        return;
    }
}

static mutex gmutex;
void TestLockGuard(int i)
{
    gmutex.lock();
    {
        //已经拥有锁，不lock
        lock_guard<mutex> lock(gmutex,adopt_lock);
        //结束释放锁
    }
    {
        lock_guard<mutex> lock(gmutex);
        cout << "begin thread " << i << endl;
    }
    for (;;)
    {
        {
            lock_guard<mutex> lock(gmutex);
            cout << "In " << i << endl;
        }
        this_thread::sleep_for(500ms);
    }
}



int main(int argc, char* argv[])
{

    {
        //共享锁
        static shared_timed_mutex  tmux;
        //读取锁 共享锁
        {
            //调用共享锁 
            shared_lock<shared_timed_mutex> lock(tmux);
            cout << "read data" << endl;
            //退出栈区 释放共享锁
        }
        //写入锁 互斥锁
        {
            unique_lock<shared_timed_mutex> lock(tmux);
            cout << "write data" << endl;
        }

    }


    {
        static mutex mux;
        {
            unique_lock<mutex> lock(mux);
            lock.unlock();
            //临时释放锁
            lock.lock();
        }

        {
            //已经拥有锁 不锁定，退出栈区解锁
            mux.lock();
            unique_lock<mutex> lock(mux, adopt_lock);
        }
        {
            //延后加锁 不拥有 退出栈区不解锁
            unique_lock<mutex> lock(mux, defer_lock);
            //加锁 退出栈区解锁
            lock.lock();
        }
        {
            //mux.lock();
            //尝试加锁 不阻塞 失败不拥有锁
            //unique_lock<mutex> lock(mux, try_to_lock);
            //锁超时 阻塞指定时间 失败不拥有锁
            unique_lock<mutex> lock(mux, 100ms);

            if (lock.owns_lock())
            {
                cout << "owns_lock" << endl;
            }
            else
            {
                cout << "not owns_lock" << endl;
            }
        }

    }

    for (int i = 0; i < 3; i++)
    {
        thread th(TestLockGuard, i + 1);
        th.detach();
    }
    TestMutex(1);
    TestMutex(2);

    getchar();
    return 0;
}