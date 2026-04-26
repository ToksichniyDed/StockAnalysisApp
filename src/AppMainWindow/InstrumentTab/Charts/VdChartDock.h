//
// Created by DED on 28.02.2026.
//

#ifndef COOLAPPNAME_VDCHARTDOCK_H
#define COOLAPPNAME_VDCHARTDOCK_H

#include <QDockWidget>

class InstrumentContext;
class InstrumentSession;

class VdChartDock : public QDockWidget {

    Q_OBJECT

public:
    explicit VdChartDock(InstrumentContext* context,
                         InstrumentSession* session,
                         QWidget* parent = nullptr);

    ~VdChartDock() = default;

};


#endif //COOLAPPNAME_VDCHARTDOCK_H
