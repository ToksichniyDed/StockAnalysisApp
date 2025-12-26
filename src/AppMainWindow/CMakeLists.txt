add_library(AppMainWindow STATIC
        appmainwindow.cpp)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Charts)

qt_standard_project_setup()

target_include_directories(AppMainWindow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(AppMainWindow PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Charts
        StockExchangeConnectors
)

