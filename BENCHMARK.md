# Logging Performance Benchmark: spdlog vs Abseil log

## 测试环境

- **CPU**: 4 cores @ 2445.43 MHz
- **缓存**: 
  - L1 Data: 32 KiB (x2)
  - L1 Instruction: 32 KiB (x2)
  - L2 Unified: 512 KiB (x2)
  - L3 Unified: 32768 KiB (x1)
- **操作系统**: Linux
- **编译器**: GCC/Clang with C++20
- **测试工具**: Google Benchmark

## 性能测试结果

### 单线程性能对比

| 测试场景 | 时间 (ns) | 吞吐量 (ops/s) | 相对性能 |
|---------|-----------|----------------|----------|
| **spdlog (同步)** | 70.6 ns | 14.17M/s | **最快** (基准) |
| **spdlog (异步)** | 327 ns | 3.07M/s | 4.6x 慢于同步 |
| **Abseil (同步)** | 541 ns | 1.85M/s | 7.7x 慢于 spdlog 同步 |

**关键发现**:
- ✅ **spdlog 同步模式**是单线程场景下性能最优的选择
- spdlog 异步模式在单线程下由于队列开销反而更慢
- Abseil log 在单线程下性能较低，因为它针对线程安全和结构化日志进行了优化

### 多线程性能对比

#### 4线程并发

| 测试场景 | CPU时间 (ns) | 吞吐量 (ops/s) | 相对性能 |
|---------|-------------|----------------|----------|
| **spdlog (同步)** | 97.3 ns | 10.28M/s | 最快 |
| **spdlog (异步)** | 895 ns | 1.12M/s | 8.7x 慢 |
| **Abseil (同步)** | 1007 ns | 993k/s | 9.8x 慢 |

#### 8线程并发

| 测试场景 | CPU时间 (ns) | 吞吐量 (ops/s) | 相对性能 |
|---------|-------------|----------------|----------|
| **spdlog (同步)** | 97.8 ns | 10.22M/s | 最快 |
| **spdlog (异步)** | 1085 ns | 921k/s | 11.1x 慢 |
| **Abseil (同步)** | 998 ns | 1.00M/s | 10.0x 慢 |

**关键发现**:
- ✅ **spdlog 同步模式**在多线程环境下表现最佳
- 当线程数增加时，spdlog 同步性能保持稳定
- spdlog 异步模式在高并发下性能下降明显
- Abseil log 在多线程下性能相对稳定

### 不同消息大小的性能对比

#### spdlog 同步模式

| 消息大小 | 时间 (ns) | 带宽 | 吞吐量 |
|---------|-----------|------|--------|
| 8 bytes | 58.7 ns | 130 MiB/s | 17.04M/s |
| 64 bytes | 57.3 ns | 1.04 GiB/s | 17.45M/s |
| 512 bytes | 81.8 ns | 5.83 GiB/s | 12.22M/s |
| 4 KiB | 1554 ns | 2.46 GiB/s | 644k/s |
| 8 KiB | 2950 ns | 2.59 GiB/s | 339k/s |

#### spdlog 异步模式

| 消息大小 | 时间 (ns) | 带宽 | 吞吐量 |
|---------|-----------|------|--------|
| 8 bytes | 315 ns | 24.5 MiB/s | 3.21M/s |
| 64 bytes | 315 ns | 194 MiB/s | 3.18M/s |
| 512 bytes | 570 ns | 864 MiB/s | 1.77M/s |
| 4 KiB | 2428 ns | 1.58 GiB/s | 415k/s |
| 8 KiB | 4646 ns | 1.64 GiB/s | 215k/s |

#### Abseil log

| 消息大小 | 时间 (ns) | 带宽 | 吞吐量 |
|---------|-----------|------|--------|
| 8 bytes | 463 ns | 16.5 MiB/s | 2.16M/s |
| 64 bytes | 458 ns | 133 MiB/s | 2.18M/s |
| 512 bytes | 492 ns | 993 MiB/s | 2.03M/s |
| 4 KiB | 593 ns | 6.44 GiB/s | 1.69M/s |
| 8 KiB | 716 ns | 10.65 GiB/s | 1.40M/s |

**关键发现**:
- ✅ 对于大消息(4KB+)，**Abseil log 带宽最高**
- spdlog 同步模式在小消息(<512B)下性能最优
- Abseil log 在不同消息大小下性能更稳定
- 消息大小对性能影响显著，大消息会降低吞吐量

## 性能分析与建议

### 同步 vs 异步模式

**同步模式 (spdlog sync, Abseil)**:
- ✅ **优势**: 延迟低，CPU开销小
- ❌ **劣势**: 日志操作会阻塞调用线程
- 📋 **适用场景**: 
  - 日志量不大的应用
  - 对实时性要求高的场景
  - 单线程或低并发应用

**异步模式 (spdlog async)**:
- ✅ **优势**: 不阻塞调用线程，适合高吞吐
- ❌ **劣势**: 额外的内存和CPU开销，延迟略高
- 📋 **适用场景**:
  - 高频日志记录
  - 需要最小化主线程延迟
  - 日志I/O可能成为瓶颈的场景

### spdlog vs Abseil 选择建议

#### 选择 spdlog 的场景

1. **性能优先**: 需要最低延迟和最高吞吐量
2. **简单日志**: 格式化文本日志即可
3. **资源受限**: CPU或内存有限
4. **小消息**: 日志消息通常较短 (<512B)

#### 选择 Abseil log 的场景

1. **结构化日志**: 需要键值对、结构化数据
2. **Google生态**: 已使用Abseil库
3. **大消息**: 需要记录大量数据 (>4KB)
4. **一致性**: 需要与Google风格工具链集成
5. **跨平台**: Abseil提供更好的跨平台一致性

## 测试代码

完整的基准测试代码位于 `logging_benchmark.cpp`。

### 运行基准测试

```bash
# 构建项目
cd build
cmake ..
make logging_benchmark

# 运行基准测试
./bin/logging_benchmark

# 运行并导出为JSON
./bin/logging_benchmark --benchmark_out=results.json --benchmark_out_format=json
```

### 测试覆盖

基准测试包括:
- ✅ 单线程同步/异步logging
- ✅ 多线程（4/8线程）logging
- ✅ 不同消息大小（8B - 8KB）
- ✅ 吞吐量和延迟测量

## 结论

### 性能排序 (单线程)

1. 🥇 **spdlog 同步**: 70.6 ns - 最快
2. 🥈 **spdlog 异步**: 327 ns - 4.6x
3. 🥉 **Abseil 同步**: 541 ns - 7.7x

### 性能排序 (多线程 4核)

1. 🥇 **spdlog 同步**: 10.28 M ops/s - 最快
2. 🥈 **spdlog 异步**: 1.12 M ops/s
3. 🥉 **Abseil 同步**: 993 k ops/s

### 最佳实践建议

**对于性能敏感的应用**:
- 使用 **spdlog 同步模式** 获得最佳性能
- 保持日志消息简短 (<512 bytes)
- 在发布版本中设置适当的日志级别过滤

**对于高可靠性应用**:
- 使用 **spdlog 异步模式** 避免阻塞主逻辑
- 考虑使用 **Abseil log** 获得结构化日志和更好的诊断信息
- 为大消息场景优化 Abseil log

**通用建议**:
- 在开发环境使用详细日志级别
- 在生产环境使用 WARNING 或 ERROR 级别
- 定期轮转日志文件避免磁盘占满
- 监控日志系统的资源使用

## 附录：完整基准测试输出

```
------------------------------------------------------------------------------------------------
Benchmark                                      Time             CPU   Iterations UserCounters...
------------------------------------------------------------------------------------------------
BM_Spdlog_Sync_SingleThread                 70.6 ns         70.6 ns     10134654 items_per_second=14.1719M/s
BM_Spdlog_Async_SingleThread                 327 ns          326 ns      2384839 items_per_second=3.06887M/s
BM_Abseil_Sync_SingleThread                  541 ns          541 ns      1292763 items_per_second=1.84711M/s
BM_Spdlog_Sync_MultiThread/threads:4        24.7 ns         97.3 ns      7350304 items_per_second=10.2789M/s
BM_Spdlog_Sync_MultiThread/threads:8        19.6 ns         97.8 ns      7170088 items_per_second=10.2233M/s
BM_Spdlog_Async_MultiThread/threads:4        399 ns          895 ns       795476 items_per_second=1.11794M/s
BM_Spdlog_Async_MultiThread/threads:8        669 ns         1085 ns       540280 items_per_second=921.328k/s
BM_Abseil_MultiThread/threads:4              252 ns         1007 ns       696548 items_per_second=993.074k/s
BM_Abseil_MultiThread/threads:8              214 ns          998 ns       706304 items_per_second=1.00168M/s
BM_Spdlog_Sync_VaryingSize/8                58.7 ns         58.7 ns     11882088 bytes_per_second=130.013Mi/s items_per_second=17.0411M/s
BM_Spdlog_Sync_VaryingSize/64               57.3 ns         57.3 ns     12326512 bytes_per_second=1.03998Gi/s items_per_second=17.4479M/s
BM_Spdlog_Sync_VaryingSize/512              81.8 ns         81.8 ns      8558271 bytes_per_second=5.82853Gi/s items_per_second=12.2233M/s
BM_Spdlog_Sync_VaryingSize/4096             1554 ns         1554 ns       450260 bytes_per_second=2.45545Gi/s items_per_second=643.682k/s
BM_Spdlog_Sync_VaryingSize/8192             2950 ns         2949 ns       237112 bytes_per_second=2.58687Gi/s items_per_second=339.066k/s
BM_Spdlog_Async_VaryingSize/8                315 ns          311 ns      2425101 bytes_per_second=24.4964Mi/s items_per_second=3.21079M/s
BM_Spdlog_Async_VaryingSize/64               315 ns          314 ns      2384069 bytes_per_second=194.198Mi/s items_per_second=3.18174M/s
BM_Spdlog_Async_VaryingSize/512              570 ns          565 ns      1250331 bytes_per_second=863.893Mi/s items_per_second=1.76925M/s
BM_Spdlog_Async_VaryingSize/4096            2428 ns         2408 ns       315558 bytes_per_second=1.58413Gi/s items_per_second=415.271k/s
BM_Spdlog_Async_VaryingSize/8192            4646 ns         4645 ns       171669 bytes_per_second=1.64247Gi/s items_per_second=215.282k/s
BM_Abseil_VaryingSize/8                      463 ns          463 ns      1525618 bytes_per_second=16.4947Mi/s items_per_second=2.162M/s
BM_Abseil_VaryingSize/64                     458 ns          458 ns      1527381 bytes_per_second=133.178Mi/s items_per_second=2.18199M/s
BM_Abseil_VaryingSize/512                    492 ns          492 ns      1466536 bytes_per_second=993.184Mi/s items_per_second=2.03404M/s
BM_Abseil_VaryingSize/4096                   593 ns          593 ns      1178301 bytes_per_second=6.43597Gi/s items_per_second=1.68715M/s
BM_Abseil_VaryingSize/8192                   716 ns          716 ns       976371 bytes_per_second=10.651Gi/s items_per_second=1.39605M/s
```
