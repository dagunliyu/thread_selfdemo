
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
   
    std::cout << "==================================" << std::endl;
    std::cout << "Test1: use TestFuture in thread" << std::endl; 
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



    std::cout << "==================================" << std::endl;
    std::cout << "Test2: catch exception through future and async" << std::endl;
    // 如果崩溃发生在thread内，而未被catch, 则会导致主线程terminate
    auto testException = []()
    {
        throw std::runtime_error("crash in lambda");
    };
    auto fut2 = std::async(std::launch::async, testException);
    try
    {
        // 子线程里抛出的异常不会丢。它会被 future 收起来。
        // 等你 get() 的时候再抛回给你。这一下就很像同步函数了。
        fut2.get();
        // wait() 很像你在问：活做完没？
        // get() 则是：活做完没？给我成果。
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    


    std::cout << "==================================" << std::endl;
    std::cout << "Test3:  sharefuture and async" << std::endl;
    auto fut3 = std::async(std::launch::async, [](){return 45;});
    // 通过future获取shared_future
    std::shared_future<int> sf3 = fut3.share();
    int res1 = sf3.get();
    int res2 = sf3.get();
    // shared_future 的直觉: 普通 future 像一份一次性的快递。shared_future 像一份复印件。你可以拿很多次。
    std::cout << "shared_future res1 = " << res1 << std::endl;
    std::cout << "shared_future res2 = " << res2 << std::endl;

    std::cout << "==================================" << std::endl;
    std::cout << "Test4: deferred async" << std::endl;
    auto func = std::async(std::launch::deferred, [](){return 45;});
    func.get();
    // 函数不是立刻跑的。而是你 get() 的那一刻。才在当前线程开始跑。



    getchar();
    return 0;
}