cmake_minimum_required(VERSION 3.6)

project(tinyBlog VERSION 1.0.0)

set(CMAKE_BUILD_TYPE Release)
set(EXECUTABLE_OUTPUT_PATH ./bin)
set(LIBRARY_OUTPUT_DIRECTORY ./lib)

set(TINYBLOG_SRC ./tinyblog.c )
set(OUTPUT_BIN tinyblog)
include_directories(./libhv/build/include/hv)

link_directories(./libhv/build/lib)

add_executable(${OUTPUT_BIN} ${TINYBLOG_SRC})

target_link_libraries(${OUTPUT_BIN} hv)