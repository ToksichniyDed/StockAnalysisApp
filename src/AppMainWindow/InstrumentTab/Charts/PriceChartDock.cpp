//
// Created by DED on 28.02.2026.
//

#include "PriceChartDock.h"

PriceChartDock::PriceChartDock(InstrumentContext* context, InstrumentSession* session, QWidget* parent) : QDockWidget(
    context->tickerName(), parent) {
    setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
    );
    setObjectName(QString("PriceChartDock_%1").arg(context->tickerName()));
}
