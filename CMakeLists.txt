cmake_minimum_required(VERSION 3.27)
project(swapwindowv2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include local headers
include_directories(.)

find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET
        hyprland
        pixman-1
        libdrm
)

file(GLOB_RECURSE SRC "*.cpp")
add_library(swapwindowv2 SHARED ${SRC})
target_link_libraries(swapwindowv2 PRIVATE rt PkgConfig::deps)
install(TARGETS swapwindowv2)
