//
// Created by DED on 28.02.2026.
//

#include "CvdChartDock.h"

#include <InstrumentTab/InstrumentContext.h>

CvdChartDock::CvdChartDock(InstrumentContext* context, InstrumentSession* session, QWidget* parent) : QDockWidget(
    context->tickerName(), parent) {
    setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
    );
    setObjectName(QString("CvdChartDock_%1").arg(context->tickerName()));
}
