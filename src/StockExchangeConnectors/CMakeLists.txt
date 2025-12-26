add_library(StockExchangeConnectors STATIC)

target_sources(StockExchangeConnectors PRIVATE FILE_SET CXX_MODULES FILES
        IExchangeDataFetcher.cppm
        MOEXDataFetcher.cppm
        configs/ConfigurationParser.cppm)

find_package(Boost REQUIRED COMPONENTS json beast)

target_link_libraries(StockExchangeConnectors PRIVATE Boost::json Boost::headers)