cmake_minimum_required(VERSION 3.14)
project(concurrent_cpp
    LANGUAGES CXX
    DESCRIPTION "C++ Concurrency in Action - Code Listings & Tests"
)

# ---- 全局编译设置 ----
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 线程库（pthread on Linux / Win32 threads on Windows）
find_package(Threads REQUIRED)

# ---- 公共包含目录 ----
# 所有头文件（.h）和源文件（.cpp）都在 src/ 中
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# ===========================================================
# 1. ATM 模拟应用（Appendix C 消息传递状态机）
#    入口：listing_c.10.cpp
#    依赖头文件：listing_c.1.h ~ listing_c.9.h
# ===========================================================
add_executable(atm_app
    ${SRC_DIR}/listing_c.10.cpp
)
target_include_directories(atm_app PRIVATE ${SRC_DIR})
target_link_libraries(atm_app PRIVATE Threads::Threads)

# ===========================================================
# 2. 第6章测试 — 基于锁的并发数据结构
#    threadsafe_stack, threadsafe_queue,
#    threadsafe_lookup_table, threadsafe_list
# ===========================================================
add_executable(test_ch6
    ${SRC_DIR}/test_ch6.cpp
)
target_include_directories(test_ch6 PRIVATE ${SRC_DIR})
target_link_libraries(test_ch6 PRIVATE Threads::Threads)

# ===========================================================
# 3. 第7章测试 — 无锁并发数据结构
#    spinlock_mutex, lock_free_stack
# ===========================================================
add_executable(test_ch7
    ${SRC_DIR}/test_ch7.cpp
)
target_include_directories(test_ch7 PRIVATE ${SRC_DIR})
target_link_libraries(test_ch7 PRIVATE Threads::Threads)

# ===========================================================
# 4. 第8章测试 — 并发算法与同步原语
#    parallel_accumulate, parallel_for_each, barrier
# ===========================================================
add_executable(test_ch8
    ${SRC_DIR}/test_ch8.cpp
)
target_include_directories(test_ch8 PRIVATE ${SRC_DIR})
target_link_libraries(test_ch8 PRIVATE Threads::Threads)

# ===========================================================
# 5. 第9章测试 — 高级线程管理
#    function_wrapper, thread_pool, work_stealing_queue
# ===========================================================
add_executable(test_ch9
    ${SRC_DIR}/test_ch9.cpp
)
target_include_directories(test_ch9 PRIVATE ${SRC_DIR})
target_link_libraries(test_ch9 PRIVATE Threads::Threads)

# ---- CTest 集成 ----
enable_testing()
add_test(NAME Chapter6_LockBasedStructures  COMMAND test_ch6)
add_test(NAME Chapter7_LockFreeStructures   COMMAND test_ch7)
add_test(NAME Chapter8_ParallelAlgorithms   COMMAND test_ch8)
add_test(NAME Chapter9_ThreadManagement     COMMAND test_ch9)

# 设置测试超时（并发测试可能需要更多时间）
set_tests_properties(
    Chapter6_LockBasedStructures
    Chapter7_LockFreeStructures
    Chapter8_ParallelAlgorithms
    Chapter9_ThreadManagement
    PROPERTIES TIMEOUT 60
)

# ---- 编译器警告 ----
if(MSVC)
    # MSVC 编译器选项
    add_compile_options(/W4 /EHsc)
else()
    # GCC / Clang
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

message(STATUS "")
message(STATUS "=== concurrent_cpp Build Configuration ===")
message(STATUS "Compiler:    ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type:  ${CMAKE_BUILD_TYPE}")
message(STATUS "Targets:     atm_app, test_ch6, test_ch7, test_ch8, test_ch9")
message(STATUS "==========================================")
message(STATUS "")
