// listing_7.1.h — 自旋锁（spinlock_mutex）
// 基于 std::atomic_flag 实现的自旋互斥量。
// lock() 使用 test_and_set (acquire) 自旋等待；
// unlock() 使用 clear (release) 释放。
// 满足 Lockable 要求，可与 std::lock_guard 等配合使用。
#ifndef LISTING_7_1_H
#define LISTING_7_1_H

#include <atomic>

class spinlock_mutex
{
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:
    spinlock_mutex()
    {}
    void lock()
    {
        while(flag.test_and_set(std::memory_order_acquire));
    }
    void unlock()
    {
        flag.clear(std::memory_order_release);
    }
};

#endif // LISTING_7_1_H
