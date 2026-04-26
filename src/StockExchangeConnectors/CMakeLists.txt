#target_sources(StockExchangeConnectors PUBLIC FILE_SET CXX_MODULES FILES
#        StockDataConfigurations/ConfigurationParser.ixx
#        IDataFetcher.ixx
#        MOEXDataFetcher.ixx
#        MOEXResponseParser.ixx
#)

set(STOCK_EXCHANGE_CONNECTORS_HEADERS
        IDataFetcher.h

        MOEXExchange/MOEXDataFetcher.h
        MOEXExchange/MOEXResponseParser.h

        StockDataConfigurations/ConfigurationParser.h
)

set(STOCK_EXCHANGE_CONNECTORS_SOURCE
        MOEXExchange/MOEXDataFetcher.cpp
        MOEXExchange/MOEXResponseParser.cpp

        StockDataConfigurations/ConfigurationParser.cpp
)

set(MOEX_CONFIG_PATH
        "${CMAKE_CURRENT_SOURCE_DIR}/StockDataConfigurations/MOEX/MOEXconfig.toml"
)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/StockDataConfigurations/MOEX/MOEXconfig.h.in
        ${CMAKE_BINARY_DIR}/generated/MOEXconfig.h
)

add_library(StockExchangeConnectors STATIC
        ${STOCK_EXCHANGE_CONNECTORS_HEADERS}
        ${STOCK_EXCHANGE_CONNECTORS_SOURCE}
        ${STOCK_EXCHANGE_DATA_CONFIGURATIONS}
)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS json beast)
find_package(tomlplusplus REQUIRED)

target_link_libraries(StockExchangeConnectors PUBLIC Boost::json Boost::headers tomlplusplus::tomlplusplus Logger NetworkTools Market)

target_include_directories(StockExchangeConnectors
        PUBLIC
        ${PROJECT_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/generated
)