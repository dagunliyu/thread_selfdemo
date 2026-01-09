//16c++17scoped_lock解决互锁造成的死锁问题////////////////////////////////////// COMMENT ///////////////////////////////////////////////

#include <thread>
#include <iostream>
#include <string>
#include <mutex>
#include <shared_mutex>
//Linux -lpthread
using namespace std;

//-----------------------------------------------------
// Test self-defined RAII-mutex
//-----------------------------------------------------
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

//-----------------------------------------------------
// Test lockGuard
//-----------------------------------------------------
static mutex gmutex;
void TestLockGuard(int i)
{
    gmutex.lock();  // 手动锁定互斥量
    {
        // adopt_lock 表示：互斥量已经被锁定
        // lock_guard 只负责在析构时解锁不会再调用 lock()
        // 离开这个作用域时，lock_guard 析构，自动调用 unlock()
        // adopt_lock配合mutex的lock使用
        lock_guard<mutex> lock(gmutex, adopt_lock);
    }

    {
        // lock_guard 不支持手动锁定和解锁，也不支持条件变量。
        // 普通用法：lock_guard 构造时自动 lock，析构时自动 unlock
        lock_guard<mutex> lock(gmutex);
        cout << "begin thread " << i << endl;
    }

    // 无限循环演示
    for (;;)
    {
        {
            lock_guard<mutex> lock(gmutex);  // 进入作用域加锁
            cout << "In " << i << endl;
        }  // 离开作用域自动解锁
        this_thread::sleep_for(500ms);  // 解锁后休眠，让其他线程有机会获取锁
    }
}

//-----------------------------------------------------
// Test deadlock and scoped_lock and std::lock
//----------------------------------------------------- 
static mutex mux1;
static mutex mux2;
void TestScope1()
{
    // 各种lock，都理解成对mutex的管理类. 实际操作的是mutex

    //模拟死锁 停100ms等另一个线程锁mux2
    this_thread::sleep_for(100ms); 
    cout <<this_thread::get_id()<< " begin mux1 lock" << endl;
    //mux1.lock();
    cout << this_thread::get_id() << " begin mux2 lock" << endl;
    //mux2.lock(); //死锁

    // C++11 方式（两步）
    // std::lock(mux1, mux2);                              // 1. 原子锁定
    // std::lock_guard<mutex> lk1(mux1, std::adopt_lock);  // 2. 接管锁
    // std::lock_guard<mutex> lk2(mux2, std::adopt_lock);
    // adopt_lock 的使用场景：
    // 当你已经通过 mutex.lock() 手动加锁，但仍想利用 lock_guard 的 RAII 特性自动解锁
    // 避免忘记解锁或异常时锁未释放

    // C++17 方式（一步）
    // 自动管理多个互斥锁的生命周期，避免死锁
    // 异常安全:若锁定过程出抛出异常，已获取的锁会自动释放
    // 一次性锁定多个mutex
    // 无死锁保证: 使用std::lock的算法，动态调整锁的获取顺序。避免线程间因锁顺序不一致导致的死锁. 
    // 对于2个mutex的场景，如果试图lock mutex2失败，则会释放mutex1. 重复操作直到成功获取全部锁
    // 在试图对所有mutex锁定时，就会去避免死锁
    scoped_lock lock(mux1, mux2); // 解决死锁
    
    cout << "TestScope1" << endl;
    this_thread::sleep_for(1000ms);
    
    //mux1.unlock();
    //mux2.unlock();
}
void TestScope2()
{
    cout << this_thread::get_id() << " begin mux2 lock" << endl;
    mux2.lock();
    
    this_thread::sleep_for(500ms);
    cout << this_thread::get_id() << " begin mux1 lock" << endl;
    
    mux1.lock();//死锁
    // mutex.lock() 获取不到锁时会阻塞（blocking），线程会一直等待直到获取到锁。   
    // mutex.lock()	阻塞等待，直到获取锁	无
    // mutex.try_lock()	不阻塞，立即返回	true=成功，false=失败
    
    cout << "TestScope2" << endl;
    this_thread::sleep_for(1500ms);

    mux1.unlock();
    mux2.unlock();
}

int main(int argc, char* argv[])
{
    //-----------------------------------------------------
    // Test deadlock and scoped_lock and std::lock
    //----------------------------------------------------- 
    {
        //演示死锁情况
        {
            // 使用scoped_lock(c++17)或std::lock(c++11)同时锁多个mutex
            thread th(TestScope1);
            th.detach();
        }
        {
            // 还使用先锁mutex1后锁mutex2之类的写法
            thread th(TestScope2);
            th.detach();
        }
    }
    getchar();

    //-----------------------------------------------------
    // Test shared_lock
    //-----------------------------------------------------
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

    //-----------------------------------------------------
    // Test unique_lock 升级版的lock_guard
    //-----------------------------------------------------k
    {
        static mutex mux;
        {
            unique_lock<mutex> lock(mux);   // 构造时加锁
            lock.unlock();                  // 可以手动解锁
            lock.lock();                    // 可以手动重新加锁
            // 析构时自动解锁
        }

        // adopt_lock 接管已有锁
        // 场景：锁已经被手动获取，用 unique_lock 来自动管理释放
        {
            mux.lock();                                 // 先手动加锁
            unique_lock<mutex> lock(mux, adopt_lock);   // 不加锁，只接管
            // 析构时解锁
        }

        // defer_lock 延迟加锁 
        // 场景：需要先创建锁对象，但稍后才加锁（如配合 std::lock 同时锁多个）
        {
            unique_lock<mutex> lock(mux, defer_lock); // 不加锁，只关联
            
            /*
             * ... 做一些准备工作 ...
             */
             
            lock.lock(); // 稍后手动加锁

            // 如果在defer_lock之后加锁了, 则离开作用域会解锁
        }

        // try_to_lock 尝试加锁，不阻塞
        // 场景：不想阻塞等待，锁不可用时做其他事
        {
            //mux.lock();
            //尝试加锁 不阻塞 失败不拥有锁
            unique_lock<mutex> lock(mux, try_to_lock); // 尝试加锁，立即返回

            if (lock.owns_lock())
            {
                // 加锁成功
                cout << "owns_lock" << endl;
            }
            else
            {
                // 加锁失败（锁被占用）
                cout << "not owns_lock" << endl;
            }
            // 如果加锁成功, 则离开作用域解锁
        }

    }

    //-----------------------------------------------------
    // Test lockGuard
    //-----------------------------------------------------
    for (int i = 0; i < 3; i++)
    {
        thread th(TestLockGuard, i + 1);
        th.detach();
    }
    
    //-----------------------------------------------------
    // Test self-defined RAII-mutex
    //-----------------------------------------------------
    TestMutex(1);
    TestMutex(2);

    getchar();
    return 0;
}