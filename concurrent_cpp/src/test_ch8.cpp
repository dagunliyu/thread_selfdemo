// test_ch8.cpp — 第8章：并发算法与同步原语测试
// 测试 parallel_accumulate（并行累加）、parallel_for_each（并行遍历）、
// barrier（线程屏障）的正确性与并发行为。

#include "test_framework.h"
#include "listing_8.5.h"   // parallel_accumulate
#include "listing_8.8.h"   // parallel_for_each
#include "listing_8.12.h"  // barrier

#include <thread>
#include <vector>
#include <numeric>
#include <atomic>
#include <cmath>

// =====================================================
// parallel_accumulate 测试
// =====================================================

TEST(accumulate_small_range)
{
    std::vector<int> v = {1, 2, 3, 4, 5};
    int result = parallel_accumulate(v.begin(), v.end(), 0);
    ASSERT_EQ(result, 15);
}

TEST(accumulate_large_range)
{
    const int n = 10000;
    std::vector<int> v(n);
    std::iota(v.begin(), v.end(), 1);  // 1, 2, ..., n

    int result = parallel_accumulate(v.begin(), v.end(), 0);
    int expected = n * (n + 1) / 2;
    ASSERT_EQ(result, expected);
}

TEST(accumulate_single_element)
{
    std::vector<int> v = {42};
    int result = parallel_accumulate(v.begin(), v.end(), 0);
    ASSERT_EQ(result, 42);
}

TEST(accumulate_empty)
{
    std::vector<int> v;
    int result = parallel_accumulate(v.begin(), v.end(), 0);
    ASSERT_EQ(result, 0);
}

TEST(accumulate_with_init)
{
    std::vector<int> v = {1, 2, 3};
    int result = parallel_accumulate(v.begin(), v.end(), 100);
    ASSERT_EQ(result, 106);
}

TEST(accumulate_doubles)
{
    std::vector<double> v = {1.5, 2.5, 3.0, 4.0};
    double result = parallel_accumulate(v.begin(), v.end(), 0.0);
    ASSERT_TRUE(std::abs(result - 11.0) < 1e-9);
}

TEST(accumulate_correctness_vs_serial)
{
    const int n = 5000;
    std::vector<int> v(n);
    std::iota(v.begin(), v.end(), 1);

    int serial_result = std::accumulate(v.begin(), v.end(), 0);
    int parallel_result = parallel_accumulate(v.begin(), v.end(), 0);

    ASSERT_EQ(parallel_result, serial_result);
}

// =====================================================
// parallel_for_each 测试
// =====================================================

TEST(for_each_basic)
{
    std::vector<int> v = {1, 2, 3, 4, 5};
    std::atomic<int> sum{0};

    parallel_for_each(v.begin(), v.end(), [&sum](int val) {
        sum += val;
    });

    ASSERT_EQ(sum.load(), 15);
}

TEST(for_each_empty)
{
    std::vector<int> v;
    int count = 0;

    parallel_for_each(v.begin(), v.end(), [&count](int) {
        count++;
    });

    ASSERT_EQ(count, 0);
}

TEST(for_each_large_range)
{
    const int n = 10000;
    std::vector<int> v(n, 1);
    std::atomic<int> count{0};

    parallel_for_each(v.begin(), v.end(), [&count](int val) {
        count += val;
    });

    ASSERT_EQ(count.load(), n);
}

TEST(for_each_modify_elements)
{
    const int n = 1000;
    std::vector<std::atomic<int>> v(n);
    for (int i = 0; i < n; ++i) v[i] = i;

    // 将每个元素加倍（各元素独立，无数据竞争）
    parallel_for_each(v.begin(), v.end(), [](std::atomic<int>& val) {
        int old = val.load();
        val.store(old * 2);
    });

    for (int i = 0; i < n; ++i)
    {
        ASSERT_EQ(v[i].load(), i * 2);
    }
}

// =====================================================
// barrier 测试
// =====================================================

TEST(barrier_basic_sync)
{
    const int num_threads = 4;
    barrier b(num_threads);
    std::atomic<int> phase1_done{0};
    std::atomic<int> phase2_started{0};

    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t)
    {
        threads.emplace_back([&b, &phase1_done, &phase2_started]() {
            // 阶段1工作
            phase1_done++;
            b.wait();
            // 到达这里时，所有线程都完成了阶段1
            phase2_started++;
        });
    }

    for (auto& t : threads) t.join();

    ASSERT_EQ(phase1_done.load(), num_threads);
    ASSERT_EQ(phase2_started.load(), num_threads);
}

TEST(barrier_multi_phase)
{
    const int num_threads = 4;
    const int num_phases = 3;
    barrier b(num_threads);
    std::vector<std::atomic<int>> phase_counts(num_phases);
    for (int i = 0; i < num_phases; ++i) phase_counts[i] = 0;

    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t)
    {
        threads.emplace_back([&b, &phase_counts, num_phases]() {
            for (int p = 0; p < num_phases; ++p)
            {
                phase_counts[p]++;
                b.wait();
            }
        });
    }

    for (auto& t : threads) t.join();

    for (int p = 0; p < num_phases; ++p)
    {
        ASSERT_EQ(phase_counts[p].load(), num_threads);
    }
}

TEST(barrier_ensures_ordering)
{
    const int num_threads = 4;
    barrier b(num_threads);
    std::atomic<int> counter{0};
    std::vector<int> results(num_threads, 0);

    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t)
    {
        threads.emplace_back([&b, &counter, &results, t]() {
            // 阶段1：每个线程递增计数器
            counter++;
            b.wait();
            // 阶段2：所有线程都应该看到 counter == num_threads
            results[t] = counter.load();
        });
    }

    for (auto& t : threads) t.join();

    for (int t = 0; t < num_threads; ++t)
    {
        ASSERT_EQ(results[t], num_threads);
    }
}

// =====================================================
int main()
{
    return RUN_ALL_TESTS();
}
