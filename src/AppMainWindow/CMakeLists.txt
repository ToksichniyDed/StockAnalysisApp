find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Charts)

qt_standard_project_setup()

add_subdirectory(InstrumentTab)

set(APP_MAIN_WINDOW_SOURCES
        AppMainWindow.cpp
        InstrumentTabBar.cpp
        ActivityBar.cpp
)

set(APP_MAIN_WINDOW_HEADERS
        AppMainWindow.h
        InstrumentTabBar.h
        ActivityBar.h
)

add_library(AppMainWindow STATIC
        ${APP_MAIN_WINDOW_SOURCES}
        ${APP_MAIN_WINDOW_HEADERS}
)

target_include_directories(AppMainWindow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(AppMainWindow PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        StockExchangeConnectors
        AppTheme
        PUBLIC
        InstrumentTab
        Market
)

target_include_directories(AppMainWindow
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(AppTheme STATIC AppTheme.h)

target_include_directories(AppTheme
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

