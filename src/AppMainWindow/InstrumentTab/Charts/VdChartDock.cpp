//
// Created by DED on 28.02.2026.
//

#include "VdChartDock.h"

VdChartDock::VdChartDock(InstrumentContext* context, InstrumentSession* session, QWidget* parent) : QDockWidget(
    context->tickerName(), parent) {
    setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
    );
    setObjectName(QString("VdChartDock_%1").arg(context->tickerName()));
}
