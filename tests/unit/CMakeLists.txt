add_executable(unit_tests Logger_Test.cpp)

add_subdirectory(StockExchangeConnectors_Tests)

set_target_properties(unit_tests PROPERTIES
        LABELS "unit"
)

target_link_libraries(unit_tests PRIVATE
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        spdlog::spdlog
        StockExchangeConnectors
        Logger
        NetworkTools
)

gtest_discover_tests(unit_tests)