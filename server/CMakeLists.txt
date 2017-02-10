cmake_minimum_required(VERSION 2.8)
project( Server )
## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED)

include_directories(${catkin_INCLUDE_DIRS})
catkin_package(
   INCLUDE_DIRS include
   LIBRARIES ${Server}
   CATKIN_DEPENDS roscpp nodelet
   DEPENDS eigen opencv)
## System dependencies are found with CMake's conventions
# find_package(Boost REQUIRED COMPONENTS system)
find_package(catkin REQUIRED COMPONENTS 
  roscpp
  rospy
  std_msgs
  geometry_msgs
  message_generation
)
add_executable( server main.cpp )

