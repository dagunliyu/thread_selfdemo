// listing_c.2.h — 消息发送者（sender）
// 封装指向 messaging::queue 的指针，提供类型安全的 send() 方法。
#ifndef LISTING_C_2_H
#define LISTING_C_2_H

#include "listing_c.1.h"

namespace messaging
{
    class sender
    {
        queue* q;
    public:
        sender():
            q(nullptr)
        {}
        explicit sender(queue* q_):
            q(q_)
        {}
        template<typename Message>
        void send(Message const& msg)
        {
            if(q)
            {
                q->push(msg);
            }
        }
    };
}

#endif // LISTING_C_2_H
