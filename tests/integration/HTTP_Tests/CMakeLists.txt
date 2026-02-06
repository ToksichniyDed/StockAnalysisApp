add_executable(Http_Tests)

target_sources(Http_Tests PRIVATE
        BoostBeastHttpClient_Test.cpp
)

target_link_libraries(Http_Tests
        PRIVATE
        GTest::gtest
        GTest::gtest_main
        NetworkTools
)

add_test(
        NAME HttpIntegrationTests
        COMMAND Http_Tests
)

set_tests_properties(HttpIntegrationTests PROPERTIES
        LABELS "integration_http_test"
        TIMEOUT 300
)

# Не запускаем автоматически - требует Docker
# gtest_discover_tests(BoostBeastHttpClientIntegrationTest)

# Добавляем custom target для запуска с Docker
add_custom_target(run_http_integration_tests
        COMMAND ${CMAKE_COMMAND} -E echo "Starting mock HTTP server..."
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR}/mock-server
        docker compose up -d
        COMMAND ${CMAKE_COMMAND} -E sleep 10
        COMMAND ${CMAKE_COMMAND} -E echo "Running tests..."
        COMMAND $<TARGET_FILE:Http_Tests>
        COMMAND ${CMAKE_COMMAND} -E echo "Stopping server..."
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR}/mock-server
        docker compose down -v
        DEPENDS Http_Tests
)