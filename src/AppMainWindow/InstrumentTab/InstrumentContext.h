//
// Created by DED on 24.02.2026.
//

#ifndef COOLAPPNAME_INSTRUMENTCONTEXT_H
#define COOLAPPNAME_INSTRUMENTCONTEXT_H

#include <QObject>
#include <Market.h>

class InstrumentContext final : public QObject {

    Q_OBJECT

public:
    explicit InstrumentContext(Market::Ticker ticker,
                               Market::Timeframe timeframe = Market::Timeframe::Day,
                               QObject* parent = nullptr);
    ~InstrumentContext();

    [[nodiscard]] const Market::Ticker& ticker() const noexcept;
    [[nodiscard]] const Market::Timeframe& timeframe() const noexcept;

    [[nodiscard]] QString tickerName() const noexcept;
    [[nodiscard]] QString timeframeString() const noexcept;

public slots:
    void setTimeframe(const Market::Timeframe& timeframe) noexcept;
    void setTicker(const Market::Ticker& ticker) noexcept;

signals:
    void signal_tickerChanged(const Market::Ticker& ticker);
    void signal_timeframeChanged(Market::Timeframe timeframe);
    void signal_contextChanged();

private:
    Market::Ticker _ticker;
    Market::Timeframe _timeframe;
};


#endif //COOLAPPNAME_INSTRUMENTCONTEXT_H