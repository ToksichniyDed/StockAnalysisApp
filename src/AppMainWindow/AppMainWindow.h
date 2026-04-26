//
// Created by DED on 20.12.2025.
//

#ifndef COOLAPPNAME_APPMAINWINDOW_H
#define COOLAPPNAME_APPMAINWINDOW_H

#include <InstrumentTab/InstrumentDockSet.h>
#include <InstrumentTabBar.h>

#include "ActivityBar.h"

class AppMainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit AppMainWindow(QWidget* parent = nullptr);
    ~AppMainWindow() override = default;

private slots:
    void slot_AddInstrumentTab(const QString& ticker);
    void slot_InstrumentTabClicked(int index);
    void slot_InstrumentTabCloseRequested(int index);

private:
    void setupUi();

private:
    InstrumentTabBar* _instrumentTabBar = nullptr;
    ActivityBar* _activityBar = nullptr;
    QList<InstrumentDockSet*> _dockSets;
    int _activeIndex = -1;
};


#endif //COOLAPPNAME_APPMAINWINDOW_H
