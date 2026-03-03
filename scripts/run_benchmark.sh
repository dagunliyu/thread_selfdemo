#!/bin/bash
# 运行基准测试并生成结果
# Run benchmark tests and generate results

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"
OUTPUT_DIR="$PROJECT_ROOT/benchmark_results"

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "========================================"
echo "  基准测试运行脚本"
echo "  Benchmark Runner Script"
echo "========================================"
echo ""

# 创建输出目录
mkdir -p "$OUTPUT_DIR"

# 检查可执行文件
if [ ! -f "$BUILD_DIR/bin/logging_benchmark" ]; then
    echo -e "${RED}错误: logging_benchmark 未找到${NC}"
    echo -e "${YELLOW}请先运行以下命令:${NC}"
    echo "  cd $PROJECT_ROOT"
    echo "  mkdir -p build && cd build"
    echo "  cmake .."
    echo "  make logging_benchmark"
    exit 1
fi

# 运行基准测试
echo -e "${GREEN}正在运行基准测试...${NC}"
echo "构建目录: $BUILD_DIR"
echo "输出目录: $OUTPUT_DIR"
echo ""

cd "$BUILD_DIR"

# 生成文本输出
echo "1. 生成文本格式结果..."
./bin/logging_benchmark | tee "$OUTPUT_DIR/benchmark_results.txt"

echo ""
echo "2. 生成 JSON 格式结果..."
./bin/logging_benchmark --benchmark_out="$OUTPUT_DIR/results.json" --benchmark_out_format=json > /dev/null 2>&1

echo "3. 生成 CSV 格式结果..."
./bin/logging_benchmark --benchmark_out="$OUTPUT_DIR/results.csv" --benchmark_out_format=csv > /dev/null 2>&1

echo ""
echo -e "${GREEN}基准测试完成！${NC}"
echo ""
echo "结果保存在: $OUTPUT_DIR"
echo "  - benchmark_results.txt  (文本格式 - 完整输出)"
echo "  - results.json           (JSON格式 - 机器可读)"
echo "  - results.csv            (CSV格式 - Excel可读)"
echo ""
echo -e "${YELLOW}提示:${NC} 参考 HOW_TO_BENCHMARK.md 了解如何解读结果"
echo ""
