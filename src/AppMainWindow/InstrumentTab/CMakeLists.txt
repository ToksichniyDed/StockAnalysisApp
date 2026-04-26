set(INSTRUMENT_TAB_SOURCES
        InstrumentContext.cpp
        InstrumentTabButton.cpp
        InstrumentDockSet.cpp
        InstrumentSession.cpp

        Charts/CvdChartDock.cpp
        Charts/VdChartDock.cpp
        Charts/PriceChartDock.cpp
        Charts/CandleRenderer.cpp
)

set(INSTRUMENT_TAB_HEADERS
        InstrumentContext.h
        InstrumentTabButton.h
        InstrumentDockSet.h
        InstrumentSession.h

        Charts/CvdChartDock.h
        Charts/VdChartDock.h
        Charts/PriceChartDock.h
        Charts/CandleRenderer.h
)

add_library(InstrumentTab STATIC
        ${INSTRUMENT_TAB_SOURCES}
        ${INSTRUMENT_TAB_HEADERS}
)

target_link_libraries(InstrumentTab PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets

        Market
        AppTheme
        StockExchangeConnectors
)