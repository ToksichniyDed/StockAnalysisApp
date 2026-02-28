//
// Created by DED on 28.02.2026.
//

#ifndef COOLAPPNAME_PRICECHARTDOCK_H
#define COOLAPPNAME_PRICECHARTDOCK_H

#include <QDockWidget>

#include "InstrumentContext.h"

class InstrumentContext;
class InstrumentSession;

class PriceChartDock : public QDockWidget {

    Q_OBJECT

public:
    explicit PriceChartDock(InstrumentContext* context,
                            InstrumentSession* session,
                            QWidget* parent = nullptr);

    ~PriceChartDock() = default;
};


#endif //COOLAPPNAME_PRICECHARTDOCK_H
