# 如何生成和理解 BENCHMARK.md

## 概述

本文档详细说明 `BENCHMARK.md` 文件是如何生成的，以及其中的 CPU 时间和吞吐量是如何统计的。

## 快速回答

**BENCHMARK.md 的生成过程**：
1. 运行编译好的 `logging_benchmark` 可执行文件
2. Google Benchmark 框架自动测量性能指标
3. 手动整理输出结果到 BENCHMARK.md 文档中

**CPU 时间和吞吐量的计算**：
- **CPU 时间**：由 Google Benchmark 自动测量，表示代码执行消耗的 CPU 时间
- **吞吐量**：根据 `state.SetItemsProcessed()` 计算，公式为：`吞吐量 = 处理的项目数 / CPU时间`

---

## 详细说明

### 1. 基准测试代码：logging_benchmark.cpp

#### 1.1 代码结构

基准测试代码使用 **Google Benchmark** 库，位于 `logging_benchmark.cpp`：

```cpp
#include <benchmark/benchmark.h>
#include <spdlog/spdlog.h>
#include "absl/log/log.h"

// 定义基准测试函数
static void BM_Spdlog_Sync_SingleThread(benchmark::State& state) {
    auto logger = spdlog::create<spdlog::sinks::null_sink_mt>("sync_bench");
    spdlog::set_default_logger(logger);
    
    // 基准测试主循环
    for (auto _ : state) {
        spdlog::info("Benchmark message: iteration {}", state.iterations());
    }
    
    spdlog::drop("sync_bench");
    state.SetItemsProcessed(state.iterations());  // 设置处理的项目数
}
BENCHMARK(BM_Spdlog_Sync_SingleThread);  // 注册基准测试
```

#### 1.2 基准测试类型

代码包含以下类型的基准测试：

1. **单线程测试**
   - `BM_Spdlog_Sync_SingleThread` - spdlog 同步模式
   - `BM_Spdlog_Async_SingleThread` - spdlog 异步模式
   - `BM_Abseil_Sync_SingleThread` - Abseil log 同步模式

2. **多线程测试**
   - `BM_Spdlog_Sync_MultiThread` - spdlog 同步多线程
   - `BM_Spdlog_Async_MultiThread` - spdlog 异步多线程
   - `BM_Abseil_MultiThread` - Abseil log 多线程
   - 使用 `->Threads(4)->Threads(8)` 指定线程数

3. **不同消息大小测试**
   - `BM_Spdlog_Sync_VaryingSize` - spdlog 同步不同大小
   - `BM_Spdlog_Async_VaryingSize` - spdlog 异步不同大小
   - `BM_Abseil_VaryingSize` - Abseil 不同大小
   - 使用 `->Range(8, 8192)` 指定消息大小范围

### 2. 性能指标的计算原理

#### 2.1 Google Benchmark 工作原理

Google Benchmark 框架会：

1. **自动执行多次迭代**：运行测试代码多次以获得稳定的测量结果
2. **测量时间**：精确测量每次迭代消耗的时间
3. **计算统计数据**：计算平均时间、标准差等
4. **自动调整迭代次数**：根据代码执行速度调整迭代次数，确保测量精度

#### 2.2 CPU 时间的测量

**CPU 时间 (CPU Time)** 是指代码实际占用 CPU 的时间：

```cpp
for (auto _ : state) {
    // 这个循环体内的代码执行时间会被测量
    spdlog::info("Message");  // <- 测量这行代码的执行时间
}
```

**测量方式**：
- 使用高精度计时器（如 `std::chrono` 或系统调用）
- 测量循环体内代码的执行时间
- 多次迭代后取平均值
- 排除外部因素（如 I/O 等待）的影响

**输出示例**：
```
BM_Spdlog_Sync_SingleThread    70.6 ns    70.6 ns    10134654
                               ^^^^^^^
                               CPU 时间（纳秒）
```

#### 2.3 吞吐量的计算

**吞吐量 (Throughput)** 表示单位时间内处理的操作数量：

```cpp
state.SetItemsProcessed(state.iterations());
```

**计算公式**：
```
吞吐量 = 处理的项目数 / CPU 时间

例如：
- CPU 时间: 70.6 ns
- 迭代次数: 10,134,654 次
- 吞吐量 = 10,134,654 / (70.6 * 10,134,654 / 10^9) 秒
        = 1 / 70.6 ns
        = 14.17 M items/s (百万次/秒)
```

**输出示例**：
```
BM_Spdlog_Sync_SingleThread    70.6 ns    70.6 ns    10134654    items_per_second=14.1719M/s
                                                                  ^^^^^^^^^^^^^^^^^^^^^^^
                                                                  吞吐量（每秒项目数）
```

#### 2.4 带宽的计算

对于消息大小可变的测试，还会计算**带宽 (Bandwidth)**：

```cpp
state.SetBytesProcessed(state.iterations() * state.range(0));
```

**计算公式**：
```
带宽 = 处理的字节数 / CPU 时间

例如：
- 消息大小: 64 bytes
- CPU 时间: 57.3 ns
- 迭代次数: 12,326,512 次
- 带宽 = (64 * 12,326,512) / (57.3 * 12,326,512 / 10^9) 秒
       = 64 / 57.3 ns
       = 1.04 GiB/s (千兆字节/秒)
```

**输出示例**：
```
BM_Spdlog_Sync_VaryingSize/64    57.3 ns    57.3 ns    12326512    bytes_per_second=1.03998Gi/s
                                                                    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^
                                                                    带宽（字节/秒）
```

### 3. 如何生成 BENCHMARK.md

#### 3.1 编译基准测试程序

```bash
cd /home/runner/work/thread_selfdemo/thread_selfdemo

# 初始化子模块（如果需要）
git submodule update --init --recursive

# 创建构建目录
mkdir -p build && cd build

# 配置 CMake
cmake ..

# 编译基准测试
make logging_benchmark -j4
```

#### 3.2 运行基准测试

```bash
# 基本运行
./bin/logging_benchmark

# 运行并输出到文件
./bin/logging_benchmark > benchmark_results.txt

# 输出为 JSON 格式
./bin/logging_benchmark --benchmark_out=results.json --benchmark_out_format=json

# 输出为 CSV 格式
./bin/logging_benchmark --benchmark_out=results.csv --benchmark_out_format=csv

# 仅运行特定测试
./bin/logging_benchmark --benchmark_filter=".*Spdlog.*"

# 设置最小运行时间（提高精度）
./bin/logging_benchmark --benchmark_min_time=1.0
```

#### 3.3 基准测试输出解读

**完整输出示例**：
```
Run on (4 X 2445.43 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x2)
  L1 Instruction 32 KiB (x2)
  L2 Unified 512 KiB (x2)
  L3 Unified 32768 KiB (x1)
------------------------------------------------------------------------------------------------
Benchmark                                      Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------
BM_Spdlog_Sync_SingleThread                 70.6 ns         70.6 ns     10134654 items_per_second=14.1719M/s
```

**字段说明**：

| 字段 | 说明 | 单位 |
|------|------|------|
| **Benchmark** | 测试用例名称 | - |
| **Time** | 墙上时钟时间（包括等待时间） | ns/μs/ms/s |
| **CPU** | CPU 执行时间（纯计算时间） | ns/μs/ms/s |
| **Iterations** | 执行的迭代次数 | 次数 |
| **items_per_second** | 吞吐量 | ops/s |
| **bytes_per_second** | 带宽（仅消息大小测试） | B/s, KiB/s, MiB/s, GiB/s |

**多线程输出示例**：
```
BM_Spdlog_Sync_MultiThread/threads:4        24.7 ns         97.3 ns      7350304 items_per_second=10.2789M/s
                                            ^^^^^^^        ^^^^^^^
                                            单线程时间      总CPU时间
```

- **Time (24.7 ns)**: 单个线程的平均时间
- **CPU (97.3 ns)**: 所有线程的总 CPU 时间
- **items_per_second**: 所有线程的总吞吐量

#### 3.4 从输出生成 MARKDOWN

BENCHMARK.md 是手动根据基准测试输出整理而成：

1. **复制测试环境信息**：
   ```
   - **CPU**: 4 cores @ 2445.43 MHz
   - **缓存**: L1/L2/L3 信息
   ```

2. **整理测试结果到表格**：
   ```markdown
   | 测试场景 | 时间 (ns) | 吞吐量 (ops/s) | 相对性能 |
   |---------|-----------|----------------|----------|
   | **spdlog (同步)** | 70.6 ns | 14.17M/s | **最快** |
   ```

3. **添加分析和建议**：
   - 性能对比分析
   - 使用场景建议
   - 最佳实践

4. **附加完整输出**：
   - 将原始输出作为附录

### 4. 自动化脚本

为了简化重新生成基准测试的过程，可以创建一个脚本：

#### 4.1 运行基准测试脚本

创建 `scripts/run_benchmark.sh`：

```bash
#!/bin/bash
# 运行基准测试并生成结果

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"
OUTPUT_DIR="$PROJECT_ROOT/benchmark_results"

# 创建输出目录
mkdir -p "$OUTPUT_DIR"

# 检查可执行文件
if [ ! -f "$BUILD_DIR/bin/logging_benchmark" ]; then
    echo "错误: logging_benchmark 未找到"
    echo "请先运行: cd build && cmake .. && make logging_benchmark"
    exit 1
fi

# 运行基准测试
echo "正在运行基准测试..."
cd "$BUILD_DIR"

# 生成文本输出
./bin/logging_benchmark > "$OUTPUT_DIR/benchmark_results.txt"

# 生成 JSON 输出
./bin/logging_benchmark --benchmark_out="$OUTPUT_DIR/results.json" --benchmark_out_format=json

# 生成 CSV 输出
./bin/logging_benchmark --benchmark_out="$OUTPUT_DIR/results.csv" --benchmark_out_format=csv

echo "基准测试完成！"
echo "结果保存在: $OUTPUT_DIR"
echo "- benchmark_results.txt  (文本格式)"
echo "- results.json           (JSON格式)"
echo "- results.csv            (CSV格式)"
```

#### 4.2 使用方法

```bash
# 给脚本添加执行权限
chmod +x scripts/run_benchmark.sh

# 运行脚本
./scripts/run_benchmark.sh
```

### 5. 深入理解：Google Benchmark 原理

#### 5.1 迭代次数的自动调整

Google Benchmark 会自动调整迭代次数：

```
第一轮: 运行 1 次，测量时间
如果时间太短 (<100ms): 增加迭代次数
第二轮: 运行更多次，重新测量
重复直到: 达到足够的统计精度
```

**示例**：
- 快速操作（如 spdlog sync: 70ns）需要运行 10,134,654 次
- 慢速操作（如大消息: 2950ns）只需运行 237,112 次

#### 5.2 时间测量的精度

```cpp
// Google Benchmark 内部实现（简化版）
auto start = std::chrono::high_resolution_clock::now();
for (int i = 0; i < iterations; ++i) {
    // 你的测试代码
    benchmark_function();
}
auto end = std::chrono::high_resolution_clock::now();
auto duration = end - start;
double time_per_iteration = duration / iterations;
```

#### 5.3 统计数据的计算

Google Benchmark 会：
1. 运行多轮测试
2. 计算每轮的平均时间
3. 检查标准差，确保结果稳定
4. 如果结果不稳定，增加迭代次数重新测试

### 6. 常见问题 (FAQ)

#### Q1: 为什么 Time 和 CPU 值不同？

**A**: 
- **Time** (墙上时钟时间): 包括所有等待时间（I/O、系统调度等）
- **CPU** (CPU 时间): 只计算 CPU 执行时间

对于纯计算任务，两者通常相同。对于多线程测试，CPU 时间是所有线程的总和。

#### Q2: 如何提高基准测试的精度？

**A**:
```bash
# 增加最小运行时间
./bin/logging_benchmark --benchmark_min_time=2.0

# 指定重复次数
./bin/logging_benchmark --benchmark_repetitions=10

# 显示统计信息
./bin/logging_benchmark --benchmark_display_aggregates_only=true
```

#### Q3: 为什么我的结果和 BENCHMARK.md 不同？

**A**: 性能结果受多种因素影响：
- CPU 型号和频率
- 系统负载
- 编译器优化级别 (`-O2` vs `-O3`)
- 内存频率
- 操作系统和内核版本

#### Q4: 如何只测试特定的场景？

**A**:
```bash
# 只测试 spdlog
./bin/logging_benchmark --benchmark_filter=".*Spdlog.*"

# 只测试单线程
./bin/logging_benchmark --benchmark_filter=".*SingleThread.*"

# 只测试异步模式
./bin/logging_benchmark --benchmark_filter=".*Async.*"
```

#### Q5: items_per_second 是如何计算的？

**A**:
```cpp
// 在测试代码中设置
state.SetItemsProcessed(state.iterations());

// Google Benchmark 计算
items_per_second = items_processed / cpu_time_in_seconds
```

### 7. 参考资料

- [Google Benchmark 官方文档](https://github.com/google/benchmark)
- [Google Benchmark 用户指南](https://github.com/google/benchmark/blob/main/docs/user_guide.md)
- [spdlog 文档](https://github.com/gabime/spdlog)
- [Abseil 文档](https://abseil.io/docs/cpp/)

### 8. 总结

**BENCHMARK.md 生成流程总结**：

```
1. 编写基准测试代码 (logging_benchmark.cpp)
   ↓
2. 使用 Google Benchmark 框架
   ↓
3. 编译基准测试程序 (make logging_benchmark)
   ↓
4. 运行程序获取结果 (./bin/logging_benchmark)
   ↓
5. 手动整理输出到 BENCHMARK.md
   ↓
6. 添加分析、建议和说明
```

**关键指标计算**：
- **CPU 时间**: Google Benchmark 自动测量 (高精度计时器)
- **吞吐量**: `items_per_second = iterations / cpu_time`
- **带宽**: `bytes_per_second = total_bytes / cpu_time`

**重新生成 BENCHMARK.md 的步骤**：
```bash
# 1. 构建
cd build && cmake .. && make logging_benchmark

# 2. 运行
./bin/logging_benchmark > ../benchmark_results.txt

# 3. 整理
# 手动将 benchmark_results.txt 的内容整理到 BENCHMARK.md
```

---

**最后更新**: 2026-01-31
