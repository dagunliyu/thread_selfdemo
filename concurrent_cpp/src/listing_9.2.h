// listing_9.2.h — 可移动类型擦除函数包装器（function_wrapper）+ 简易线程池
// function_wrapper 通过虚函数实现类型擦除，支持移动语义但禁止拷贝，
// 可以包装任意可调用对象（包括 std::packaged_task 等不可拷贝类型）。
// thread_pool 使用 function_wrapper 和 threadsafe_queue 管理工作线程。
// submit() 返回 std::future 以获取异步结果。
#ifndef LISTING_9_2_H
#define LISTING_9_2_H

#include <deque>
#include <future>
#include <memory>
#include <functional>
#include <thread>
#include <vector>
#include <atomic>

#include "listing_6.2.h"  // threadsafe_queue

// ---- 可移动函数包装（type-erased callable wrapper） ----
class function_wrapper
{
    struct impl_base {
        virtual void call()=0;
        virtual ~impl_base() {}
    };
    std::unique_ptr<impl_base> impl;
    template<typename F>
    struct impl_type: impl_base
    {
        F f;
        impl_type(F&& f_): f(std::move(f_)) {}
        void call() { f(); }
    };
public:
    function_wrapper() = default;

    template<typename F>
    function_wrapper(F&& f):
        impl(new impl_type<F>(std::move(f)))
    {}

    void operator()() { impl->call(); }
    void call() { impl->call(); }

    function_wrapper(function_wrapper&& other):
        impl(std::move(other.impl))
    {}

    function_wrapper& operator=(function_wrapper&& other)
    {
        impl=std::move(other.impl);
        return *this;
    }

    function_wrapper(const function_wrapper&)=delete;
    function_wrapper(function_wrapper&)=delete;
    function_wrapper& operator=(const function_wrapper&)=delete;
};

// ---- RAII 线程合并守卫 ----
class join_threads
{
    std::vector<std::thread>& threads;
public:
    explicit join_threads(std::vector<std::thread>& threads_):
        threads(threads_)
    {}
    ~join_threads()
    {
        for(unsigned long i=0;i<threads.size();++i)
        {
            if(threads[i].joinable())
                threads[i].join();
        }
    }
};

// ---- 线程池 ----
class thread_pool
{
    std::atomic_bool done;
    threadsafe_queue<function_wrapper> work_queue;
    std::vector<std::thread> threads;
    join_threads joiner;

    void worker_thread()
    {
        while(!done)
        {
            function_wrapper task;
            if(work_queue.try_pop(task))
            {
                task();
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }

public:
    thread_pool():
        done(false),joiner(threads)
    {
        unsigned const thread_count=std::thread::hardware_concurrency();
        try
        {
            for(unsigned i=0;i<thread_count;++i)
            {
                threads.push_back(
                    std::thread(&thread_pool::worker_thread,this));
            }
        }
        catch(...)
        {
            done=true;
            throw;
        }
    }

    ~thread_pool()
    {
        done=true;
    }

    template<typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type>
    submit(FunctionType f)
    {
        typedef typename std::result_of<FunctionType()>::type result_type;
        
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> res(task.get_future());
        work_queue.push(std::move(task));
        return res;
    }

    void run_pending_task()
    {
        function_wrapper task;
        if(work_queue.try_pop(task))
        {
            task();
        }
        else
        {
            std::this_thread::yield();
        }
    }
};

#endif // LISTING_9_2_H
