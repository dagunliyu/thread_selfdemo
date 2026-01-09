/*
 * @Description: 
 * @Author: lhc
 * @Date: 2026-01-09 10:26:31
 * @LastEditTime: 2026-01-09 15:04:52
 * @LastEditors: lhc
 * @Reference: 
 */
 
//22packaged_task 异步调用函数打包////////////////////////////////////// COMMENT ///////////////////////////////////////////////

#include <thread>
#include <iostream>
#include <future>
#include <string>
using namespace std;

string TestPack(int index)
{
    cout << "begin Test Pack " << index << endl;
    this_thread::sleep_for(2s);
    return "Test Pack return";
}
int main(int argc, char* argv[])
{
    packaged_task< string(int) > task(TestPack);
    auto result = task.get_future();
    //task(100);
    thread th(move(task),101);
    
    cout << "begin result get" << endl;

    //测试是否超时
    for (int i = 0; i < 30; i++)
    {
        if (result.wait_for(100ms) != future_status::ready)
        {
            continue;
        }
    }
    if (result.wait_for(100ms) == future_status::timeout)
    {
        cout << "wait result timeout" << endl;
    }
    else
        cout << "result get " << result.get() << endl;
    th.join();
    getchar();
    return 0;
}