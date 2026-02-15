// listing_8.5.h — 递归并行累加（parallel_accumulate）
// 基于 std::async 的分治并行累加。
// 当数据量超过阈值时，递归将范围对半分割，
// 一半提交给 std::async 异步执行，另一半在当前线程处理。
// 运行时自动管理线程数量，避免线程过度创建。
#ifndef LISTING_8_5_H
#define LISTING_8_5_H

#include <future>
#include <algorithm>
#include <numeric>
#include <iterator>

template<typename Iterator,typename T>
T parallel_accumulate(Iterator first,Iterator last,T init)
{
    unsigned long const length=std::distance(first,last);
    unsigned long const max_chunk_size=25;
    if(length<=max_chunk_size)
    {
        return std::accumulate(first,last,init);
    }
    else
    {
        Iterator mid_point=first;
        std::advance(mid_point,length/2);
        std::future<T> first_half_result=
            std::async(parallel_accumulate<Iterator,T>,
                       first,mid_point,init);
        T second_half_result=parallel_accumulate(mid_point,last,T());
        return first_half_result.get()+second_half_result;
    }
}

#endif // LISTING_8_5_H
