// listing_8.8.h — 递归并行 for_each
// 基于 std::async 的分治并行遍历。
// 当范围足够小时使用 std::for_each 串行处理；
// 否则从中点分为两半，一半异步执行，一半在当前线程处理。
#ifndef LISTING_8_8_H
#define LISTING_8_8_H

#include <future>
#include <algorithm>
#include <iterator>

template<typename Iterator,typename Func>
void parallel_for_each(Iterator first,Iterator last,Func f)
{
    unsigned long const length=std::distance(first,last);

    if(!length)
        return;

    unsigned long const min_per_thread=25;

    if(length<(2*min_per_thread))
    {
        std::for_each(first,last,f);
    }
    else
    {
        Iterator const mid_point=first+(length/2);
        std::future<void> first_half=
            std::async(&parallel_for_each<Iterator,Func>,
                       first,mid_point,f);
        parallel_for_each(mid_point,last,f);
        first_half.get();
    }
}

#endif // LISTING_8_8_H
