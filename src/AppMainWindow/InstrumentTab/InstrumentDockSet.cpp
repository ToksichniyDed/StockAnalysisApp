//
// Created by DED on 28.02.2026.
//

#include "InstrumentDockSet.h"
#include <Logger/Logger.h>

InstrumentDockSet::InstrumentDockSet(InstrumentContext* context, const std::shared_ptr<Exchange::IDataFetcher>& fetcher, QMainWindow* mainWindow,
                                     QObject* parent) : QObject(parent)
                                                        , _mainWindow(mainWindow)
                                                        , _context(context)
                                                        , _session(new InstrumentSession(context, fetcher, this)) {
    setupDocks();

    connect(_session, &InstrumentSession::signal_candlesReady, this, &InstrumentDockSet::slot_PriceChartSetCandles);
    connect(_session, &InstrumentSession::signal_candlesReady, this, &InstrumentDockSet::slot_CvdChartSetCandles);
    connect(_session, &InstrumentSession::signal_candlesReady, this, &InstrumentDockSet::slot_VdChartSetCandles);

    _session->load();
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
        //TODO: восстановление положения доков между сессиями
        //_mainWindow->restoreState(_savedState);
    }

    Logger::log<Logger::LogLevel::Debug>("Отображение доксета {}!", _context->tickerName().toStdString());
}

void InstrumentDockSet::hide() {
    //TODO: сохранение положения доков между сессиями
    //_savedState = _mainWindow->saveState();
    _priceDock->hide();
    _cvdDock->hide();
    _vdDock->hide();

    Logger::log<Logger::LogLevel::Debug>("Скрытие доксета {}!", _context->tickerName().toStdString());
}

InstrumentContext* InstrumentDockSet::context() const noexcept {
    return _context;
}

InstrumentSession* InstrumentDockSet::session() const noexcept {
    return _session;
}

bool InstrumentDockSet::priceChartDockVisible() const noexcept {
    return _priceDock->isVisible();
}

bool InstrumentDockSet::cvdChartDockVisible() const noexcept {
    return _cvdDock->isVisible();
}

bool InstrumentDockSet::vdChartDockVisible() const noexcept {
    return _vdDock->isVisible();
}

void InstrumentDockSet::slot_PriceChartDockVisible(const bool visible) {
    QSignalBlocker blocker(_priceDock);
    _priceDock->setVisible(visible);
}

void InstrumentDockSet::slot_CvdChartDockVisible(const bool visible) {
    QSignalBlocker blocker(_cvdDock);
    _cvdDock->setVisible(visible);
}

void InstrumentDockSet::slot_VdChartDockVisible(const bool visible) {
    QSignalBlocker blocker(_vdDock);
    _vdDock->setVisible(visible);
}

void InstrumentDockSet::slot_ResetDocksLayout() {
    applyDefaultLayout();
}

void InstrumentDockSet::slot_PriceChartSetCandles() const {
    _priceDock->widget()->setCandles(_session->candles());
}

void InstrumentDockSet::slot_CvdChartSetCandles() {
    _cvdDock->widget()
}

void InstrumentDockSet::slot_VdChartSetCandles() {
}

void InstrumentDockSet::setupDocks() {
    _priceDock = new PriceChartDock(_context, _session, _mainWindow);
    _cvdDock = new CvdChartDock(_context, _session, _mainWindow);
    _vdDock = new VdChartDock(_context, _session, _mainWindow);

    connect(_priceDock, &QDockWidget::visibilityChanged, this, &InstrumentDockSet::signal_PriceChartDockChangedVisible);
    connect(_cvdDock, &QDockWidget::visibilityChanged, this, &InstrumentDockSet::signal_CvdChartDockChangedVisible);
    connect(_vdDock, &QDockWidget::visibilityChanged, this, &InstrumentDockSet::signal_VdChartDockChangedVisible);

    _mainWindow->addDockWidget(Qt::TopDockWidgetArea, _priceDock);
    _mainWindow->addDockWidget(Qt::BottomDockWidgetArea, _cvdDock);
    _mainWindow->splitDockWidget(_cvdDock, _vdDock, Qt::Horizontal);

    _priceDock->hide();
    _cvdDock->hide();
    _vdDock->hide();
}

void InstrumentDockSet::applyDefaultLayout() {
    if (_priceDock->isFloating())
        _priceDock->setFloating(false);
    if (_cvdDock->isFloating())
        _cvdDock->setFloating(false);
    if (_vdDock->isFloating())
        _vdDock->setFloating(false);

    _priceDock->show();
    _cvdDock->show();
    _vdDock->show();

    // пересобираем структуру
    _mainWindow->addDockWidget(Qt::TopDockWidgetArea, _priceDock);
    _mainWindow->addDockWidget(Qt::TopDockWidgetArea, _cvdDock);
    _mainWindow->addDockWidget(Qt::TopDockWidgetArea, _vdDock);

    _mainWindow->splitDockWidget(_priceDock, _cvdDock, Qt::Vertical);
    _mainWindow->splitDockWidget(_cvdDock, _vdDock, Qt::Horizontal);

    _mainWindow->resizeDocks(
        {_priceDock, _cvdDock},
        {560, 240},
        Qt::Vertical
    );
    _mainWindow->resizeDocks(
        {_cvdDock, _vdDock},
        {1, 1},
        Qt::Horizontal
    );
}
