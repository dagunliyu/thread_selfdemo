# Third-party libraries

# Build spdlog as a static library
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "" FORCE)
add_subdirectory(spdlog)

# ============================================================================
# Abseil-cpp 构建配置
# ============================================================================
# 选项说明：
#   ABSL_BUILD_MODE - 控制构建哪些Abseil模块
#     - LOG_ONLY:  仅构建log相关模块（默认，最小化构建）
#     - CUSTOM:    构建用户指定的模块（通过ABSL_CUSTOM_COMPONENTS设置）
#     - ALL:       构建所有Abseil模块（完整构建）
#
# 使用示例：
#   cmake -DABSL_BUILD_MODE=LOG_ONLY ..           # 仅log模块（默认）
#   cmake -DABSL_BUILD_MODE=ALL ..                # 所有模块
#   cmake -DABSL_BUILD_MODE=CUSTOM -DABSL_CUSTOM_COMPONENTS="log;strings;time" ..  # 自定义模块
# ============================================================================

set(ABSL_BUILD_MODE "LOG_ONLY" CACHE STRING "Abseil build mode: LOG_ONLY, CUSTOM, or ALL")
set_property(CACHE ABSL_BUILD_MODE PROPERTY STRINGS "LOG_ONLY" "CUSTOM" "ALL")

# 用于CUSTOM模式的组件列表（用分号分隔）
set(ABSL_CUSTOM_COMPONENTS "log" CACHE STRING "Custom Abseil components to build (semicolon-separated)")

# Abseil基础配置
set(ABSL_PROPAGATE_CXX_STD ON CACHE BOOL "" FORCE)
set(ABSL_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(ABSL_USE_EXTERNAL_GOOGLETEST OFF CACHE BOOL "" FORCE)

# 根据BUILD_MODE设置要启用的模块
if(ABSL_BUILD_MODE STREQUAL "LOG_ONLY")
    message(STATUS "Abseil Build Mode: LOG_ONLY (仅构建log相关模块)")
    # Abseil会根据依赖关系自动构建所需的基础模块
    set(ABSL_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
    
elseif(ABSL_BUILD_MODE STREQUAL "CUSTOM")
    message(STATUS "Abseil Build Mode: CUSTOM")
    message(STATUS "  Custom components: ${ABSL_CUSTOM_COMPONENTS}")
    set(ABSL_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
    
elseif(ABSL_BUILD_MODE STREQUAL "ALL")
    message(STATUS "Abseil Build Mode: ALL (构建所有Abseil模块)")
    set(ABSL_ENABLE_INSTALL ON CACHE BOOL "" FORCE)
    
else()
    message(WARNING "Unknown ABSL_BUILD_MODE: ${ABSL_BUILD_MODE}, defaulting to LOG_ONLY")
    set(ABSL_BUILD_MODE "LOG_ONLY" CACHE STRING "Abseil build mode" FORCE)
endif()

# 添加Abseil子目录
add_subdirectory(abseil-cpp)

# Build Google Benchmark
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(benchmark)
