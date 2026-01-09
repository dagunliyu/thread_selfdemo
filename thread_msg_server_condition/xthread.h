 
//20条件变量应用线程通信解决线程退出时的阻塞问题////////////////////////////////////// COMMENT ///////////////////////////////////////////////

#pragma once
#include <thread>
class XThread
{
public:
    //启动线程
    virtual void Start();

    //设置线程退出标志 并等待
    virtual void Stop();

    //等待线程退出（阻塞）
    virtual void Wait();

    //线程是否退出
    bool is_exit();
protected:
    bool is_exit_ = false;
private:
    //线程入口
    virtual void Main() = 0;

    std::thread th_;
};

