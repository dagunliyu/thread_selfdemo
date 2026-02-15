// listing_c.3.h — 消息接收者（receiver）
// 拥有一个 messaging::queue 实例。可隐式转换为 sender（提供写入端），
// wait() 返回 dispatcher 以开始消息分派循环。
#ifndef LISTING_C_3_H
#define LISTING_C_3_H

#include "listing_c.1.h"
#include "listing_c.2.h"
#include "listing_c.5.h"  // 包含 dispatcher + TemplateDispatcher 完整定义

namespace messaging
{
    class receiver
    {
        queue q;
    public:
        operator sender()
        {
            return sender(&q);
        }
        dispatcher wait()
        {
            return dispatcher(&q);
        }
    };
}

#endif // LISTING_C_3_H
