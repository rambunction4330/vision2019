cmake_minimum_required(VERSION 3.1)
project( Vision )
find_package ( Threads REQUIRED )
find_package ( OpenCV REQUIRED )
add_executable(blob blob.cpp)
target_link_libraries ( blob ${CMAKE_THREAD_LIBS_INIT} ${OpenCV_LIBS})

add_compile_options(-std=c++0x)
