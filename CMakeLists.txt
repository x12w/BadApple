cmake_minimum_required(VERSION 3.10)

project(my_opencv_project VERSION 1.0)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)


include_directories(${OpenCV_INCLUDE_DIRS})


add_executable(my_opencv_app main.cpp)


target_link_libraries(my_opencv_app PRIVATE ${OpenCV_LIBS})