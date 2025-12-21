cmake_minimum_required(VERSION 3.16)
project(MotionDetector VERSION 1.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui)

add_executable(motion_detector
    src/main.cpp
    src/queues/frame_queue.cpp
    src/core/motion_detector.cpp
    src/core/video_capture.cpp
    src/core/motion_consumer.cpp
    src/gui/main_window.cpp
    include/gui/main_window.hpp
)


target_include_directories(motion_detector PRIVATE 
    include
    include/core
    include/queues
    include/models
    include/gui
)

target_link_libraries(motion_detector PRIVATE 
    ${OpenCV_LIBS} 
    Threads::Threads
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
)


# Debug build with sanitizers
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -g")


# Install
install(TARGETS motion_detector DESTINATION bin)
