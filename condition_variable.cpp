 
//19condition_variable 读写线程同步

#include <thread>
#include <iostream>
#include <mutex>
#include <list>
#include <string>
#include <sstream>
#include <condition_variable>
#include <atomic>
using namespace std;

std::list<string> msgs_;
std::mutex mux;
std::condition_variable cv;
std::atomic<bool> running(true); // 控制线程退出

// 目的是写完再读
void ThreadWrite()
{
    int i = 0;
    // for (int i = 0;;i++)
    while(running)
    {
        stringstream ss;
        ss << "Write msg " << i;
       
        // unique_
        unique_lock<mutex> lock(mux);
        msgs_.push_back(ss.str());
        lock.unlock();
        
        //发送信号, 唤醒1个消费者
        cv.notify_one(); 
        //cv.notify_all();
        this_thread::sleep_for(3s);
    }
}

void ThreadRead(int i)
{
    for (;;)
    {
        cout << "read msg" << endl;
        unique_lock<mutex> lock(mux);
        //cv.wait(lock);//解锁、阻塞等待信号
        cv.wait(lock, [i] 
            {
                cout << i << " wait" << endl;
                return !msgs_.empty(); 
            });
        //获取信号后锁定
        while (!msgs_.empty())
        {
            cout << i << " read " << msgs_.front() << endl;
            msgs_.pop_front();
        }

    }
}
int main(int argc, char* argv[])
{
    // create 1 writeThread
    thread th(ThreadWrite);
    th.detach();

    // create 3 readthread and using detach to run thread self without block
    for (int i = 0; i < 3; i++)
    {
        thread th(ThreadRead, i + 1);
        th.detach();
    }
    getchar();
    return 0;
}