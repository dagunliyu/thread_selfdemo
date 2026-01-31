 
///30����̳߳��˳����ȴ���������������lambada����ʽ///////////////////////////////////// COMMENT ///////////////////////////////////////////////

#pragma once
#include <thread>
#include <mutex>
#include <vector>
#include <list>
#include <functional>
#include <condition_variable>
class XTask
{
public:
    virtual int Run() = 0;
    std::function<bool()> is_exit = nullptr;
};

class XThreadPool
{
public:
    //////////////////////////////////////////////
    /// ��ʼ���̳߳�
    /// @para num �߳�����
    void Init(int num);

    //////////////////////////////////////////////
    /// ���������̣߳������ȵ���Init
    void Start();

    //////////////////////////////////////////////
    /// �̳߳��˳�
    void Stop();

    void AddTask(XTask* task);

    XTask* GetTask();

    //�̳߳��Ƿ��˳�
    bool is_exit() { return is_exit_; }

private:
    //�̳߳��̵߳���ں���
    void Run() ;
    int thread_num_ = 0;//�߳�����
    std::mutex mux_;
    std::vector<std::thread*> threads_;
    std::list<XTask*> tasks_;
    std::condition_variable cv_;
    bool is_exit_ = false; //�̳߳��˳�
};

