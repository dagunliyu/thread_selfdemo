// test_ch7.cpp — 第7章：无锁并发数据结构测试
// 测试 spinlock_mutex（自旋锁）和 lock_free_stack（无锁栈）。
// 包含基本功能验证和多线程并发正确性测试。

#include "test_framework.h"
#include "listing_7.1.h"   // spinlock_mutex
#include "listing_7.13.h"  // lock_free_stack

#include <thread>
#include <vector>
#include <atomic>
#include <mutex>  // for std::lock_guard

// =====================================================
// spinlock_mutex 测试
// =====================================================

TEST(spinlock_basic_lock_unlock)
{
    spinlock_mutex sm;
    sm.lock();
    sm.unlock();  // 不死锁即通过
    ASSERT_TRUE(true);
}

TEST(spinlock_with_lock_guard)
{
    spinlock_mutex sm;
    {
        std::lock_guard<spinlock_mutex> lk(sm);
        // 在锁保护下操作
    }
    // lock_guard 析构自动解锁
    ASSERT_TRUE(true);
}

TEST(spinlock_concurrent_counter)
{
    spinlock_mutex sm;
    int counter = 0;
    const int num_threads = 8;
    const int increments = 10000;

    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t)
    {
        threads.emplace_back([&sm, &counter, increments]() {
            for (int i = 0; i < increments; ++i)
            {
                std::lock_guard<spinlock_mutex> lk(sm);
                ++counter;
            }
        });
    }
    for (auto& t : threads) t.join();

    ASSERT_EQ(counter, num_threads * increments);
}

TEST(spinlock_protects_shared_data)
{
    spinlock_mutex sm;
    std::vector<int> shared_data;
    const int num_threads = 4;
    const int items_per_thread = 1000;

    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t)
    {
        threads.emplace_back([&sm, &shared_data, t, items_per_thread]() {
            for (int i = 0; i < items_per_thread; ++i)
            {
                std::lock_guard<spinlock_mutex> lk(sm);
                shared_data.push_back(t * items_per_thread + i);
            }
        });
    }
    for (auto& t : threads) t.join();

    ASSERT_EQ(shared_data.size(),
              static_cast<size_t>(num_threads * items_per_thread));
}

// =====================================================
// lock_free_stack 测试
// =====================================================

TEST(lf_stack_push_and_pop)
{
    lock_free_stack<int> s;
    s.push(1);
    s.push(2);
    s.push(3);

    auto v = s.pop();
    ASSERT_TRUE(v != nullptr);
    ASSERT_EQ(*v, 3);

    v = s.pop();
    ASSERT_EQ(*v, 2);

    v = s.pop();
    ASSERT_EQ(*v, 1);
}

TEST(lf_stack_pop_empty)
{
    lock_free_stack<int> s;
    auto v = s.pop();
    ASSERT_TRUE(v == nullptr);
}

TEST(lf_stack_concurrent_push)
{
    lock_free_stack<int> s;
    const int num_threads = 4;
    const int items_per_thread = 1000;

    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t)
    {
        threads.emplace_back([&s, t, items_per_thread]() {
            for (int i = 0; i < items_per_thread; ++i)
                s.push(t * items_per_thread + i);
        });
    }
    for (auto& t : threads) t.join();

    // 弹出所有元素并计数
    int count = 0;
    while (s.pop()) count++;
    ASSERT_EQ(count, num_threads * items_per_thread);
}

TEST(lf_stack_concurrent_push_pop)
{
    lock_free_stack<int> s;
    const int num_threads = 4;
    const int items_per_thread = 500;
    std::atomic<int> pushed{0};
    std::atomic<int> popped{0};

    // 同时运行生产者和消费者
    std::vector<std::thread> threads;

    // 生产者
    for (int t = 0; t < num_threads; ++t)
    {
        threads.emplace_back([&s, &pushed, items_per_thread]() {
            for (int i = 0; i < items_per_thread; ++i)
            {
                s.push(i);
                pushed++;
            }
        });
    }

    // 消费者
    std::atomic<bool> done{false};
    for (int t = 0; t < num_threads; ++t)
    {
        threads.emplace_back([&s, &popped, &done]() {
            while (!done.load(std::memory_order_relaxed))
            {
                if (s.pop()) popped++;
                else std::this_thread::yield();
            }
            // 清理剩余
            while (s.pop()) popped++;
        });
    }

    // 等待生产者完成
    for (int i = 0; i < num_threads; ++i)
        threads[i].join();

    done = true;

    // 等待消费者完成
    for (int i = num_threads; i < num_threads * 2; ++i)
        threads[i].join();

    ASSERT_EQ(pushed.load(), popped.load());
}

TEST(lf_stack_stress_test)
{
    lock_free_stack<int> s;
    const int num_threads = 8;
    const int ops_per_thread = 2000;
    std::atomic<int> total_pushed{0};
    std::atomic<int> total_popped{0};

    // 每个线程交替 push 和 pop
    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t)
    {
        threads.emplace_back([&s, &total_pushed, &total_popped, ops_per_thread]() {
            for (int i = 0; i < ops_per_thread; ++i)
            {
                if (i % 2 == 0)
                {
                    s.push(i);
                    total_pushed++;
                }
                else
                {
                    if (s.pop())
                        total_popped++;
                }
            }
        });
    }
    for (auto& t : threads) t.join();

    // 弹出剩余元素
    while (s.pop()) total_popped++;

    ASSERT_EQ(total_pushed.load(), total_popped.load());
}

// =====================================================
int main()
{
    return RUN_ALL_TESTS();
}
