add_library(Market)

target_sources(Market PUBLIC FILE_SET CXX_MODULES FILES
        Market.ixx
)

target_include_directories(Market
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(Market PUBLIC Market.h)