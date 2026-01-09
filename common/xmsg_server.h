/**
 * @file xmsg_server.h
 * @brief 消息服务器类，基于条件变量实现线程间消息通信
 */
#pragma once
#include "xthread.h"
#include <string>
#include <list>
#include <mutex>
#include <condition_variable>

class XMsgServer : public XThread
{
public:
    /// 给当前线程发消息
    void SendMsg(std::string msg);

    /// 重写Stop，唤醒等待中的线程
    void Stop() override;

protected:
    /// 从消息队列中获取一条消息（阻塞）
    std::string GetMsg();

private:
    /// 处理消息的线程入口函数
    void Main() override;

    /// 消息队列缓冲
    std::list<std::string> msgs_;

    /// 互斥访问消息队列
    std::mutex mux_;

    /// 条件变量，用于等待消息
    std::condition_variable cv_;
};
