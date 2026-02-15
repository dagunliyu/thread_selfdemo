// listing_8.12.h — 线程屏障（barrier）
// 基于 std::atomic 的线程同步屏障。
// 所有参与线程调用 wait() 后，最后到达的线程重置 spaces 并递增 generation，
// 其他线程通过自旋检测 generation 变化来恢复执行。
#ifndef LISTING_8_12_H
#define LISTING_8_12_H

#include <thread>
#include <atomic>

class barrier
{
    unsigned const count;
    std::atomic<unsigned> spaces;
    std::atomic<unsigned> generation;
public:
    explicit barrier(unsigned count_):
        count(count_),spaces(count_),generation(0)
    {}
    void wait()
    {
        unsigned const my_generation=generation.load();
        if(!--spaces)
        {
            spaces=count;
            ++generation;
        }
        else
        {
            while(generation.load()==my_generation)
                std::this_thread::yield();
        }
    }
};

#endif // LISTING_8_12_H
