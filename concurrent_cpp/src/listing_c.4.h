// listing_c.4.h — 消息分派器（dispatcher）+ 关闭队列异常
// close_queue 用作终止消息循环的异常类型。
// dispatcher 从队列中取消息并分派；通过 handle<Msg>() 链式注册处理器，
// 未匹配的消息若为 close_queue 则抛出异常以退出循环。
// 注意：dispatcher::handle() 的定义在 listing_c.5.h 中，因为它依赖 TemplateDispatcher。
#ifndef LISTING_C_4_H
#define LISTING_C_4_H

#include "listing_c.1.h"
#include <memory>

namespace messaging
{
    class close_queue
    {};

    // 前向声明 TemplateDispatcher
    template<
        typename PreviousDispatcher,
        typename Msg,
        typename Func>
    class TemplateDispatcher;

    class dispatcher
    {
        queue* q;
        bool chained;

        dispatcher(dispatcher const&)=delete;
        dispatcher& operator=(dispatcher const&)=delete;

        template<
            typename Dispatcher,
            typename Msg,
            typename Func>
        friend class TemplateDispatcher;

        void wait_and_dispatch()
        {
            for(;;)
            {
                auto msg=q->wait_and_pop();
                dispatch(msg);
            }
        }

        bool dispatch(
            std::shared_ptr<message_base> const& msg)
        {
            if(dynamic_cast<wrapped_message<close_queue>*>(msg.get()))
            {
                throw close_queue();
            }
            return false;
        }
    public:
        dispatcher(dispatcher&& other):
            q(other.q),chained(other.chained)
        {
            other.chained=true;
        }

        explicit dispatcher(queue* q_):
            q(q_),chained(false)
        {}

        // handle() 声明——定义见 listing_c.5.h（依赖完整的 TemplateDispatcher）
        template<typename Message,typename Func>
        TemplateDispatcher<dispatcher,Message,Func>
        handle(Func&& f);

        ~dispatcher() noexcept(false)
        {
            if(!chained)
            {
                wait_and_dispatch();
            }
        }
    };
}

#endif // LISTING_C_4_H
