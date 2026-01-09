
//6lambda临时函数作为线程入口////////////////////////////////////// COMMENT ///////////////////////////////////////////////

#include <thread>
#include <iostream>
#include <string>
//Linux -lpthread
using namespace std;
// test lambda thread
class TestLambda
{
public:
    void Start()
    {
        thread th([this]() {cout << "name = " << name << endl; });
        th.join();
    }

    string name = "test lambda";
};
int main(int argc, char* argv[])
{
    thread th(
        [](int i) {cout << "test lmbda " << i << endl; },
        123
    );

    // 调用了这个函数的线程对象，一定要等这个线程对象的方法（在构造时传入的方法）执行完毕后
    // （或者理解为这个线程的活干完了！），这个join()函数才能得到返回。
    
    // 通过join在这里阻塞线程, 直到
    th.join();
    
    
    TestLambda test;
    test.Start();

    return 0;
}