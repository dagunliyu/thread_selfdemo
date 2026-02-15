// listing_c.1.h — 线程安全消息队列（ATM消息传递框架基础）
// 提供类型擦除的消息基类 message_base、类型化包装 wrapped_message<Msg>，
// 以及基于 mutex + condition_variable 的线程安全消息队列 queue。
#ifndef LISTING_C_1_H
#define LISTING_C_1_H

#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>

namespace messaging
{
    // 消息基类，用于类型擦除
    struct message_base
    {
        virtual ~message_base()
        {}
    };

    // 类型化消息包装器，通过多态存入队列
    template<typename Msg>
    struct wrapped_message:
        message_base
    {
        Msg contents;
        explicit wrapped_message(Msg const& contents_):
            contents(contents_)
        {}
    };

    // 线程安全的消息队列
    class queue
    {
        std::mutex m;
        std::condition_variable c;
        std::queue<std::shared_ptr<message_base> > q;
    public:
        template<typename T>
        void push(T const& msg)
        {
            std::lock_guard<std::mutex> lk(m);
            q.push(std::make_shared<wrapped_message<T> >(msg));
            c.notify_all();
        }
        std::shared_ptr<message_base> wait_and_pop()
        {
            std::unique_lock<std::mutex> lk(m);
            c.wait(lk,[&]{return !q.empty();});
            auto res=q.front();
            q.pop();
            return res;
        }
    };
}

#endif // LISTING_C_1_H
