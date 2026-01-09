# common 公共库 CMakeLists.txt

# XThread 基础线程库
add_library(xthread STATIC
    xthread.cpp
    xthread.h
)
target_include_directories(xthread PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(xthread PUBLIC Threads::Threads)

# XMsgServer 消息服务器库 (依赖 xthread)
add_library(xmsg_server STATIC
    xmsg_server.cpp
    xmsg_server.h
)
target_include_directories(xmsg_server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(xmsg_server PUBLIC xthread)

# XThreadPool 线程池库
add_library(xthread_pool STATIC
    xthread_pool.cpp
    xthread_pool.h
)
target_include_directories(xthread_pool PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(xthread_pool PUBLIC Threads::Threads)
