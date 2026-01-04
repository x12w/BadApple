set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTORCC ON)

cmake_minimum_required(VERSION 3.16)

project(my_bad_apple_project VERSION 1.0)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets)


#include_directories(${OpenCV_INCLUDE_DIRS})


add_executable(BadApple badapple.cpp)
add_executable(testMatrix testMatrix.cpp)
add_executable(Test1 test_video.cpp)
add_executable(play play.cpp)
add_executable(jumpbox jump_box.cpp assets/resources.qrc)
add_library(video src/video.cpp)
add_library(matrix src/matrix.cpp)

target_include_directories(video PUBLIC include)
target_include_directories(matrix PUBLIC include)
target_include_directories(BadApple PUBLIC ${OpenCV_INCLUDE_DIRS})
target_include_directories(testMatrix PUBLIC include)
target_link_libraries(Test1 PRIVATE ${OpenCV_LIBS})
target_link_libraries(BadApple PRIVATE ${OpenCV_LIBS})
target_link_libraries(play PRIVATE ${OpenCV_LIBS})
target_link_libraries(video PUBLIC ${OpenCV_LIBS} matrix)
target_link_libraries(BadApple PRIVATE Qt6::Widgets)
target_link_libraries(jumpbox PRIVATE Qt6::Widgets)
target_link_libraries(Test1 PRIVATE video)
target_link_libraries(testMatrix PUBLIC matrix)