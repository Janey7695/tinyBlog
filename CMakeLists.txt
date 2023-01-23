cmake_minimum_required(VERSION 3.6)

project(tinyBlog VERSION 1.0.0)

set(CMAKE_BUILD_TYPE Debug)
set(EXECUTABLE_OUTPUT_PATH ./bin)
set(LIBRARY_OUTPUT_DIRECTORY ./lib)

set(TINYBLOG_SRC ./tinyblog.c ./create_html.c ./utils.c ./cjson/cJSON.c)

set(OUTPUT_BIN tinyblog)

include_directories(./libhv/build/include/hv ./moonrise/src ./moonrise/html ./cjson)

link_directories(./libhv/build/lib ./moonrise)

add_executable(${OUTPUT_BIN} ${TINYBLOG_SRC})
# add_executable(${OUTPUT_TEST_BIN} ${UTILSTEST_SRC})

target_link_libraries(${OUTPUT_BIN} hv sundown pthread)