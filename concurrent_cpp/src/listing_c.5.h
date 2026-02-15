// listing_c.5.h — 模板消息分派器（TemplateDispatcher）
// 链式消息处理：每个 TemplateDispatcher 处理一种消息类型 Msg，
// 未匹配的消息委托给 PreviousDispatcher（递归链，末端为 dispatcher）。
// 同时在此文件末尾提供 dispatcher::handle() 的定义，
// 因为它需要 TemplateDispatcher 的完整定义来构造返回值。
#ifndef LISTING_C_5_H
#define LISTING_C_5_H

#include "listing_c.4.h"

namespace messaging
{
    template<typename PreviousDispatcher,typename Msg,typename Func>
    class TemplateDispatcher
    {
        queue* q;
        PreviousDispatcher* prev;
        Func f;
        bool chained;

        TemplateDispatcher(TemplateDispatcher const&)=delete;
        TemplateDispatcher& operator=(TemplateDispatcher const&)=delete;

        template<typename Dispatcher,typename OtherMsg,typename OtherFunc>
        friend class TemplateDispatcher;

        void wait_and_dispatch()
        {
            for(;;)
            {
                auto msg=q->wait_and_pop();
                if(dispatch(msg))
                    break;
            }
        }

        bool dispatch(std::shared_ptr<message_base> const& msg)
        {
            if(wrapped_message<Msg>* wrapper=
               dynamic_cast<wrapped_message<Msg>*>(msg.get()))
            {
                f(wrapper->contents);
                return true;
            }
            else
            {
                return prev->dispatch(msg);
            }
        }
    public:
        TemplateDispatcher(TemplateDispatcher&& other):
            q(other.q),prev(other.prev),f(std::move(other.f)),
            chained(other.chained)
        {
            other.chained=true;
        }

        TemplateDispatcher(queue* q_,PreviousDispatcher* prev_,Func&& f_):
            q(q_),prev(prev_),f(std::forward<Func>(f_)),chained(false)
        {
            prev_->chained=true;
        }

        template<typename OtherMsg,typename OtherFunc>
        TemplateDispatcher<TemplateDispatcher,OtherMsg,OtherFunc>
        handle(OtherFunc&& of)
        {
            return TemplateDispatcher<
                TemplateDispatcher,OtherMsg,OtherFunc>(
                    q,this,std::forward<OtherFunc>(of));
        }

        ~TemplateDispatcher() noexcept(false)
        {
            if(!chained)
            {
                wait_and_dispatch();
            }
        }
    };

    // ---- dispatcher::handle() 定义（需要完整的 TemplateDispatcher） ----
    template<typename Message,typename Func>
    TemplateDispatcher<dispatcher,Message,Func>
    dispatcher::handle(Func&& f)
    {
        return TemplateDispatcher<dispatcher,Message,Func>(
            q,this,std::forward<Func>(f));
    }
}

#endif // LISTING_C_5_H
