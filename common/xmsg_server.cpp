/**
 * @file xmsg_server.cpp
 * @brief XMsgServer 实现
 */
#include "xmsg_server.h"
#include <iostream>

void XMsgServer::SendMsg(std::string msg)
{
    std::unique_lock<std::mutex> lock(mux_);
    msgs_.push_back(std::move(msg));
    lock.unlock();
    cv_.notify_one();
}

void XMsgServer::Stop()
{
    is_exit_ = true;
    cv_.notify_all();
    Wait();
}

std::string XMsgServer::GetMsg()
{
    std::unique_lock<std::mutex> lock(mux_);
    cv_.wait(lock, [this] { return !msgs_.empty() || is_exit_; });
    
    if (is_exit_ && msgs_.empty())
        return "";
    
    auto msg = msgs_.front();
    msgs_.pop_front();
    return msg;
}

void XMsgServer::Main()
{
    while (!is_exit())
    {
        auto msg = GetMsg();
        if (msg.empty()) continue;
        
        // 子类可以重写Main来处理消息
        std::cout << "[XMsgServer] received: " << msg << std::endl;
    }
}
