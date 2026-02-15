// listing_c.6.h — ATM 消息类型定义
// 定义了 ATM 系统中所有状态机之间传递的消息结构体：
// 取款流程：withdraw, withdraw_ok, withdraw_denied, cancel_withdrawal, withdrawal_processed
// 卡片/PIN：card_inserted, digit_pressed, clear_last_pressed, eject_card
// 验证：verify_pin, pin_verified, pin_incorrect
// 余额：get_balance, balance, display_balance, balance_pressed
// 界面显示：display_enter_pin, display_enter_card, display_insufficient_funds, ...
// 操作：withdraw_pressed, cancel_pressed, issue_money
#ifndef LISTING_C_6_H
#define LISTING_C_6_H

#include "listing_c.2.h"
#include <string>

struct withdraw
{
    std::string account;
    unsigned amount;
    mutable messaging::sender atm_queue;
    withdraw(std::string const& account_,
             unsigned amount_,
             messaging::sender atm_queue_):
        account(account_),amount(amount_),
        atm_queue(atm_queue_)
    {}
};
struct withdraw_ok
{};
struct withdraw_denied
{};
struct cancel_withdrawal
{
    std::string account;
    unsigned amount;
    cancel_withdrawal(std::string const& account_,
                      unsigned amount_):
        account(account_),amount(amount_)
    {}
};
struct withdrawal_processed
{
    std::string account;
    unsigned amount;
    withdrawal_processed(std::string const& account_,
                         unsigned amount_):
        account(account_),amount(amount_)
    {}
};
struct card_inserted
{
    std::string account;
    explicit card_inserted(std::string const& account_):
        account(account_)
    {}
};
struct digit_pressed
{
    char digit;
    explicit digit_pressed(char digit_):
        digit(digit_)
    {}
};
struct clear_last_pressed
{};
struct eject_card
{};
struct withdraw_pressed
{
    unsigned amount;
    explicit withdraw_pressed(unsigned amount_):
        amount(amount_)
    {}
};
struct cancel_pressed
{};
struct issue_money
{
    unsigned amount;
    issue_money(unsigned amount_):
        amount(amount_)
    {}
};
struct verify_pin
{
    std::string account;
    std::string pin;
    mutable messaging::sender atm_queue;
    verify_pin(std::string const& account_,std::string const& pin_,
               messaging::sender atm_queue_):
        account(account_),pin(pin_),atm_queue(atm_queue_)
    {}
};
struct pin_verified
{};
struct pin_incorrect
{};
struct display_enter_pin
{};
struct display_enter_card
{};
struct display_insufficient_funds
{};
struct display_withdrawal_cancelled
{};
struct display_pin_incorrect_message
{};
struct display_withdrawal_options
{};
struct get_balance
{
    std::string account;
    mutable messaging::sender atm_queue;
    get_balance(std::string const& account_,messaging::sender atm_queue_):
        account(account_),atm_queue(atm_queue_)
    {}
};
struct balance
{
    unsigned amount;
    explicit balance(unsigned amount_):
        amount(amount_)
    {}
};
struct display_balance
{
    unsigned amount;
    explicit display_balance(unsigned amount_):
        amount(amount_)
    {}
};
struct balance_pressed
{};

#endif // LISTING_C_6_H
