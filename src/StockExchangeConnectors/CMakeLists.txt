add_library(StockExchangeConnectors STATIC)

target_sources(StockExchangeConnectors PUBLIC FILE_SET CXX_MODULES FILES
        StockDataConfigurations/ConfigurationParser.ixx
        IDataFetcher.ixx
        MOEXDataFetcher.ixx
        MOEXResponseParser.ixx
)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS json beast)
find_package(tomlplusplus REQUIRED)

target_link_libraries(StockExchangeConnectors PRIVATE Boost::json Boost::headers tomlplusplus::tomlplusplus Logger NetworkTools Market)