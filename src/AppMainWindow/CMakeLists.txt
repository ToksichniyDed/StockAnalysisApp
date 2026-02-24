set(APP_MAIN_WINDOW_SOURCES
        appmainwindow.cpp
        appmainwindow.h
        AppTheme.h
)

set(APP_MAIN_WINDOW_UI
        AppMainWindow.ui
)

add_library(AppMainWindow STATIC
        ${APP_MAIN_WINDOW_SOURCES}
        ${APP_MAIN_WINDOW_UI}
)

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

