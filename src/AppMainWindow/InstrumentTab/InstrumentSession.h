//
// Created by DED on 28.02.2026.
//

#ifndef COOLAPPNAME_INSTRUMENTSESSION_H
#define COOLAPPNAME_INSTRUMENTSESSION_H

#include <QObject>

#include "InstrumentContext.h"

class InstrumentSession : public QObject {

    Q_OBJECT

public:
    explicit InstrumentSession(InstrumentContext* context,
                               QObject* parent = nullptr);

    ~InstrumentSession() override = default;

    void load(Market::TimePoint from, Market::TimePoint till);
    void reload();

signals:
    void signal_CandlesReady();

private:
    InstrumentContext* _context = nullptr;
    Market::TimePoint _lastFrom{};
    Market::TimePoint _lastTill{};

    std::vector<Market::Candle> _candles;
};


#endif //COOLAPPNAME_INSTRUMENTSESSION_H
