/**
 * @file xthread_pool.h
 * @brief 线程池实现，支持异步任务执行和结果获取
 */
#pragma once
#include <thread>
#include <mutex>
#include <vector>
#include <list>
#include <functional>
#include <atomic>
#include <future>
#include <memory>
#include <condition_variable>
#include <iostream>

/**
 * @brief 任务基类
 */
class XTask
{
public:
    virtual ~XTask() = default;
    
    /// 任务执行入口，子类实现具体逻辑
    virtual int Run() = 0;
    
    /// 判断任务是否应该退出的回调
    std::function<bool()> is_exit = nullptr;
    
    /// 阻塞等待并获取任务返回值
    auto GetReturn()
    {
        return p_.get_future().get();
    }
    
    /// 设置任务返回值
    void SetValue(int v)
    {
        p_.set_value(v);
    }

private:
    std::promise<int> p_;
};

/**
 * @brief 线程池类
 */
class XThreadPool
{
public:
    ~XThreadPool() { Stop(); }

    /// 初始化线程池
    /// @param num 线程数量
    void Init(int num);

    /// 启动所有线程，必须先调用Init
    void Start();

    /// 线程池退出
    void Stop();

    /// 添加任务到线程池
    void AddTask(std::shared_ptr<XTask> task);

    /// 获取一个待执行的任务
    std::shared_ptr<XTask> GetTask();

    /// 线程池是否退出
    bool is_exit() const { return is_exit_; }

    /// 获取正在运行的任务数量
    int task_run_count() const { return task_run_count_; }

private:
    /// 线程池工作线程入口函数
    void Run();

    int thread_num_ = 0;
    std::mutex mux_;
    std::vector<std::shared_ptr<std::thread>> threads_;
    std::list<std::shared_ptr<XTask>> tasks_;
    std::condition_variable cv_;
    bool is_exit_ = false;
    std::atomic<int> task_run_count_{0};
};
