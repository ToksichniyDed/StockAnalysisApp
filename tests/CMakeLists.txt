find_package(GTest REQUIRED)
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

add_test(NAME UnitTests COMMAND unit_tests)