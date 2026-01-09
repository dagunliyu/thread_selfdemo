/*
 * @Description: 28完成线程池的初始化和启动
 * @Author: lhc
 * @Date: 2021-02-06 20:18:25
 * @LastEditTime: 2024-01-23 23:24:48
 * @LastEditors: lhc
 * @Reference: 
 */ 

#include "xthread_pool.h"
int main(int argc, char* argv[])
{
  
    XThreadPool pool;
    pool.Init(16);
    pool.Start();
    getchar();
    return 0;
}