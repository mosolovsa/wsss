cmake_minimum_required(VERSION 3.6)
project(wsss)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES main.c mongoose.c mongoose.h)
add_executable(wsss ${SOURCE_FILES})