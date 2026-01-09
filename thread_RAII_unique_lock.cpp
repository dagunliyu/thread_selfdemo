 
//14unique_lock可临时解锁控制超时的互斥体包装器////////////////////////////////////// COMMENT ///////////////////////////////////////////////

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
        static mutex mux;
        {
            unique_lock<mutex> lock(mux);
         
            // 常用于：在持有锁期间需要执行耗时操作或可能阻塞的操作（如网络请求、磁盘IO），但这些操作不需要锁。
            lock.unlock(); //临时释放锁

            // ... 可以执行不需要锁的操作（例如耗时计算、IO等）

            lock.lock();   // 重新获取锁
        }// 析构时自动释放锁（如果当前持有）

        {
            // 互斥量（mutex）本身是可以直接调用 .lock() 和 .unlock() 成员函数来加锁和解锁的。这是 C++ 标准库中所有互斥类型的基本能力
            // 如果在 lock() 和 unlock() 之间抛出异常，unlock() 就不会被执行，导致：锁永远不释放/其他线程阻塞/死锁或资源泄漏

            // 推荐使用 RAII 包装器（如 lock_guard, unique_lock）,它们在构造时加锁，析构时自动解锁（即使发生异常）：

            //已经拥有锁 不锁定，退出栈区解锁
            mux.lock();

            // 现在用 unique_lock 接管，确保异常安全释放
            unique_lock<mutex> lock(mux, adopt_lock);

            // ... 操作临界资源

        } // lock 析构时自动 unlock()

        
        {
            //延后加锁 不拥有 退出栈区不解锁
            unique_lock<mutex> lock(mux, defer_lock);

            // 做一些不需要锁的操作

            //加锁 退出栈区解锁
            lock.lock();
            // ... 临界区
            // lock.unlock() // 可手动解锁
            // 
        }
        {
            //mux.lock();
            //尝试加锁 不阻塞 失败不拥有锁
            unique_lock<mutex> lock(mux, try_to_lock);

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