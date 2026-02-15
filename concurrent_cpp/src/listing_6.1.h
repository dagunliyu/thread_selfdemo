// listing_6.1.h — 线程安全栈（threadsafe_stack）
// 基于 std::mutex + std::lock_guard 的线程安全栈。
// 所有操作（push、pop、empty）均在互斥量保护下执行。
// pop() 提供两种重载：返回 shared_ptr 或通过引用参数输出。
#ifndef LISTING_6_1_H
#define LISTING_6_1_H

#include <exception>
#include <stack>
#include <mutex>
#include <memory>

struct empty_stack: std::exception
{
    const char* what() const noexcept override
    {
        return "empty stack";
    }
};

template<typename T>
class threadsafe_stack
{
private:
    std::stack<T> data;
    mutable std::mutex m;
public:
    threadsafe_stack(){}
    threadsafe_stack(const threadsafe_stack& other)
    {
        std::lock_guard<std::mutex> lock(other.m);
        data=other.data;
    }
    threadsafe_stack& operator=(const threadsafe_stack&) = delete;

    void push(T new_value)
    {
        std::lock_guard<std::mutex> lock(m);
        data.push(std::move(new_value));
    }
    std::shared_ptr<T> pop()
    {
        std::lock_guard<std::mutex> lock(m);
        if(data.empty()) throw empty_stack();
        std::shared_ptr<T> const res(
            std::make_shared<T>(std::move(data.top())));
        data.pop();
        return res;
    }
    void pop(T& value)
    {
        std::lock_guard<std::mutex> lock(m);
        if(data.empty()) throw empty_stack();
        value=std::move(data.top());
        data.pop();
    }
    bool empty() const
    {
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }
};

#endif // LISTING_6_1_H
