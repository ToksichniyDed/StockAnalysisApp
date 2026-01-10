add_library(StockExchangeConnectors STATIC)

target_sources(StockExchangeConnectors PRIVATE FILE_SET CXX_MODULES FILES
        configs/ConfigurationParser.ixx
        IExchangeDataFetcher.ixx
        MOEXDataFetcher.ixx
)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS json beast)
find_package(tomlplusplus REQUIRED)

target_link_libraries(StockExchangeConnectors PRIVATE Boost::json Boost::headers tomlplusplus::tomlplusplus Logger NetworkTools)