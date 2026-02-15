// test_ch9.cpp — 第9章：高级线程管理测试
// 测试 function_wrapper（类型擦除包装）、thread_pool（线程池）、
// work_stealing_queue（工作窃取队列）的功能和并发正确性。

#include "test_framework.h"
#include "listing_9.2.h"   // function_wrapper + thread_pool + join_threads
#include "listing_9.7.h"   // work_stealing_queue

#include <thread>
#include <vector>
#include <atomic>
#include <numeric>
#include <cmath>

// =====================================================
// function_wrapper 测试
// =====================================================

TEST(fw_basic_callable)
{
    bool called = false;
    function_wrapper fw([&called]() { called = true; });
    fw();
    ASSERT_TRUE(called);
}

TEST(fw_with_return_value)
{
    // function_wrapper 本身不返回值，但可以配合 packaged_task 使用
    std::packaged_task<int()> task([]() { return 42; });
    auto future = task.get_future();

    function_wrapper fw(std::move(task));
    fw();

    ASSERT_EQ(future.get(), 42);
}

TEST(fw_move_semantics)
{
    int counter = 0;
    function_wrapper fw1([&counter]() { counter++; });
    function_wrapper fw2(std::move(fw1));
    fw2();
    ASSERT_EQ(counter, 1);
}

TEST(fw_packaged_task_string)
{
    std::packaged_task<std::string()> task([]() {
        return std::string("hello concurrent world");
    });
    auto future = task.get_future();

    function_wrapper fw(std::move(task));
    fw();

    ASSERT_EQ(future.get(), "hello concurrent world");
}

// =====================================================
// thread_pool 测试
// =====================================================

TEST(pool_submit_single_task)
{
    thread_pool pool;
    auto future = pool.submit([]() { return 42; });
    ASSERT_EQ(future.get(), 42);
}

TEST(pool_submit_multiple_tasks)
{
    thread_pool pool;
    const int n = 100;
    std::vector<std::future<int>> futures;

    for (int i = 0; i < n; ++i)
    {
        futures.push_back(pool.submit([i]() { return i * i; }));
    }

    for (int i = 0; i < n; ++i)
    {
        ASSERT_EQ(futures[i].get(), i * i);
    }
}

TEST(pool_submit_void_task)
{
    thread_pool pool;
    std::atomic<int> counter{0};
    const int n = 50;
    std::vector<std::future<void>> futures;

    for (int i = 0; i < n; ++i)
    {
        futures.push_back(pool.submit([&counter]() {
            counter++;
        }));
    }

    for (auto& f : futures) f.get();

    ASSERT_EQ(counter.load(), n);
}

TEST(pool_parallel_accumulate)
{
    thread_pool pool;
    const int n = 10000;
    std::vector<int> data(n);
    std::iota(data.begin(), data.end(), 1);

    // 分块并行累加
    const int num_chunks = 10;
    const int chunk_size = n / num_chunks;
    std::vector<std::future<int>> futures;

    for (int c = 0; c < num_chunks; ++c)
    {
        int start = c * chunk_size;
        int end = (c == num_chunks - 1) ? n : start + chunk_size;
        futures.push_back(pool.submit([&data, start, end]() {
            int sum = 0;
            for (int i = start; i < end; ++i)
                sum += data[i];
            return sum;
        }));
    }

    int total = 0;
    for (auto& f : futures) total += f.get();

    int expected = n * (n + 1) / 2;
    ASSERT_EQ(total, expected);
}

TEST(pool_stress_test)
{
    thread_pool pool;
    std::atomic<int> completed{0};
    const int n = 1000;
    std::vector<std::future<void>> futures;

    for (int i = 0; i < n; ++i)
    {
        futures.push_back(pool.submit([&completed]() {
            // 模拟一些计算
            volatile int sum = 0;
            for (int j = 0; j < 100; ++j) sum += j;
            completed++;
        }));
    }

    for (auto& f : futures) f.get();
    ASSERT_EQ(completed.load(), n);
}

// =====================================================
// work_stealing_queue 测试
// =====================================================

TEST(wsq_push_and_pop)
{
    work_stealing_queue q;
    bool called = false;
    q.push([&called]() { called = true; });

    function_wrapper task;
    ASSERT_TRUE(q.try_pop(task));
    task();
    ASSERT_TRUE(called);
}

TEST(wsq_empty)
{
    work_stealing_queue q;
    ASSERT_TRUE(q.empty());

    q.push([]() {});
    ASSERT_FALSE(q.empty());

    function_wrapper task;
    q.try_pop(task);
    ASSERT_TRUE(q.empty());
}

TEST(wsq_pop_empty_returns_false)
{
    work_stealing_queue q;
    function_wrapper task;
    ASSERT_FALSE(q.try_pop(task));
    ASSERT_FALSE(q.try_steal(task));
}

TEST(wsq_steal_from_back)
{
    work_stealing_queue q;
    std::vector<int> order;

    q.push([&order]() { order.push_back(1); });
    q.push([&order]() { order.push_back(2); });
    q.push([&order]() { order.push_back(3); });

    // steal 从后端取（FIFO 对窃取者）
    function_wrapper stolen;
    ASSERT_TRUE(q.try_steal(stolen));
    stolen();

    // pop 从前端取（LIFO 对拥有者）
    function_wrapper popped;
    ASSERT_TRUE(q.try_pop(popped));
    popped();

    ASSERT_EQ(order[0], 1);  // stolen: 最先 push 的在后端
    ASSERT_EQ(order[1], 3);  // popped: 最后 push 的在前端
}

TEST(wsq_concurrent_pop_and_steal)
{
    work_stealing_queue q;
    const int n = 2000;
    std::atomic<int> sum{0};

    // 先填充队列
    for (int i = 1; i <= n; ++i)
    {
        q.push([&sum, i]() { sum += i; });
    }

    // 一个线程 pop，另一个线程 steal
    std::thread t1([&q, &sum]() {
        function_wrapper task;
        while (q.try_pop(task))
            task();
    });

    std::thread t2([&q, &sum]() {
        function_wrapper task;
        while (q.try_steal(task))
            task();
    });

    t1.join();
    t2.join();

    int expected = n * (n + 1) / 2;
    ASSERT_EQ(sum.load(), expected);
}

TEST(wsq_multi_thread_steal)
{
    work_stealing_queue q;
    const int n = 5000;
    std::atomic<int> count{0};

    for (int i = 0; i < n; ++i)
    {
        q.push([&count]() { count++; });
    }

    const int num_stealers = 4;
    std::vector<std::thread> threads;
    for (int t = 0; t < num_stealers; ++t)
    {
        threads.emplace_back([&q, &count]() {
            function_wrapper task;
            while (q.try_steal(task))
                task();
        });
    }

    // 主线程也 pop
    function_wrapper task;
    while (q.try_pop(task))
        task();

    for (auto& t : threads) t.join();

    ASSERT_EQ(count.load(), n);
}

// =====================================================
int main()
{
    return RUN_ALL_TESTS();
}
