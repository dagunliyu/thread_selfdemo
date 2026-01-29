# Thread Programming Examples with spdlog

这是一个C++多线程编程示例集合，已集成spdlog日志库。

## 项目特性

- ✅ C++20标准
- ✅ 集成spdlog静态库用于日志记录
- ✅ 多个多线程编程示例
- ✅ 线程池实现
- ✅ 各种同步机制示例

## spdlog集成

spdlog已作为静态库集成到本项目中，位于 `third_party/spdlog/` 目录。

### spdlog特性

- 高性能、线程安全的日志库
- 支持彩色控制台输出
- 支持多种日志级别（trace, debug, info, warn, error, critical）
- 支持文件日志和滚动日志
- Header-only或编译为静态/动态库

### 已添加spdlog日志的示例

1. **spdlog_multithreading_demo.cpp** - spdlog专用演示程序
   - 演示多线程环境下的日志记录
   - 展示不同日志级别
   - 线程ID自动记录

2. **first_thread.cpp** - 第一个线程示例
   - 添加了基本的线程启动和结束日志

3. **thread_mutex.cpp** - 互斥锁示例
   - 记录锁获取和释放操作
   - 跟踪线程竞争情况

4. **condition_variable.cpp** - 条件变量示例
   - 记录读写线程同步操作
   - 跟踪消息的生产和消费

5. **thread_pool_2.0/thread_pool_2.0.cpp** - 线程池示例
   - 记录线程池的初始化、启动和停止
   - 跟踪任务的执行状态

## 构建项目

```bash
# 创建构建目录
mkdir build
cd build

# 配置CMake
cmake ..

# 编译所有示例
make -j4

# 或编译特定示例
make first_thread
make spdlog_multithreading_demo
make thread_pool_2.0
```

## 运行示例

```bash
# 进入构建目录
cd build/bin

# 运行spdlog演示
./spdlog_multithreading_demo

# 运行其他示例
./first_thread
./thread_mutex
./condition_variable
./thread_pool_2.0
```

## 日志格式

默认日志格式：
```
[年-月-日 时:分:秒.毫秒] [级别] [线程ID] 消息
```

示例输出：
```
[2026-01-29 02:51:32.046] [info] [thread 4440] === spdlog 多线程日志示例 ===
[2026-01-29 02:51:32.046] [info] [thread 4441] Worker thread 1 started with 10 iterations
[2026-01-29 02:51:32.046] [debug] [thread 4441] Thread 1 - Debug message 0
```

## 在你的代码中使用spdlog

### 基本用法

```cpp
#include <spdlog/spdlog.h>

// 设置日志级别
spdlog::set_level(spdlog::level::debug);

// 设置日志格式
spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");

// 记录不同级别的日志
spdlog::trace("This is a trace message");
spdlog::debug("This is a debug message");
spdlog::info("This is an info message");
spdlog::warn("This is a warning message");
spdlog::error("This is an error message");
spdlog::critical("This is a critical message");

// 使用格式化
spdlog::info("Hello {} {}", "World", 2026);
```

### CMake集成

所有示例程序自动链接spdlog库，无需额外配置。新增的程序会自动获得spdlog支持。

## 项目结构

```
.
├── CMakeLists.txt              # 主CMake配置
├── README.md                   # 本文件
├── third_party/                # 第三方库
│   ├── CMakeLists.txt
│   └── spdlog/                 # spdlog静态库源码
├── common/                     # 公共库
├── *.cpp                       # 各种线程示例
├── thread_pool_*/              # 线程池实现
└── thread_msg_server*/         # 消息服务器实现
```

## 许可证

请查阅各个组件的许可证：
- spdlog: MIT License
- 本项目示例代码：请参考原作者声明
