cmake_minimum_required(VERSION 3.30)

project(SIPF)

file(GLOB_RECURSE SRC_FILES src/*.c)

add_executable(${PROJECT_NAME} ${SRC_FILES})
