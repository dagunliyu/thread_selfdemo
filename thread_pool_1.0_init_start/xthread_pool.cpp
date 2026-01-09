 
////28完成线程池的初始化和启动//////////////////////////////////// COMMENT ///////////////////////////////////////////////

#include "xthread_pool.h"
#include <iostream>
using namespace std;

//////////////////////////////////////////////
/// 初始化线程池
/// @para num 线程数量
void XThreadPool::Init(int num)
{
    unique_lock<mutex> lock(mux_);
    this->thread_num_ = num;
    cout << "Thread pool Init " << num << endl;
}

//////////////////////////////////////////////
/// 启动所有线程，必须先调用Init
void XThreadPool::Start()
{
    unique_lock<mutex> lock(mux_);
    if (thread_num_ <= 0)
    {
        cerr << "Please Init XThreadPool" << endl;
        return;
    }
    if (!threads_.empty())
    {
        cerr << "Thread pool has start!" << endl;
        return;
    }
    //启动线程
    for (int i = 0; i < thread_num_; i++)
    {
        auto th = new thread(&XThreadPool::Run, this);
        threads_.push_back(th);
    }
}

//线程池线程的入口函数
void XThreadPool::Run()
{
    cout << "begin XThreadPool Run " << this_thread::get_id() << endl;

    cout << "end XThreadPool Run " << this_thread::get_id() << endl;
}

