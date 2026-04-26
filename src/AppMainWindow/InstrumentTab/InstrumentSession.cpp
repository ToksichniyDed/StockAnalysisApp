//
// Created by DED on 28.02.2026.
//

#include "InstrumentSession.h"

#include <QtConcurrent/QtConcurrentRun>
#include <utility>

InstrumentSession::InstrumentSession(InstrumentContext* context, std::shared_ptr<Exchange::IDataFetcher> fetcher, QObject* parent): QObject(parent), _context(context), _dataFetcher(std::move(fetcher)) {
}

void InstrumentSession::load(const Market::TimePoint from, const Market::TimePoint till) {
    _lastFrom = from;
    _lastTill = till;

    auto fetcher = _dataFetcher;
    auto ticker = _context->ticker();
    auto timeframe = _context->timeframe();

    auto future = QtConcurrent::run([this, fetcher, ticker, timeframe, from, till] {
        auto result = fetcher->fetchCandles(ticker, from, till, timeframe);

        if (!result.has_value()) {
            return;
        }

        _candles = std::move(result.value());
        emit signal_candlesReady();
    });
}

void InstrumentSession::reload() {
    load(_lastFrom, _lastTill);
}

