CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

PROJECT(jero-robot)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_BUILD_TYPE "Release")
#TARGET_LINK_LIBRARIES(jero-robot ${GRM_LIBS} ${BOT_LIBS} ${ROS_LIBS} dl  ${catkin_LIBRARIES})

# 头文件搜索路径：自己的头 + third_party 头文件
include_directories(
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/inc/third_party
)

# 收集所有 cpp 源文件（包含 main.cpp）
file(GLOB_RECURSE JERO_SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/main.cpp
)

add_executable(jero-robot ${JERO_SOURCES})