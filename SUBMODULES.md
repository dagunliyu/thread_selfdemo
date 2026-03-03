# Git Submodules - Third-Party Libraries

This repository uses git submodules to manage third-party dependencies. The following libraries are included as submodules:

## Included Libraries

1. **spdlog** (v1.13.0)
   - URL: https://github.com/gabime/spdlog.git
   - Path: `third_party/spdlog/`
   - Fast C++ logging library

2. **abseil-cpp** (20240722.0)
   - URL: https://github.com/abseil/abseil-cpp.git
   - Path: `third_party/abseil-cpp/`
   - Abseil C++ library from Google

3. **Google Benchmark** (v1.8.3)
   - URL: https://github.com/google/benchmark.git
   - Path: `third_party/benchmark/`
   - Microbenchmark support library

## How to Clone This Repository

### Option 1: Clone with submodules (Recommended)

```bash
# Clone the repository and initialize all submodules in one command
git clone --recursive https://github.com/dagunliyu/thread_selfdemo.git
```

### Option 2: Clone then initialize submodules

```bash
# Clone the repository
git clone https://github.com/dagunliyu/thread_selfdemo.git
cd thread_selfdemo

# Initialize and update all submodules
git submodule update --init --recursive
```

## Updating Submodules

If you've already cloned the repository and need to update the submodules:

```bash
# Update all submodules to the latest commit referenced by the main repository
git submodule update --recursive

# Or pull the latest changes from the main repo and update submodules
git pull
git submodule update --init --recursive
```

## Checking Submodule Status

```bash
# Check the status of all submodules
git submodule status

# Should show something like:
#  7c02e2049e81448e7d91823a3c17758ee582e80e third_party/spdlog (v1.13.0)
#  4447c7562e3bc702ade25105912dce503f0c4010 third_party/abseil-cpp (20240722.0)
#  344117638c8ff7e239044fd0fa7085839fc03021 third_party/benchmark (v1.8.3)
```

## Troubleshooting

### Empty third_party directories

If you cloned the repository without `--recursive` and the third_party directories are empty:

```bash
git submodule update --init --recursive
```

### Submodule commit mismatch

If you see changes in submodules that you didn't make:

```bash
# Reset submodules to the commit referenced by the main repository
git submodule update --recursive
```

### Cannot download submodules

If you're behind a firewall or proxy, you might need to configure git to use HTTPS instead of SSH:

```bash
# Use HTTPS for GitHub
git config --global url."https://github.com/".insteadOf git@github.com:
git config --global url."https://".insteadOf git://
```

## Building the Project

Once submodules are initialized, you can build the project:

```bash
mkdir build
cd build
cmake ..
make -j4
```

The build system will automatically compile the third-party libraries as static libraries.

## More Information

For more details about git submodules, see:
- [Git Submodules Documentation](https://git-scm.com/book/en/v2/Git-Tools-Submodules)
- [GitHub Submodules Guide](https://github.blog/2016-02-01-working-with-submodules/)
