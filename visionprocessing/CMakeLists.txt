cmake_minimum_required(VERSION 3.1)
project( Vision )
find_package ( Threads REQUIRED )
find_package ( OpenCV REQUIRED )
add_executable(main main.cpp)
add_executable(mainElevatorCamera mainElevatorCamera.cpp)
add_executable(mainShooterCamera mainShooterCamera.cpp)
target_link_libraries ( main ${CMAKE_THREAD_LIBS_INIT} ${OpenCV_LIBS})
target_link_libraries ( mainElevatorCamera ${CMAKE_THREAD_LIBS_INIT} ${OpenCV_LIBS})
target_link_libraries ( mainShooterCamera ${CMAKE_THREAD_LIBS_INIT} ${OpenCV_LIBS})

add_compile_options(-std=c++0x)
