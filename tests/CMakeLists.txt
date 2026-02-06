cmake_minimum_required(VERSION 3.31)
find_package(GTest REQUIRED)
include(GoogleTest)
enable_testing()

add_subdirectory(unit)
add_subdirectory(integration)
