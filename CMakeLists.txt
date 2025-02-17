cmake_minimum_required(VERSION 3.30)

project(IPF)

file(GLOB_RECURSE SIPF_SRC_FILES SIPF/src/*.c)
add_executable(SIPF ${SIPF_SRC_FILES})

file(GLOB_RECURSE CIPF_SRC_FILES CIPF/src/*.c)
add_executable(CIPF ${CIPF_SRC_FILES})