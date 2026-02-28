//
// Created by DED on 20.12.2025.
//

#include <QToolBar>

#include <Market.h>
#include "AppMainWindow.h"
#include "AppTheme.h"

AppMainWindow::AppMainWindow(QWidget* parent) : QMainWindow(parent) {
    setupUi();
    setStyleSheet(AppTheme::baseStyleSheet());
    setPalette(AppTheme::buildPalette());
    resize(1400, 860);
}

AppMainWindow::~AppMainWindow() {
}

void AppMainWindow::slot_AddInstrumentTab(const QString& ticker) {
    //проверка на дубликат
    for (const auto* dock : _dockSets) {
        if (dock->context()->tickerName() == ticker) {
            slot_InstrumentTabClicked(_dockSets.indexOf(dock));
            return;
        }
    }

    auto newContext = new InstrumentContext(Market::Ticker{ticker.toStdString()}, Market::Timeframe::Day, this);
    auto* newDockSet = new InstrumentDockSet(newContext, this, this);
    _dockSets.append(newDockSet);

    _instrumentTabBar->addInstrumentTab(newContext);

    slot_InstrumentTabClicked(_dockSets.size() - 1);
}

void AppMainWindow::slot_InstrumentTabClicked(int index) {
    if (index < 0 || index >= _dockSets.size())
        return;

    if (index == _activeIndex)
        return;

    if (_activeIndex >= 0 && _activeIndex < _dockSets.size())
        _dockSets.at(_activeIndex)->hide();

    _dockSets.at(index)->show();
    _activeIndex = index;

    _instrumentTabBar->setActiveInstrumentTab(index);
}

void AppMainWindow::slot_InstrumentTabCloseRequested(int index) {
    if (index < 0 || index >= _dockSets.size())
        return;

    if (index == _activeIndex) {
        const int next = (index > 0) ? index - 1 : index + 1;
        if (next < _dockSets.size() && next != index)
            slot_InstrumentTabClicked(next);
        else
            _activeIndex = -1;
    } else if (index < _activeIndex) {
        // сдвигаем индекс активного
        --_activeIndex;
    }

    auto context = _dockSets.takeAt(index);
    context->deleteLater();

    _instrumentTabBar->removeInstrumentTab(index);

    if (_dockSets.empty()) {
        _activeIndex = -1;
        return;
    }

    // пересчитываем активный индекс
    if (index < _activeIndex)
        --_activeIndex;
    else if (index == _activeIndex)
        _activeIndex = -1;

    const int next = qMin(index, _dockSets.size() - 1);
    slot_InstrumentTabClicked(next);
}

void AppMainWindow::setupUi() {
    auto* tabToolBar = new QToolBar(this);
    tabToolBar->setMovable(false);
    tabToolBar->setFloatable(false);
    tabToolBar->setContentsMargins(0, 0, 0, 0);

    _instrumentTabBar = new InstrumentTabBar(this);
    tabToolBar->addWidget(_instrumentTabBar);

    addToolBar(Qt::TopToolBarArea, tabToolBar);

    connect(_instrumentTabBar, &InstrumentTabBar::signal_AddInstrumentTabRequested, this, &AppMainWindow::slot_AddInstrumentTab);
    connect(_instrumentTabBar, &InstrumentTabBar::signal_InstrumentTabClicked, this, &AppMainWindow::slot_InstrumentTabClicked);
    connect(_instrumentTabBar, &InstrumentTabBar::signal_InstrumentTabCloseRequested, this, &AppMainWindow::slot_InstrumentTabCloseRequested);

    // auto centralWidget = new QWidget(this);
    // setCentralWidget(centralWidget);
    setDockNestingEnabled(true);
}
