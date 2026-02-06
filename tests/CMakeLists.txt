cmake_minimum_required(VERSION 4.0)
find_package(GTest REQUIRED)
include(GoogleTest)
enable_testing()

add_subdirectory(unit)
add_subdirectory(integration)

add_compile_definitions(BOOST_ASIO_ADL_WORKAROUND_NEEDED)
