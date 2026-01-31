# CMake 构建选项说明

本项目使用 CMake 构建系统，并提供了灵活的配置选项来控制第三方库的构建行为。

## Abseil 构建模式控制

项目集成了 Abseil-cpp 库，可以通过 CMake 选项灵活控制构建哪些 Abseil 模块。

### 选项：ABSL_BUILD_MODE

控制构建哪些 Abseil 模块。可选值：

- **LOG_ONLY** (默认) - 仅构建 log 相关模块，最小化构建
- **CUSTOM** - 构建用户指定的模块
- **ALL** - 构建所有 Abseil 模块（完整构建）

### 选项：ABSL_CUSTOM_COMPONENTS

在 `CUSTOM` 模式下，指定要构建的 Abseil 组件列表（用分号分隔）。

## 使用示例

### 1. 仅构建 log 模块（默认）

```bash
mkdir build && cd build
cmake ..
# 或显式指定
cmake -DABSL_BUILD_MODE=LOG_ONLY ..
```

这是默认模式，最小化构建，仅编译项目实际需要的 log 相关组件：
- `absl::log`
- `absl::log_initialize`
- `absl::log_internal_check_op`
- `absl::flags`
- `absl::flags_parse`

**优点**：
- ✅ 构建速度最快
- ✅ 编译产物最小
- ✅ 适合只使用 Abseil log 功能的场景

### 2. 构建所有 Abseil 模块

```bash
mkdir build && cd build
cmake -DABSL_BUILD_MODE=ALL ..
```

构建完整的 Abseil 库，包含所有可用模块。

**优点**：
- ✅ 提供完整的 Abseil 功能
- ✅ 可以使用任意 Abseil 组件
- ✅ 适合需要多个 Abseil 功能的场景

**缺点**：
- ⚠️ 构建时间较长
- ⚠️ 编译产物较大

### 3. 自定义构建指定模块

```bash
mkdir build && cd build
cmake -DABSL_BUILD_MODE=CUSTOM -DABSL_CUSTOM_COMPONENTS="log;strings;time" ..
```

只构建指定的 Abseil 组件。组件名可以带或不带 `absl::` 前缀。

**常用组件示例**：
- `log` - 日志功能
- `strings` - 字符串处理
- `time` - 时间相关功能
- `container` - 容器类
- `algorithm` - 算法
- `hash` - 哈希功能
- `synchronization` - 同步原语
- `flags` - 命令行标志
- `status` - 状态码

**优点**：
- ✅ 灵活控制构建范围
- ✅ 按需构建，平衡速度和功能
- ✅ 适合明确知道需要哪些组件的场景

**示例**：

构建 log 和 strings 模块：
```bash
cmake -DABSL_BUILD_MODE=CUSTOM -DABSL_CUSTOM_COMPONENTS="log;strings" ..
```

构建 log、strings、time 和 container 模块：
```bash
cmake -DABSL_BUILD_MODE=CUSTOM -DABSL_CUSTOM_COMPONENTS="log;strings;time;container" ..
```

## 查看当前配置

构建时，CMake 会显示当前的 Abseil 构建模式：

```
-- Abseil Build Mode: LOG_ONLY (仅构建log相关模块)
...
========================================
  Thread Programming Examples
========================================
  C++ Standard: 20
  Build Type:   Release
  Output Dir:   /path/to/build/bin
  Abseil Mode:  LOG_ONLY
========================================
```

对于 CUSTOM 模式，还会显示具体的组件列表：

```
-- Abseil Build Mode: CUSTOM
--   Custom components: log;strings;time
...
  Abseil Mode:  CUSTOM
  Abseil Comps: log;strings;time
```

## 切换构建模式

要切换构建模式，需要重新配置：

```bash
# 切换到 ALL 模式
cd build
cmake -DABSL_BUILD_MODE=ALL ..
make clean
make

# 切换到 CUSTOM 模式
cmake -DABSL_BUILD_MODE=CUSTOM -DABSL_CUSTOM_COMPONENTS="log;strings" ..
make clean
make
```

建议使用 `make clean` 或删除 `build` 目录后重新构建，以确保所有变更生效。

## 技术细节

### 依赖关系自动处理

Abseil 的 CMake 配置会自动处理模块之间的依赖关系。例如：
- 指定 `log` 组件时，会自动构建 `log` 依赖的基础组件（如 `base`、`strings` 等）
- 不需要手动指定所有传递依赖

### 链接组件

项目中的所有可执行目标会自动链接所选的 Abseil 组件。链接逻辑在 `CMakeLists.txt` 中的 `get_abseil_components()` 函数中定义。

### 扩展自定义组件

如果需要在项目中使用额外的 Abseil 组件：

1. 在 CUSTOM 模式下添加组件名
2. 或者在主 `CMakeLists.txt` 的 `get_abseil_components()` 函数中的 ALL 模式部分添加组件

## 其他第三方库

项目还集成了：
- **spdlog** - 高性能日志库（总是构建）
- **Google Benchmark** - 性能基准测试库（总是构建）

这些库目前没有构建选项控制，总是完整构建。

## 推荐配置

根据不同使用场景的推荐配置：

| 场景 | 推荐模式 | 命令 |
|------|----------|------|
| 日常开发 | LOG_ONLY | `cmake ..` |
| 快速构建 | LOG_ONLY | `cmake ..` |
| 使用多个 Abseil 功能 | CUSTOM | `cmake -DABSL_BUILD_MODE=CUSTOM -DABSL_CUSTOM_COMPONENTS="log;strings;..."` |
| 需要完整 Abseil | ALL | `cmake -DABSL_BUILD_MODE=ALL ..` |
| CI/CD | LOG_ONLY | `cmake ..` |

## 故障排除

### 链接错误

如果遇到类似 `undefined reference to absl::xxx` 的链接错误：

1. 检查是否在 CUSTOM 模式下遗漏了必要的组件
2. 尝试切换到 ALL 模式确认是否是组件缺失问题
3. 检查组件名称是否正确（参考 Abseil 文档）

### 构建缓存

如果修改了构建模式但没有生效：

```bash
# 方法1：清理构建
cd build
make clean
cmake -DABSL_BUILD_MODE=xxx ..
make

# 方法2：删除构建目录
rm -rf build
mkdir build
cd build
cmake -DABSL_BUILD_MODE=xxx ..
make
```

## 参考资料

- [Abseil C++ 官方文档](https://abseil.io/docs/cpp/)
- [Abseil GitHub](https://github.com/abseil/abseil-cpp)
- [CMake 文档](https://cmake.org/documentation/)
