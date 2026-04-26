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
            Logger::log<Logger::LogLevel::Error>("Запрос свечей по тикету {} закончился с ошибкой {}!", ticker.name(), result.error().errorMessage);
            signal_LoadError(result.error().errorMessage.data());
            return;
        }

        Logger::log<Logger::LogLevel::Debug>("Запрос свечей по тикету {} закончился успешно!", ticker.name());
        _candles = std::move(result.value());
        emit signal_candlesReady();
    });

    Logger::log<Logger::LogLevel::Debug>("Запрошенны данные по тикету {}!", ticker.name());
}

void InstrumentSession::reload() {
    load(_lastFrom, _lastTill);
}

