cmake_minimum_required(VERSION 3.16)
project(PNASSound VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2
find_package(SDL2 REQUIRED)

# Create executable
add_executable(pnas_sound main.cpp)

# Link SDL2
target_link_libraries(pnas_sound PRIVATE SDL2::SDL2)

# macOS specific settings
if(APPLE)
    target_link_libraries(pnas_sound PRIVATE "-framework CoreAudio" "-framework AudioToolbox")
endif()
