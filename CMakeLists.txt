cmake_minimum_required(VERSION 3.6)

project(tinyBlog VERSION 1.2.0)

set(CMAKE_BUILD_TYPE Debug)
set(EXECUTABLE_OUTPUT_PATH ./bin)


set(MOONRISE_SRC ./moonrise/src/markdown.c 
    ./moonrise/src/buffer.c 
    ./moonrise/src/stack.c 
    ./moonrise/src/autolink.c 
    ./moonrise/html/html.c
    ./moonrise/html/houdini_href_e.c
    ./moonrise/html/houdini_html_e.c
    ./moonrise/html/html_smartypants.c
    )

set(UTILS_SRC ./src/utils/utils.cpp
    ./src/utils/targs.c
    ./src/utils/tconfigure.cpp
    ./src/utils/tmkd.cpp
    )

set(LOGGER_SRC ./src/log/log.cpp
    ./src/log/pr.cpp
    )
set(NET_SRC ./src/net/epoll.cpp

)

set(TINYBLOG_SRC ./src/main.cpp  ./src/create_html.cpp ./src/thttpd.cpp ./cjson/cJSON.c ${UTILS_SRC} ${MOONRISE_SRC} ${LOGGER_SRC} ${NET_SRC})

set(OUTPUT_BIN tinyblog)

include_directories(./moonrise/src ./moonrise/html ./cjson ./src ./src/utils ./src/threadpool ./src/log ./src/net)

add_executable(${OUTPUT_BIN} ${TINYBLOG_SRC})

target_link_libraries(${OUTPUT_BIN} pthread)
