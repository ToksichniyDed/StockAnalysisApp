set(INSTRUMENT_TAB_SOURCES
        InstrumentContext.cpp
        InstrumentTabButton.cpp)

set(INSTRUMENT_TAB_HEADERS
        InstrumentContext.h
        InstrumentTabButton.h)

add_library(InstrumentTab STATIC
        ${INSTRUMENT_TAB_SOURCES}
        ${INSTRUMENT_TAB_HEADERS}
)

target_link_libraries(InstrumentTab PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Market
        AppTheme)

target_include_directories(InstrumentTab
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)