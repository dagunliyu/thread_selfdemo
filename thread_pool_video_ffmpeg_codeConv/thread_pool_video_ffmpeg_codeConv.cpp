 
//34基于线程池实现音视频批量转码测试任务////////////////////////////////////// COMMENT ///////////////////////////////////////////////

#include "xthread_pool.h"
#include "xvideo_task.h"
#include <iostream>
using namespace std;
/// 命令行视频转码工具
/// ffmpeg工具
/// 用户输入 视频源 输出视频尺寸
/// 在线程池中执行转码任务
/// 转码任务调用ffmpeg
/// ffmpeg -y -i test.mp4 -s 400x300 400.mp4 >log.txt 2>&1

int main(int argc, char* argv[])
{
    XThreadPool pool;
    pool.Init(16);
    pool.Start();
    auto vtask1 = make_shared<XVideoTask>();
    vtask1->in_path = "test.mp4";
    vtask1->out_path = "640.mp4";
    vtask1->width = 640;
    vtask1->height = 480;
    pool.AddTask(vtask1);

    vtask1->GetReturn();
    
    return 0;
}