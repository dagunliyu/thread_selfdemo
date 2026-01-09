 
//34基于线程池实现音视频批量转码测试任务////////////////////////////////////// COMMENT ///////////////////////////////////////////////

#pragma once
#include "xthread_pool.h"
class XVideoTask :public XTask
{
public:
    std::string in_path;
    std::string out_path;
    int width = 0;
    int height = 0;
private:
    int Run();
};

