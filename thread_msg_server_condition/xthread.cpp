 
//20条件变量应用线程通信解决线程退出时的阻塞问题////////////////////////////////////// COMMENT ///////////////////////////////////////////////

#include "xthread.h"
using namespace std;
//启动线程
void XThread::Start()
{
    is_exit_ = false;
    th_ = thread(&XThread::Main, this);
}

//设置线程退出标志 并等待
void XThread::Stop()
{
    is_exit_ = true;
    Wait();
}

//等待线程退出（阻塞）
void XThread::Wait()
{
    if (th_.joinable())
        th_.join();
}

//线程是否退出
bool XThread::is_exit()
{
    return is_exit_;
}