set(CMAKE_AUTORCC ON)

cmake_minimum_required(VERSION 3.16)

project(my_bad_apple_project VERSION 1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BADAPPLE_RELEASE_ONLY "Build only the embedded release player" OFF)

if(NOT BADAPPLE_RELEASE_ONLY)
    find_package(OpenCV REQUIRED)
endif()
find_package(Qt6 REQUIRED COMPONENTS Widgets)


#include_directories(${OpenCV_INCLUDE_DIRS})


add_executable(BadAppleRelease release_embedded.cpp)
target_include_directories(BadAppleRelease PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(BadAppleRelease PRIVATE Qt6::Widgets)

if(NOT BADAPPLE_RELEASE_ONLY)
    add_executable(BadApple badapple.cpp)
    add_executable(testMatrix testMatrix.cpp)
    add_executable(Test1 test_video.cpp)
    add_executable(play play.cpp)
    add_executable(jumpbox jump_box.cpp assets/resources.qrc)
    add_executable(string_mode_play test_video_string.cpp)
    add_executable(cache_generator tools/generate_embedded_cache.cpp)
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
    target_link_libraries(cache_generator PRIVATE video)
    target_link_libraries(jumpbox PRIVATE Qt6::Widgets)
    target_link_libraries(video PUBLIC Qt6::Widgets)
    target_link_libraries(Test1 PRIVATE video)
    target_link_libraries(testMatrix PUBLIC matrix)
    target_link_libraries(string_mode_play video)

    if(NOT CMAKE_CROSSCOMPILING)
        add_custom_target(
            refresh_embedded_cache
            COMMAND cache_generator
                ${CMAKE_CURRENT_SOURCE_DIR}/build/video.mp4
                ${CMAKE_CURRENT_SOURCE_DIR}/generated/embedded_video_cache.hpp
                300
                100
            DEPENDS cache_generator
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating embedded playback cache from build/video.mp4"
        )

        add_dependencies(BadAppleRelease refresh_embedded_cache)
    endif()
endif()
