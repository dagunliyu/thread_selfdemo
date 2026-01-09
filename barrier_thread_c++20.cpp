/*
 * @Description: 36c++20屏障barrier实现线程协调
 * @Author: lhc
 * @Date: 2021-02-09 18:06:36
 * @LastEditTime: 2024-01-23 23:29:06
 * @LastEditors: lhc
 * @Reference: 
 */
 
#include <thread>
#include <iostream>
#include <barrier>
using namespace std;
void TestBar(int i, barrier<>* bar)
{
    this_thread::sleep_for(chrono::seconds(i));
    cout << i << " begin wait" << endl;
    bar->wait(bar->arrive()); //期待数 -1 阻塞等待，期待为0是返回
    cout << i << " end wait" << endl;

}
int main(int argc, char* argv[])
{
    int count = 3;
    barrier bar(count);//初始数量
    for (int i = 0; i < count; i++)
    {
        thread th(TestBar, i, &bar);
        th.detach();
    }
    getchar();
    return 0;
}