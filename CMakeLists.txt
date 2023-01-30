cmake_minimum_required(VERSION 3.6)

project(tinyBlog VERSION 1.0.9)

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

set(UTILS_SRC ./src/utils/utils.c
    ./src/utils/targs.c
    ./src/utils/tconfigure.c
    ./src/utils/tmkd.c
    )

set(TINYBLOG_SRC ./src/main.c  ./src/create_html.c ./cjson/cJSON.c ${UTILS_SRC} ${MOONRISE_SRC})

set(OUTPUT_BIN tinyblog)

include_directories(./libhv/build/include/hv ./moonrise/src ./moonrise/html ./cjson ./src ./src/utils)

link_directories(./libhv/build/lib)

add_executable(${OUTPUT_BIN} ${TINYBLOG_SRC})

target_link_libraries(${OUTPUT_BIN} libhv_static.a pthread)