/**
 * @file xthread.h
 * @brief 线程基类封装，提供统一的线程启动/停止/等待接口
 */
#pragma once
#include <thread>

class XThread
{
public:
    virtual ~XThread() = default;

    /// 启动线程
    virtual void Start();

    /// 设置线程退出标志并等待线程结束
    virtual void Stop();

    /// 等待线程退出（阻塞）
    virtual void Wait();

    /// 检查线程是否应该退出
    bool is_exit() const;

protected:
    bool is_exit_ = false;

private:
    /// 线程入口函数，子类必须实现
    virtual void Main() = 0;

    std::thread th_;
};
