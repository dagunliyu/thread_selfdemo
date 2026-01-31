# Third-Party Library Versions

This document tracks the versions of third-party libraries used in this project.

## Current Versions (as of 2026-01-31)

| Library | Version | Release Date | Repository |
|---------|---------|--------------|------------|
| **spdlog** | v1.17.0 | 2024-12-29 | https://github.com/gabime/spdlog |
| **Abseil C++** | 20260107.0 | 2026-01-07 | https://github.com/abseil/abseil-cpp |
| **Google Benchmark** | v1.9.5 | 2024-12-12 | https://github.com/google/benchmark |

## Previous Versions

### 2024-07-22 (Initial Integration)
| Library | Version |
|---------|---------|
| spdlog | v1.13.0 |
| Abseil C++ | 20240722.0 |
| Google Benchmark | v1.8.3 |

## Version History

### 2026-01-31: Major Library Upgrades

**spdlog: v1.13.0 → v1.17.0**
- Upgrade spanning 4 minor versions
- Improvements in performance and bug fixes
- No breaking API changes affecting this project

**Abseil C++: 20240722.0 → 20260107.0**
- ~6 months of updates (LTS release)
- Added new features:
  - `absl::chunked_queue`
  - `absl::down_cast`
  - `absl::linked_hash_set` and `absl::linked_hash_map`
  - Better support for C++17/20 features
- Deprecated some legacy APIs
- Performance improvements in hash containers and string operations
- No breaking changes affecting this project

**Google Benchmark: v1.8.3 → v1.9.5**
- 1 minor version + 5 patch versions upgrade
- Bug fixes and stability improvements
- Enhanced compatibility with newer compilers
- No breaking API changes affecting this project

## Compatibility

All libraries have been tested with:
- **C++ Standard**: C++20
- **CMake**: 3.15+
- **Compilers**: GCC, Clang
- **Build Modes**: Debug, Release

## Testing Status

✅ All examples compile successfully
✅ All demos run correctly:
- spdlog_multithreading_demo
- abseil_log_demo
- logging_benchmark

✅ No breaking API changes detected
✅ All existing functionality preserved

## Update Process

To update libraries to newer versions:

1. Check for new releases:
```bash
cd third_party/spdlog && git fetch --tags && git tag -l | sort -V | tail -5
cd third_party/abseil-cpp && git fetch --tags && git tag -l | sort -V | tail -5
cd third_party/benchmark && git fetch --tags && git tag -l | sort -V | tail -5
```

2. Checkout new version:
```bash
cd third_party/spdlog && git checkout <version-tag>
cd third_party/abseil-cpp && git checkout <version-tag>
cd third_party/benchmark && git checkout <version-tag>
```

3. Stage and test:
```bash
git add third_party/
rm -rf build && mkdir build && cd build
cmake ..
make -j4
```

4. Run demos to verify:
```bash
./bin/spdlog_multithreading_demo
./bin/abseil_log_demo
./bin/logging_benchmark --benchmark_min_time=0.1s
```

## Release Notes Links

- [spdlog releases](https://github.com/gabime/spdlog/releases)
- [Abseil releases](https://github.com/abseil/abseil-cpp/releases)
- [Google Benchmark releases](https://github.com/google/benchmark/releases)
