// test_framework.h — 轻量级测试框架
// 提供 TEST() 宏定义测试用例，ASSERT_* 系列断言宏，
// 以及 TestRunner 自动注册和批量运行所有测试。
// 无外部依赖，不需要链接第三方库。
#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <chrono>

// ---- 测试注册与运行 ----
class TestRunner
{
    struct TestCase {
        std::string name;
        std::function<void()> func;
    };

    std::vector<TestCase>& tests()
    {
        static std::vector<TestCase> t;
        return t;
    }

    static TestRunner& instance()
    {
        static TestRunner runner;
        return runner;
    }

public:
    static void add(const std::string& name, std::function<void()> func)
    {
        instance().tests().push_back({name, func});
    }

    static int run_all()
    {
        int passed = 0, failed = 0;
        auto& all_tests = instance().tests();

        std::cout << "========================================" << std::endl;
        std::cout << "  Running " << all_tests.size() << " test(s)..." << std::endl;
        std::cout << "========================================" << std::endl;

        for (auto& t : all_tests)
        {
            auto start = std::chrono::high_resolution_clock::now();
            try
            {
                t.func();
                auto end = std::chrono::high_resolution_clock::now();
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                std::cout << "  [PASS] " << t.name
                          << " (" << ms << " ms)" << std::endl;
                passed++;
            }
            catch (std::exception& e)
            {
                auto end = std::chrono::high_resolution_clock::now();
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                std::cout << "  [FAIL] " << t.name
                          << " (" << ms << " ms)" << std::endl;
                std::cout << "         " << e.what() << std::endl;
                failed++;
            }
        }

        std::cout << "========================================" << std::endl;
        std::cout << "  Result: " << passed << " passed, "
                  << failed << " failed" << std::endl;
        std::cout << "========================================" << std::endl;
        return failed > 0 ? 1 : 0;
    }
};

struct TestRegistrar
{
    TestRegistrar(const std::string& name, std::function<void()> func)
    {
        TestRunner::add(name, func);
    }
};

// ---- 宏定义 ----
#define TEST(name)                                                  \
    void test_##name();                                             \
    static TestRegistrar registrar_##name(#name, test_##name);      \
    void test_##name()

#define ASSERT_TRUE(expr)                                           \
    do { if(!(expr)) {                                              \
        std::ostringstream oss;                                     \
        oss << "ASSERT_TRUE failed: " #expr                         \
            << "  [" << __FILE__ << ":" << __LINE__ << "]";         \
        throw std::runtime_error(oss.str());                        \
    } } while(0)

#define ASSERT_FALSE(expr)                                          \
    do { if(expr) {                                                 \
        std::ostringstream oss;                                     \
        oss << "ASSERT_FALSE failed: " #expr                        \
            << "  [" << __FILE__ << ":" << __LINE__ << "]";         \
        throw std::runtime_error(oss.str());                        \
    } } while(0)

#define ASSERT_EQ(a, b)                                             \
    do { if((a) != (b)) {                                           \
        std::ostringstream oss;                                     \
        oss << "ASSERT_EQ failed: " #a " != " #b                   \
            << "  [" << __FILE__ << ":" << __LINE__ << "]";         \
        throw std::runtime_error(oss.str());                        \
    } } while(0)

#define ASSERT_NE(a, b)                                             \
    do { if((a) == (b)) {                                           \
        std::ostringstream oss;                                     \
        oss << "ASSERT_NE failed: " #a " == " #b                   \
            << "  [" << __FILE__ << ":" << __LINE__ << "]";         \
        throw std::runtime_error(oss.str());                        \
    } } while(0)

#define ASSERT_THROW(expr, exc_type)                                \
    do { bool caught_ = false;                                      \
        try { expr; }                                               \
        catch(exc_type&) { caught_ = true; }                        \
        if(!caught_) {                                              \
            std::ostringstream oss;                                  \
            oss << "ASSERT_THROW failed: expected " #exc_type       \
                << "  [" << __FILE__ << ":" << __LINE__ << "]";     \
            throw std::runtime_error(oss.str());                    \
        }                                                           \
    } while(0)

#define ASSERT_NO_THROW(expr)                                       \
    do { try { expr; }                                              \
        catch(...) {                                                \
            std::ostringstream oss;                                  \
            oss << "ASSERT_NO_THROW failed: " #expr                 \
                << "  [" << __FILE__ << ":" << __LINE__ << "]";     \
            throw std::runtime_error(oss.str());                    \
        }                                                           \
    } while(0)

#define RUN_ALL_TESTS() TestRunner::run_all()

#endif // TEST_FRAMEWORK_H
