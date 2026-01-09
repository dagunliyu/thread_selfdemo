/**
 * @file xthread.cpp
 * @brief XThread 基类实现
 */
#include "xthread.h"

void XThread::Start()
{
    is_exit_ = false;
    th_ = std::thread(&XThread::Main, this);
}

void XThread::Stop()
{
    is_exit_ = true;
    Wait();
}

void XThread::Wait()
{
    if (th_.joinable())
        th_.join();
}

bool XThread::is_exit() const
{
    return is_exit_;
}
