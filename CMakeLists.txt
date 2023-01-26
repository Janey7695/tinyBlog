cmake_minimum_required(VERSION 3.6)

project(tinyBlog VERSION 1.0.0)

set(CMAKE_BUILD_TYPE Debug)
set(EXECUTABLE_OUTPUT_PATH ./bin)
set(LIBRARY_OUTPUT_DIRECTORY ./lib)

add_subdirectory(./libhv)

set(MOONRISE_SRC ./moonrise/src/markdown.c 
    ./moonrise/src/buffer.c 
    ./moonrise/src/stack.c 
    ./moonrise/src/autolink.c 
    ./moonrise/html/html.c
    ./moonrise/html/houdini_href_e.c
    ./moonrise/html/houdini_html_e.c
    ./moonrise/html/html_smartypants.c
    )

set(TINYBLOG_SRC ./tinyblog.c ./create_html.c ./utils.c ./cjson/cJSON.c ${MOONRISE_SRC})

set(OUTPUT_BIN tinyblog)

include_directories(./build/libhv/include/hv ./moonrise/src ./moonrise/html ./cjson)

link_directories(./build/lib)

add_executable(${OUTPUT_BIN} ${TINYBLOG_SRC})


target_link_libraries(${OUTPUT_BIN} libhv_static.a pthread)