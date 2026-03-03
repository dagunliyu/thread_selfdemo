cmake_minimum_required(VERSION 3.15)
project(ThreadProgrammingExamples LANGUAGES CXX)

# ============================================================================
# 全局编译设置
# ============================================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 查找线程库
find_package(Threads REQUIRED)

# ============================================================================
# 集成第三方库（spdlog, abseil, benchmark）
# ============================================================================
add_subdirectory(third_party)

# ============================================================================
# 根据Abseil构建模式确定要链接的组件
# ============================================================================
# 这个函数会根据third_party/CMakeLists.txt中的ABSL_BUILD_MODE设置
# 返回应该链接的Abseil组件列表
function(get_abseil_components RESULT_VAR)
    if(ABSL_BUILD_MODE STREQUAL "LOG_ONLY")
        # LOG_ONLY模式：仅链接log相关组件
        set(${RESULT_VAR} 
            absl::log
            absl::log_initialize
            absl::log_internal_check_op
            absl::flags
            absl::flags_parse
            PARENT_SCOPE
        )
    elseif(ABSL_BUILD_MODE STREQUAL "CUSTOM")
        # CUSTOM模式：链接用户指定的组件
        set(CUSTOM_LIBS "")
        foreach(component ${ABSL_CUSTOM_COMPONENTS})
            # 如果组件名已经有absl::前缀，直接使用；否则添加前缀
            if(component MATCHES "^absl::")
                list(APPEND CUSTOM_LIBS "${component}")
            else()
                list(APPEND CUSTOM_LIBS "absl::${component}")
            endif()
        endforeach()
        set(${RESULT_VAR} ${CUSTOM_LIBS} PARENT_SCOPE)
    elseif(ABSL_BUILD_MODE STREQUAL "ALL")
        # ALL模式：链接当前项目需要的所有组件（可以扩展）
        set(${RESULT_VAR} 
            absl::log
            absl::log_initialize
            absl::log_internal_check_op
            absl::flags
            absl::flags_parse
            # 用户可以在这里添加更多需要的组件
            PARENT_SCOPE
        )
    else()
        # 默认使用LOG_ONLY
        set(${RESULT_VAR} 
            absl::log
            absl::log_initialize
            absl::log_internal_check_op
            absl::flags
            absl::flags_parse
            PARENT_SCOPE
        )
    endif()
endfunction()

# 获取要链接的Abseil组件
get_abseil_components(ABSL_LINK_COMPONENTS)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ============================================================================
# 公共函数：为单个源文件创建可执行目标
# ============================================================================
function(add_thread_example source_file)
    # 从文件路径提取名称作为target名
    get_filename_component(target_name ${source_file} NAME_WE)
    
    add_executable(${target_name} ${source_file})
    target_link_libraries(${target_name} PRIVATE 
        Threads::Threads 
        spdlog::spdlog
        ${ABSL_LINK_COMPONENTS}
    )
    
    # 将target分组到Examples文件夹（Visual Studio/Xcode）
    set_target_properties(${target_name} PROPERTIES FOLDER "Examples")
    
    message(STATUS "[Example] Added: ${target_name}")
endfunction()

# ============================================================================
# 公共函数：为复杂子项目创建可执行目标
# ============================================================================
function(add_subproject_executable target_name source_dir)
    # 扫描目录下所有cpp文件
    file(GLOB SUB_SOURCES "${source_dir}/*.cpp")
    file(GLOB SUB_HEADERS "${source_dir}/*.h")
    
    add_executable(${target_name} ${SUB_SOURCES} ${SUB_HEADERS})
    target_include_directories(${target_name} PRIVATE ${source_dir})
    target_link_libraries(${target_name} PRIVATE 
        Threads::Threads 
        spdlog::spdlog
        ${ABSL_LINK_COMPONENTS}
    )
    
    set_target_properties(${target_name} PROPERTIES FOLDER "Subprojects")
    
    message(STATUS "[Subproject] Added: ${target_name}")
endfunction()

# ============================================================================
# 1. 构建公共库 (common/)
# ============================================================================
add_subdirectory(common)

# ============================================================================
# 2. 构建根目录下的独立示例程序（排除benchmark）
# ============================================================================
file(GLOB ROOT_EXAMPLES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

foreach(example_file ${ROOT_EXAMPLES})
    get_filename_component(example_name ${example_file} NAME_WE)
    # 跳过benchmark文件，它需要特殊处理
    if(NOT example_name STREQUAL "logging_benchmark")
        add_thread_example(${example_file})
    endif()
endforeach()

# ============================================================================
# 3. 构建子目录项目 (使用各自目录内的源文件)
# ============================================================================

# thread_msg_server - 使用本地文件（早期版本，无条件变量）
add_subproject_executable(
    "msg_server"
    "${CMAKE_CURRENT_SOURCE_DIR}/thread_msg_server"
)

# thread_msg_server_condition - 使用本地文件（带条件变量）
add_subproject_executable(
    "msg_server_condition"
    "${CMAKE_CURRENT_SOURCE_DIR}/thread_msg_server_condition"
)

# thread_pool_1.0 - 线程池初始版本
add_subproject_executable(
    "thread_pool_1.0"
    "${CMAKE_CURRENT_SOURCE_DIR}/thread_pool_1.0_init_start"
)

# thread_pool_2.0 - 线程池 2.0 版本
add_subproject_executable(
    "thread_pool_2.0"
    "${CMAKE_CURRENT_SOURCE_DIR}/thread_pool_2.0"
)

# thread_pool_2.0_atomic - 带原子操作的线程池
add_subproject_executable(
    "thread_pool_2.0_atomic"
    "${CMAKE_CURRENT_SOURCE_DIR}/thread_pool_2.0_atomic"
)

# thread_pool_3.0 - 线程池 3.0 版本
add_subproject_executable(
    "thread_pool_3.0"
    "${CMAKE_CURRENT_SOURCE_DIR}/thread_pool_3.0"
)

# thread_pool_video_ffmpeg - 视频转码线程池（不包含ffmpeg.exe和mp4文件）
set(VIDEO_POOL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thread_pool_video_ffmpeg_codeConv")
file(GLOB VIDEO_POOL_SOURCES "${VIDEO_POOL_DIR}/*.cpp")
file(GLOB VIDEO_POOL_HEADERS "${VIDEO_POOL_DIR}/*.h")
add_executable(video_ffmpeg_pool ${VIDEO_POOL_SOURCES} ${VIDEO_POOL_HEADERS})
target_include_directories(video_ffmpeg_pool PRIVATE ${VIDEO_POOL_DIR})
target_link_libraries(video_ffmpeg_pool PRIVATE 
    Threads::Threads 
    spdlog::spdlog
    ${ABSL_LINK_COMPONENTS}
)
set_target_properties(video_ffmpeg_pool PROPERTIES FOLDER "Subprojects")
message(STATUS "[Subproject] Added: video_ffmpeg_pool")

# ============================================================================
# 4. 使用公共库的示例（可选，演示如何链接公共库）
# ============================================================================
# 示例：创建一个使用公共库的新target
# add_executable(my_app main.cpp)
# target_link_libraries(my_app PRIVATE xthread_pool)

# ============================================================================
# 5. 构建性能基准测试
# ============================================================================
add_executable(logging_benchmark logging_benchmark.cpp)
target_link_libraries(logging_benchmark PRIVATE
    Threads::Threads
    spdlog::spdlog
    ${ABSL_LINK_COMPONENTS}
    benchmark::benchmark
)
set_target_properties(logging_benchmark PROPERTIES FOLDER "Benchmarks")
message(STATUS "[Benchmark] Added: logging_benchmark")

# ============================================================================
# 打印构建信息
# ============================================================================
message(STATUS "")
message(STATUS "========================================")
message(STATUS "  Thread Programming Examples")
message(STATUS "========================================")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type:   ${CMAKE_BUILD_TYPE}")
message(STATUS "  Output Dir:   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "  Abseil Mode:  ${ABSL_BUILD_MODE}")
if(ABSL_BUILD_MODE STREQUAL "CUSTOM")
    message(STATUS "  Abseil Comps: ${ABSL_CUSTOM_COMPONENTS}")
endif()
message(STATUS "========================================")
message(STATUS "")
