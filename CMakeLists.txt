cmake_minimum_required(VERSION 3.20)
project(mygit)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")
set(CMAKE_CXX_STANDARD 17)
# 头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)

# 获取所有源文件列表
file(GLOB_RECURSE SOURCES "src/*.cpp" "include/*.hpp" "test/*.cpp")

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 链接库
target_link_libraries(${PROJECT_NAME} -lcrypto -lgtest_main -lgtest)