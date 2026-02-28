//
// Created by DED on 28.02.2026.
//

#include "InstrumentDockSet.h"

InstrumentDockSet::InstrumentDockSet(InstrumentContext* context, QMainWindow* mainWindow,
                                     QObject* parent) : QObject(parent)
                                                        , _mainWindow(mainWindow)
                                                        , _context(context)
                                                        , _session(new InstrumentSession(context, this)) {
    setupDocks();
}

InstrumentDockSet::~InstrumentDockSet() = default;

void InstrumentDockSet::show() {
    _priceDock->show();
    _cvdDock->show();
    _vdDock->show();

    if (_firstShow) {
        applyDefaultLayout();
        _firstShow = false;
    } else if (!_savedState.isEmpty()) {
        _mainWindow->restoreState(_savedState);
    }
}

void InstrumentDockSet::hide() {
    _savedState = _mainWindow->saveState();
    _priceDock->hide();
    _cvdDock->hide();
    _vdDock->hide();
}

InstrumentContext* InstrumentDockSet::context() const noexcept {
    return _context;
}

InstrumentSession* InstrumentDockSet::session() const noexcept {
    return _session;
}

void InstrumentDockSet::setupDocks() {
    _priceDock = new PriceChartDock(_context, _session, _mainWindow);
    _cvdDock = new CvdChartDock(_context, _session, _mainWindow);
    _vdDock = new VdChartDock(_context, _session, _mainWindow);

    _mainWindow->addDockWidget(Qt::TopDockWidgetArea, _priceDock);
    _mainWindow->addDockWidget(Qt::BottomDockWidgetArea, _cvdDock);
    _mainWindow->splitDockWidget(_cvdDock, _vdDock, Qt::Horizontal);

    _priceDock->hide();
    _cvdDock->hide();
    _vdDock->hide();
}

void InstrumentDockSet::applyDefaultLayout() {
    _mainWindow->resizeDocks(
        {_priceDock, _cvdDock, _vdDock},
        {500, 200, 200},
        Qt::Vertical
    );
    _mainWindow->resizeDocks(
        {_cvdDock, _vdDock},
        {1, 1},
        Qt::Horizontal
    );
}
