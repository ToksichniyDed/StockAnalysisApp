add_executable(Http_Tests
        BoostBeastHttpClient_Test.cpp
)

target_link_libraries(Http_Tests
        PRIVATE
        GTest::gtest
        GTest::gtest_main
        NetworkTools
)

set_target_properties(Http_Tests
        PROPERTIES
        LABELS "integration"
)

# Не запускаем автоматически - требует Docker
# gtest_discover_tests(BoostBeastHttpClientIntegrationTest)

# Добавляем custom target для запуска с Docker
add_custom_target(run_http_integration_tests
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR}
        docker-compose up -d
        COMMAND ${CMAKE_COMMAND} -E sleep 5
        COMMAND $<TARGET_FILE:Http_Tests>
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR}
        docker-compose down
        DEPENDS Http_Tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)