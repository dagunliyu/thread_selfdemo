// test_ch6.cpp — 第6章：基于锁的并发数据结构测试
// 测试 threadsafe_stack、threadsafe_queue、threadsafe_lookup_table、threadsafe_list
// 包含单线程功能测试和多线程并发压力测试。

#include "test_framework.h"
#include "listing_6.1.h"   // threadsafe_stack
#include "listing_6.2.h"   // threadsafe_queue
#include "listing_6.11.h"  // threadsafe_lookup_table
#include "listing_6.13.h"  // threadsafe_list

#include <thread>
#include <vector>
#include <set>
#include <algorithm>
#include <atomic>

// =====================================================
// threadsafe_stack 测试
// =====================================================

TEST(stack_push_and_pop)
{
    threadsafe_stack<int> s;
    s.push(1);
    s.push(2);
    s.push(3);

    auto val = s.pop();
    ASSERT_EQ(*val, 3);
    val = s.pop();
    ASSERT_EQ(*val, 2);
    val = s.pop();
    ASSERT_EQ(*val, 1);
}

TEST(stack_pop_by_reference)
{
    threadsafe_stack<int> s;
    s.push(42);
    int val = 0;
    s.pop(val);
    ASSERT_EQ(val, 42);
}

TEST(stack_empty_check)
{
    threadsafe_stack<int> s;
    ASSERT_TRUE(s.empty());
    s.push(1);
    ASSERT_FALSE(s.empty());
    s.pop();
    ASSERT_TRUE(s.empty());
}

TEST(stack_pop_empty_throws)
{
    threadsafe_stack<int> s;
    ASSERT_THROW(s.pop(), empty_stack);
}

TEST(stack_copy_constructor)
{
    threadsafe_stack<int> s;
    s.push(10);
    s.push(20);

    threadsafe_stack<int> s2(s);
    ASSERT_EQ(*s2.pop(), 20);
    ASSERT_EQ(*s2.pop(), 10);
    // 原栈不受影响
    ASSERT_EQ(*s.pop(), 20);
}

TEST(stack_concurrent_push_pop)
{
    threadsafe_stack<int> s;
    const int num_threads = 4;
    const int items_per_thread = 1000;
    std::atomic<int> pop_count{0};

    // 生产者线程
    std::vector<std::thread> producers;
    for (int t = 0; t < num_threads; ++t)
    {
        producers.emplace_back([&s, t, items_per_thread]() {
            for (int i = 0; i < items_per_thread; ++i)
                s.push(t * items_per_thread + i);
        });
    }
    for (auto& t : producers) t.join();

    // 消费者线程
    std::vector<std::thread> consumers;
    for (int t = 0; t < num_threads; ++t)
    {
        consumers.emplace_back([&s, &pop_count]() {
            while (true)
            {
                try {
                    s.pop();
                    pop_count++;
                } catch (empty_stack&) {
                    break;
                }
            }
        });
    }
    for (auto& t : consumers) t.join();

    ASSERT_EQ(pop_count.load(), num_threads * items_per_thread);
    ASSERT_TRUE(s.empty());
}

// =====================================================
// threadsafe_queue 测试
// =====================================================

TEST(queue_push_and_try_pop)
{
    threadsafe_queue<int> q;
    q.push(10);
    q.push(20);
    q.push(30);

    int val = 0;
    ASSERT_TRUE(q.try_pop(val));
    ASSERT_EQ(val, 10);

    auto ptr = q.try_pop();
    ASSERT_TRUE(ptr != nullptr);
    ASSERT_EQ(*ptr, 20);
}

TEST(queue_try_pop_empty)
{
    threadsafe_queue<int> q;
    int val = 0;
    ASSERT_FALSE(q.try_pop(val));
    auto ptr = q.try_pop();
    ASSERT_TRUE(ptr == nullptr);
}

TEST(queue_wait_and_pop)
{
    threadsafe_queue<int> q;

    std::thread producer([&q]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        q.push(42);
    });

    int val = 0;
    q.wait_and_pop(val);
    ASSERT_EQ(val, 42);

    producer.join();
}

TEST(queue_concurrent_produce_consume)
{
    threadsafe_queue<int> q;
    const int num_items = 5000;
    std::atomic<int> sum{0};

    std::thread producer([&q, num_items]() {
        for (int i = 1; i <= num_items; ++i)
            q.push(i);
    });

    std::thread consumer([&q, &sum, num_items]() {
        for (int i = 0; i < num_items; ++i)
        {
            int val;
            q.wait_and_pop(val);
            sum += val;
        }
    });

    producer.join();
    consumer.join();

    int expected = num_items * (num_items + 1) / 2;
    ASSERT_EQ(sum.load(), expected);
}

TEST(queue_multi_producer_multi_consumer)
{
    threadsafe_queue<int> q;
    const int num_producers = 4;
    const int num_consumers = 4;
    const int items_per_producer = 500;
    std::atomic<int> total_consumed{0};

    std::vector<std::thread> producers;
    for (int p = 0; p < num_producers; ++p)
    {
        producers.emplace_back([&q, items_per_producer]() {
            for (int i = 0; i < items_per_producer; ++i)
                q.push(i);
        });
    }

    std::vector<std::thread> consumers;
    std::atomic<bool> done{false};
    for (int c = 0; c < num_consumers; ++c)
    {
        consumers.emplace_back([&q, &total_consumed, &done]() {
            while (!done.load() || !q.empty())
            {
                int val;
                if (q.try_pop(val))
                    total_consumed++;
                else
                    std::this_thread::yield();
            }
        });
    }

    for (auto& t : producers) t.join();
    // 等待队列排空
    while (!q.empty())
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    done = true;
    for (auto& t : consumers) t.join();

    ASSERT_EQ(total_consumed.load(), num_producers * items_per_producer);
}

// =====================================================
// threadsafe_lookup_table 测试
// =====================================================

TEST(lookup_table_add_and_get)
{
    threadsafe_lookup_table<std::string, int> table;
    table.add_or_update_mapping("apple", 1);
    table.add_or_update_mapping("banana", 2);

    ASSERT_EQ(table.value_for("apple", 0), 1);
    ASSERT_EQ(table.value_for("banana", 0), 2);
    ASSERT_EQ(table.value_for("cherry", -1), -1);  // 默认值
}

TEST(lookup_table_update)
{
    threadsafe_lookup_table<std::string, int> table;
    table.add_or_update_mapping("key", 100);
    ASSERT_EQ(table.value_for("key", 0), 100);

    table.add_or_update_mapping("key", 200);
    ASSERT_EQ(table.value_for("key", 0), 200);
}

TEST(lookup_table_remove)
{
    threadsafe_lookup_table<std::string, int> table;
    table.add_or_update_mapping("key", 42);
    ASSERT_EQ(table.value_for("key", 0), 42);

    table.remove_mapping("key");
    ASSERT_EQ(table.value_for("key", 0), 0);
}

TEST(lookup_table_concurrent_access)
{
    threadsafe_lookup_table<int, int> table;
    const int num_threads = 4;
    const int ops_per_thread = 1000;

    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t)
    {
        threads.emplace_back([&table, t, ops_per_thread]() {
            for (int i = 0; i < ops_per_thread; ++i)
            {
                int key = t * ops_per_thread + i;
                table.add_or_update_mapping(key, key * 2);
            }
        });
    }
    for (auto& t : threads) t.join();

    // 验证所有值
    for (int t = 0; t < num_threads; ++t)
    {
        for (int i = 0; i < ops_per_thread; ++i)
        {
            int key = t * ops_per_thread + i;
            ASSERT_EQ(table.value_for(key, -1), key * 2);
        }
    }
}

// =====================================================
// threadsafe_list 测试
// =====================================================

TEST(list_push_front_and_for_each)
{
    threadsafe_list<int> lst;
    lst.push_front(3);
    lst.push_front(2);
    lst.push_front(1);

    std::vector<int> collected;
    lst.for_each([&collected](int val) {
        collected.push_back(val);
    });

    ASSERT_EQ(collected.size(), 3u);
    ASSERT_EQ(collected[0], 1);
    ASSERT_EQ(collected[1], 2);
    ASSERT_EQ(collected[2], 3);
}

TEST(list_find_first_if)
{
    threadsafe_list<int> lst;
    lst.push_front(10);
    lst.push_front(20);
    lst.push_front(30);

    auto found = lst.find_first_if([](int v) { return v == 20; });
    ASSERT_TRUE(found != nullptr);
    ASSERT_EQ(*found, 20);

    auto not_found = lst.find_first_if([](int v) { return v == 99; });
    ASSERT_TRUE(not_found == nullptr);
}

TEST(list_remove_if)
{
    threadsafe_list<int> lst;
    for (int i = 1; i <= 5; ++i) lst.push_front(i);

    // 删除偶数
    lst.remove_if([](int v) { return v % 2 == 0; });

    std::vector<int> remaining;
    lst.for_each([&remaining](int v) { remaining.push_back(v); });

    ASSERT_EQ(remaining.size(), 3u);
    // 验证只剩奇数
    for (int v : remaining)
    {
        ASSERT_TRUE(v % 2 != 0);
    }
}

TEST(list_concurrent_push)
{
    threadsafe_list<int> lst;
    const int num_threads = 4;
    const int items_per_thread = 500;

    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t)
    {
        threads.emplace_back([&lst, items_per_thread]() {
            for (int i = 0; i < items_per_thread; ++i)
                lst.push_front(i);
        });
    }
    for (auto& t : threads) t.join();

    int count = 0;
    lst.for_each([&count](int) { count++; });
    ASSERT_EQ(count, num_threads * items_per_thread);
}

// =====================================================
int main()
{
    return RUN_ALL_TESTS();
}
