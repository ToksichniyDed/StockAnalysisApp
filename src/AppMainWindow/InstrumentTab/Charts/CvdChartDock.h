//
// Created by DED on 28.02.2026.
//

#ifndef COOLAPPNAME_CVDCHARTDOCK_H
#define COOLAPPNAME_CVDCHARTDOCK_H

#include <QDockWidget>

#include "InstrumentContext.h"

class InstrumentContext;
class InstrumentSession;

class CvdChartDock : public QDockWidget {

    Q_OBJECT

public:
    explicit CvdChartDock(InstrumentContext* context,
                          InstrumentSession* session,
                          QWidget* parent = nullptr);

    ~CvdChartDock() = default;

};


#endif //COOLAPPNAME_CVDCHARTDOCK_H
