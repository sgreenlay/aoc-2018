cmake_minimum_required(VERSION 3.8.0)

file(GLOB SRCS *.cpp)
foreach(SRC IN LISTS SRCS)
  get_filename_component(EXE "${SRC}" NAME_WE)
  add_executable(${EXE} ${SRC})
  add_definitions(-std:c++latest -wd4251 -wd4275)
endforeach()
