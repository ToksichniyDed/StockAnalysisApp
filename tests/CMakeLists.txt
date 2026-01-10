find_package(GTest REQUIRED)
include(GoogleTest)
enable_testing()

add_executable(unit_tests
Logger_Test.cpp)

target_link_libraries(GTest::GTest INTERFACE gtest_main)
target_link_libraries(unit_tests PRIVATE
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        spdlog::spdlog
        StockExchangeConnectors
        Logger)

gtest_discover_tests(unit_tests)