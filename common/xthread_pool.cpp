/**
 * @file xthread_pool.cpp
 * @brief XThreadPool 实现
 */
#include "xthread_pool.h"

void XThreadPool::Init(int num)
{
    std::unique_lock<std::mutex> lock(mux_);
    thread_num_ = num;
    std::cout << "Thread pool Init " << num << std::endl;
}

void XThreadPool::Start()
{
    std::unique_lock<std::mutex> lock(mux_);
    if (thread_num_ <= 0)
    {
        std::cerr << "Please Init XThreadPool first" << std::endl;
        return;
    }
    if (!threads_.empty())
    {
        std::cerr << "Thread pool already started!" << std::endl;
        return;
    }

    for (int i = 0; i < thread_num_; i++)
    {
        auto th = std::make_shared<std::thread>(&XThreadPool::Run, this);
        threads_.push_back(th);
    }
}

void XThreadPool::Stop()
{
    is_exit_ = true;
    cv_.notify_all();
    
    for (auto& th : threads_)
    {
        if (th->joinable())
            th->join();
    }
    
    std::unique_lock<std::mutex> lock(mux_);
    threads_.clear();
}

void XThreadPool::Run()
{
    std::cout << "XThreadPool thread started: " << std::this_thread::get_id() << std::endl;
    
    while (!is_exit())
    {
        auto task = GetTask();
        if (!task) continue;
        
        ++task_run_count_;
        try
        {
            auto re = task->Run();
            task->SetValue(re);
        }
        catch (...)
        {
            // 异常处理
        }
        --task_run_count_;
    }
    
    std::cout << "XThreadPool thread ended: " << std::this_thread::get_id() << std::endl;
}

void XThreadPool::AddTask(std::shared_ptr<XTask> task)
{
    std::unique_lock<std::mutex> lock(mux_);
    tasks_.push_back(task);
    task->is_exit = [this] { return is_exit(); };
    lock.unlock();
    cv_.notify_one();
}

std::shared_ptr<XTask> XThreadPool::GetTask()
{
    std::unique_lock<std::mutex> lock(mux_);
    cv_.wait(lock, [this] { return !tasks_.empty() || is_exit_; });
    
    if (is_exit_)
        return nullptr;
    if (tasks_.empty())
        return nullptr;
    
    auto task = tasks_.front();
    tasks_.pop_front();
    return task;
}
