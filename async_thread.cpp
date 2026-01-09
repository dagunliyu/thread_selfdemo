
#include <thread>
#include <iostream>
#include <future>
#include <string>
using namespace std;

string TestAsync(int index)
{
    cout << index<<" begin in TestAsync " << this_thread::get_id() << endl;
    this_thread::sleep_for(2s);
    return "TestAsync string return";
}
int main(int argc, char* argv[])
{
   //创建异步线程  23async创建异步线程替代thread
    
    cout << "main thread id " << this_thread::get_id() << endl;
    //不创建线程启动异步任务
    auto future = async(launch::deferred, TestAsync,100);
    this_thread::sleep_for(100ms);
    cout << "begin future get " << endl;
    cout << "future.get() = " << future.get() << endl;
    cout << "end future get" << endl;

    //创建异步线程
    cout << "=====创建异步线程====" << endl;
    auto future2 = async(TestAsync, 101);
    this_thread::sleep_for(100ms);
    cout << "begin future2 get " << endl;
    cout << "future2.get() = " << future2.get() << endl;
    cout << "end future2 get" << endl;


    getchar();
    return 0;
}