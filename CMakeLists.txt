cmake_minimum_required(VERSION 3.8.0)

project(run CXX)

set(SOURCE day2.cpp)

add_executable(${PROJECT_NAME} ${SOURCE})
add_definitions(-std:c++latest -wd4251 -wd4275)