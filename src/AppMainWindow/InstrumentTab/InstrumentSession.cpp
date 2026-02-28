//
// Created by DED on 28.02.2026.
//

#include "InstrumentSession.h"

InstrumentSession::InstrumentSession(InstrumentContext* context, QObject* parent): QObject(parent), _context(context) {
}

void InstrumentSession::load(Market::TimePoint from, Market::TimePoint till) {
    _lastFrom = from;
    _lastTill = till;
}

void InstrumentSession::reload() {
    load(_lastFrom, _lastTill);
}

