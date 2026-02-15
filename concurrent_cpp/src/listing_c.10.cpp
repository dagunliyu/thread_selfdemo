// listing_c.10.cpp — ATM 模拟系统主入口
// 启动三个独立线程分别运行：bank_machine（银行后端）、
// interface_machine（硬件显示接口）、atm（ATM状态机）。
// 主线程读取键盘输入并通过消息队列发送给 ATM。
//
// 操作说明：
//   i — 插入银行卡（账号 acc1234）
//   0-9 — 输入 PIN 码数字（正确 PIN: 1937）
//   w — 取款 50
//   b — 查询余额
//   c — 取消操作
//   q — 退出程序

#include "listing_c.7.h"     // atm 状态机
#include "listing_c.8.h"     // bank_machine  银行后端
#include "listing_c.9.h"     // interface_machine 硬件接口

#include <thread>
#include <iostream>

int main()
{
    std::cout << "===== ATM Simulation =====" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  i     - Insert card (account: acc1234)" << std::endl;
    std::cout << "  0-9   - Enter PIN digit (correct PIN: 1937)" << std::endl;
    std::cout << "  w     - Withdraw 50" << std::endl;
    std::cout << "  b     - Display balance" << std::endl;
    std::cout << "  c     - Cancel" << std::endl;
    std::cout << "  q     - Quit" << std::endl;
    std::cout << "==========================" << std::endl;

    bank_machine bank;
    interface_machine interface_hardware;
    atm machine(bank.get_sender(),interface_hardware.get_sender());

    std::thread bank_thread(&bank_machine::run,&bank);
    std::thread if_thread(&interface_machine::run,&interface_hardware);
    std::thread atm_thread(&atm::run,&machine);

    messaging::sender atmqueue(machine.get_sender());

    bool quit_pressed=false;
    while(!quit_pressed)
    {
        char c=getchar();
        switch(c)
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            atmqueue.send(digit_pressed(c));
            break;
        case 'b':
            atmqueue.send(balance_pressed());
            break;
        case 'w':
            atmqueue.send(withdraw_pressed(50));
            break;
        case 'c':
            atmqueue.send(cancel_pressed());
            break;
        case 'q':
            quit_pressed=true;
            break;
        case 'i':
            atmqueue.send(card_inserted("acc1234"));
            break;
        }
    }

    bank.done();
    machine.done();
    interface_hardware.done();

    atm_thread.join();
    bank_thread.join();
    if_thread.join();

    return 0;
}
