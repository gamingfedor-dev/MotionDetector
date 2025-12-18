cmake_minimum_required(VERSION 3.16)
project(MotionDetector VERSION 1.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)


add_executable(motion_detector
    src/main.cpp
    include/frame_buffer.cpp
    include/motion_detector.cpp
    include/video_capture.cpp
    include/motion_consumer.cpp
)


target_include_directories(motion_detector PRIVATE include)
target_link_libraries(motion_detector PRIVATE ${OpenCV_LIBS} Threads::Threads)


# Debug build with sanitizers
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -g")


# Install
install(TARGETS motion_detector DESTINATION bin)
