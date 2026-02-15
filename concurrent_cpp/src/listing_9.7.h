// listing_9.7.h — 工作窃取队列（work_stealing_queue）
// 基于 std::deque + std::mutex 实现。
// try_pop() 从前端取任务（本线程使用），
// try_steal() 从后端偷任务（其他线程使用），
// 实现了工作窃取调度策略。
#ifndef LISTING_9_7_H
#define LISTING_9_7_H

#include <deque>
#include <mutex>

#include "listing_9.2.h"  // function_wrapper

class work_stealing_queue
{
private:
    typedef function_wrapper data_type;
    std::deque<data_type> the_queue;
    mutable std::mutex the_mutex;
    
public:
    work_stealing_queue()
    {}

    work_stealing_queue(const work_stealing_queue& other)=delete;
    work_stealing_queue& operator=(
        const work_stealing_queue& other)=delete;

    void push(data_type data)
    {
        std::lock_guard<std::mutex> lock(the_mutex);
        the_queue.push_front(std::move(data));
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(the_mutex);
        return the_queue.empty();
    }

    bool try_pop(data_type& res)
    {
        std::lock_guard<std::mutex> lock(the_mutex);
        if(the_queue.empty())
        {
            return false;
        }
        
        res=std::move(the_queue.front());
        the_queue.pop_front();
        return true;
    }

    bool try_steal(data_type& res)
    {
        std::lock_guard<std::mutex> lock(the_mutex);
        if(the_queue.empty())
        {
            return false;
        }
        
        res=std::move(the_queue.back());
        the_queue.pop_back();
        return true;
    }
};

#endif // LISTING_9_7_H
