# Thread Programming Examples with spdlog and Abseil

这是一个C++多线程编程示例集合，已集成spdlog和Abseil日志库，并包含性能基准测试。

## 项目特性

- ✅ C++20标准
- ✅ 集成spdlog静态库用于日志记录
- ✅ 集成Abseil-cpp库（包含Abseil log）
- ✅ 性能基准测试（spdlog vs Abseil log）
- ✅ 多个多线程编程示例
- ✅ 线程池实现
- ✅ 各种同步机制示例

## 第三方库

本项目集成了以下第三方库（通过Git子模块管理）：

| 库 | 版本 | 用途 |
|----|------|------|
| [spdlog](https://github.com/gabime/spdlog) | v1.17.0 | 高性能日志库 |
| [Abseil C++](https://github.com/abseil/abseil-cpp) | 20260107.0 | Google C++库（包含日志功能） |
| [Google Benchmark](https://github.com/google/benchmark) | v1.9.5 | 性能基准测试框架 |

详细版本信息和更新历史请参阅 [LIBRARY_VERSIONS.md](LIBRARY_VERSIONS.md)。

### spdlog集成

spdlog已作为静态库集成到本项目中，位于 `third_party/spdlog/` 目录。

**spdlog特性**:
- 高性能、线程安全的日志库
- 支持彩色控制台输出
- 支持多种日志级别（trace, debug, info, warn, error, critical）
- 支持文件日志和滚动日志
- Header-only或编译为静态/动态库

### Abseil log集成

Abseil-cpp库（包含Abseil log）已集成到项目中，位于 `third_party/abseil-cpp/` 目录。

**Abseil log特性**:
- Google开发的C++通用库
- 结构化日志支持
- 线程安全的日志记录
- 与Google工具链良好集成
- 支持日志级别：INFO, WARNING, ERROR, FATAL
- 支持VLOG详细日志级别

## 性能基准测试

我们提供了详细的性能基准测试，比较spdlog和Abseil log在不同场景下的性能：

- 📊 **单线程性能**: spdlog同步 (70.6 ns) vs Abseil (541 ns)
- 📊 **多线程性能**: 4线程和8线程并发测试
- 📊 **消息大小**: 8字节到8KB不同大小的消息
- 📊 **同步vs异步**: spdlog支持异步模式的性能对比

详细的性能测试结果和分析，请参阅 [BENCHMARK.md](BENCHMARK.md)。

### 快速性能对比

| 场景 | spdlog (同步) | spdlog (异步) | Abseil |
|------|---------------|---------------|---------|
| 单线程 | **70.6 ns** ⚡ | 327 ns | 541 ns |
| 4线程 | **10.28M ops/s** ⚡ | 1.12M ops/s | 993k ops/s |
| 8线程 | **10.22M ops/s** ⚡ | 921k ops/s | 1.00M ops/s |

**结论**: spdlog同步模式在性能测试中表现最佳，适合性能敏感场景。Abseil log提供更好的结构化日志和跨平台一致性。

## 已添加日志的示例

1. **spdlog_multithreading_demo.cpp** - spdlog专用演示程序
   - 演示多线程环境下的日志记录
   - 展示不同日志级别
   - 线程ID自动记录

2. **abseil_log_demo.cpp** - Abseil log演示程序
   - 演示Abseil log在多线程中的使用
   - 展示结构化日志特性
   - Google风格的日志记录

3. **logging_benchmark.cpp** - 日志性能基准测试
   - 对比spdlog和Abseil log性能
   - 测试同步和异步模式
   - 测试不同消息大小和线程数

4. **first_thread.cpp** - 第一个线程示例
   - 添加了基本的线程启动和结束日志

5. **thread_mutex.cpp** - 互斥锁示例
   - 记录锁获取和释放操作
   - 跟踪线程竞争情况

6. **condition_variable.cpp** - 条件变量示例
   - 记录读写线程同步操作
   - 跟踪消息的生产和消费

7. **thread_pool_2.0/thread_pool_2.0.cpp** - 线程池示例
   - 记录线程池的初始化、启动和停止
   - 跟踪任务的执行状态

## 构建项目

⚠️ **重要**: 本项目使用 Git 子模块管理第三方库。首次克隆时请使用：

```bash
# 推荐：克隆时同时初始化子模块
git clone --recursive https://github.com/dagunliyu/thread_selfdemo.git

# 或者先克隆，再初始化子模块
git clone https://github.com/dagunliyu/thread_selfdemo.git
cd thread_selfdemo
git submodule update --init --recursive
```

详细的子模块使用说明请参阅 [SUBMODULES.md](SUBMODULES.md)。

### CMake 构建选项

本项目支持灵活控制 Abseil 库的构建模式，可以根据需求选择：

```bash
# 默认：仅构建 log 模块（推荐，构建最快）
cmake ..

# 构建所有 Abseil 模块
cmake -DABSL_BUILD_MODE=ALL ..

# 自定义构建特定模块
cmake -DABSL_BUILD_MODE=CUSTOM -DABSL_CUSTOM_COMPONENTS="log;strings;time" ..
```

详细的构建选项说明请参阅 [CMAKE_OPTIONS.md](CMAKE_OPTIONS.md)。

### 编译步骤

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

# 运行Abseil log演示
./abseil_log_demo

# 运行性能基准测试
./logging_benchmark

# 运行其他示例
./first_thread
./thread_mutex
./condition_variable
./thread_pool_2.0
```

## 性能基准测试

```bash
# 运行基准测试
cd build/bin
./logging_benchmark

# 运行并输出为JSON格式
./logging_benchmark --benchmark_out=results.json --benchmark_out_format=json

# 查看详细结果
cat ../../BENCHMARK.md
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

## 在你的代码中使用日志库

### spdlog 基本用法

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

### Abseil log 基本用法

```cpp
#include "absl/log/log.h"
#include "absl/log/initialize.h"

int main(int argc, char* argv[]) {
    // 初始化Abseil logging
    absl::InitializeLog();
    
    // 记录不同级别的日志
    LOG(INFO) << "This is an info message";
    LOG(WARNING) << "This is a warning message";
    LOG(ERROR) << "This is an error message";
    // LOG(FATAL) << "This would terminate the program";
    
    // 详细日志 (verbose logging)
    VLOG(1) << "Verbose level 1 message";
    VLOG(2) << "Verbose level 2 message";
    
    // 使用流式格式化
    int value = 42;
    LOG(INFO) << "The answer is " << value;
    
    return 0;
}
```

### CMake集成

所有示例程序自动链接spdlog和Abseil库，无需额外配置。新增的程序会自动获得两个库的支持。

## 项目结构

```
.
├── CMakeLists.txt              # 主CMake配置
├── README.md                   # 本文件
├── BENCHMARK.md                # 性能基准测试结果
├── third_party/                # 第三方库
│   ├── CMakeLists.txt
│   ├── spdlog/                 # spdlog静态库源码
│   ├── abseil-cpp/             # Abseil-cpp库源码
│   └── benchmark/              # Google Benchmark库
├── common/                     # 公共库
├── *.cpp                       # 各种线程示例
├── spdlog_multithreading_demo.cpp   # spdlog演示
├── abseil_log_demo.cpp         # Abseil log演示
├── logging_benchmark.cpp       # 性能基准测试
├── thread_pool_*/              # 线程池实现
└── thread_msg_server*/         # 消息服务器实现
```

## 许可证

请查阅各个组件的许可证：
- spdlog: MIT License
- Abseil-cpp: Apache License 2.0
- Google Benchmark: Apache License 2.0
- 本项目示例代码：请参考原作者声明
