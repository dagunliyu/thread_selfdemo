// listing_c.8.h — 银行后端状态机（bank_machine）
// 处理 ATM 发来的消息：verify_pin（验证PIN）、withdraw（取款）、
// get_balance（查余额）、withdrawal_processed / cancel_withdrawal。
// 维护账户余额，运行在独立线程中。
#ifndef LISTING_C_8_H
#define LISTING_C_8_H

#include "listing_c.3.h"
#include "listing_c.6.h"

class bank_machine
{
    messaging::receiver incoming;
    unsigned balance_;
public:
    bank_machine():
        balance_(199)
    {}
    void done()
    {
        get_sender().send(messaging::close_queue());
    }
    void run()
    {
        try
        {
            for(;;)
            {
                incoming.wait()
                    .handle<verify_pin>(
                        [&](verify_pin const& msg)
                        {
                            if(msg.pin=="1937")
                            {
                                msg.atm_queue.send(pin_verified());
                            }
                            else
                            {
                                msg.atm_queue.send(pin_incorrect());
                            }
                        }
                        )
                    .handle<withdraw>(
                        [&](withdraw const& msg)
                        {
                            if(balance_>=msg.amount)
                            {
                                msg.atm_queue.send(withdraw_ok());
                                balance_-=msg.amount;
                            }
                            else
                            {
                                msg.atm_queue.send(withdraw_denied());
                            }
                        }
                        )
                    .handle<get_balance>(
                        [&](get_balance const& msg)
                        {
                            msg.atm_queue.send(::balance(balance_));
                        }
                        )
                    .handle<withdrawal_processed>(
                        [&](withdrawal_processed const& msg)
                        {
                        }
                        )
                    .handle<cancel_withdrawal>(
                        [&](cancel_withdrawal const& msg)
                        {
                        }
                        );
            }
        }
        catch(messaging::close_queue const&)
        {
        }
    }
    messaging::sender get_sender()
    {
        return incoming;
    }
};

#endif // LISTING_C_8_H
