
#include <thread>
#include <iostream>
#include <future>
#include <string>
using namespace std;

void TestFuture(promise<string> p)
{
    cout << "begin TestFuture" << endl;
    this_thread::sleep_for(3s);
    cout << "begin set value" << endl;
    p.set_value("TestFuture value");
    this_thread::sleep_for(3s);
    cout << "end TestFuture" << endl;
}

int main(int argc, char* argv[])
{
    //异步传输变量存储
    promise<string> p;

    //用来获取线程异步值获取
    auto future = p.get_future();

    auto th = thread(TestFuture, std::move(p));

    std::cout << "==================================" << std::endl;
    std::cout << "Test1: use TestFuture in thread" << std::endl;
    cout << "begin future.get()" << endl;
    cout <<  "future get() = " << future.get() << endl;
    cout << "end future.get()" << endl;
    th.join();

    std::cout << "==================================" << std::endl;
    std::cout << "Test2: use thread without async" << std::endl;
    // 不使用async还是会把值返回回去
    std::promise<int> pInt;
    std::future<int> pFuture = pInt.get_future();

    // 在 lambda 里面新建了一个变量 pm。把外面的 pInt move 进来。然后线程里用的其实是 pm。
    // mutable 的意思是：允许这个 lambda 里修改 pm。
    std::thread ppThread([pm = std::move(pInt)]() mutable{
        pm.set_value(45); 
    });
    ppThread.join();
    int val = pFuture.get();
    std::cout << "Test2: use thread without async and future get: " << val << std::endl;


    std::cout << "==================================" << std::endl;
    std::cout << "Test3: collect exception through promise" << std::endl;
    std::promise<int> pt3;
    std::future<int> pt3Future = pt3.get_future();
    std::thread pt3Thread([pm3 = std::move(pt3)]() mutable
    {
        try
        {
            throw 1;
        }
        catch(...) //const std::exception& e
        {
            // 没有mutable不能修改promise的值
            pm3.set_exception(std::current_exception());
            // std::cerr << e.what() << '\n';
        }
    });
    pt3Thread.join();

    try
    {
        pt3Future.get();
    }
    catch (int e)
    {
        std::cout << "Captured int exception: " << e << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Captured std::exception: " << e.what() << '\n';
    }
    catch (...)
    {
        std::cerr << "Captured unknown exception" << '\n';
    }
    

    std::cout << "==================================" << std::endl;
    std::cout << "Test4: collect exception through promise without assign val" << std::endl;
    std::future<int> pt4Future;    
    {
        std::promise<int> pt4;
        pt4Future = pt4.get_future();
    }
    try
    {
        pt4Future.get();
    }
    catch (int e)
    {
        std::cout << "Captured int exception: " << e << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Captured std::exception: " << e.what() << '\n';
    }
    catch (...)
    {
        std::cerr << "Captured unknown exception" << '\n';
    }

    getchar();
    return 0;
}