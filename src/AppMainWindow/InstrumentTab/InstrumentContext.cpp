//
// Created by DED on 24.02.2026.
//

#include "InstrumentContext.h"

#include <Logger/Logger.h>

InstrumentContext::InstrumentContext(Market::Ticker ticker, Market::Timeframe timeframe, QObject* parent) : QObject(parent), _ticker(
    std::move(ticker)), _timeframe(timeframe) {
}

InstrumentContext::~InstrumentContext() = default;

const Market::Ticker& InstrumentContext::ticker() const noexcept {
    return _ticker;
}

const Market::Timeframe& InstrumentContext::timeframe() const noexcept {
    return _timeframe;
}

QString InstrumentContext::tickerName() const noexcept {
    return QString::fromStdString(_ticker.name());
}

QString InstrumentContext::timeframeString() const noexcept {
    return QString::fromStdString(Market::timeframeToString(_timeframe));
}

void InstrumentContext::setTimeframe(const Market::Timeframe& timeframe) noexcept {
    if (timeframe == _timeframe)
        return;
    _timeframe = timeframe;

    Logger::log<Logger::LogLevel::Debug>("Установлен таймфрейм!");

    emit signal_timeframeChanged(_timeframe);
    emit signal_contextChanged();
}

void InstrumentContext::setTicker(const Market::Ticker& ticker) noexcept {
    if (ticker == _ticker)
        return;
    _ticker = ticker;

    Logger::log<Logger::LogLevel::Debug>("Установлен тикер!");

    emit signal_tickerChanged(_ticker);
    emit signal_contextChanged();
}
