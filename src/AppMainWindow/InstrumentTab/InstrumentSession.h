//
// Created by DED on 28.02.2026.
//

#ifndef COOLAPPNAME_INSTRUMENTSESSION_H
#define COOLAPPNAME_INSTRUMENTSESSION_H

#include "InstrumentContext.h"
#include <StockExchangeConnectors/IDataFetcher.h>

class InstrumentSession : public QObject {

    Q_OBJECT

public:
    explicit InstrumentSession(InstrumentContext* context, std::shared_ptr<Exchange::IDataFetcher> fetcher,
                               QObject* parent = nullptr);

    ~InstrumentSession() override = default;

    void load(Market::TimePoint from = std::chrono::system_clock::now() - std::chrono::days(30),
          Market::TimePoint till = std::chrono::system_clock::now());
    void reload();
    [[nodiscard]] std::vector<Market::Candle>& candles();

signals:
    void signal_candlesReady();
    void signal_CvdReady();
    void signal_LoadError(const QString& message);

private:
    InstrumentContext* _context = nullptr;
    std::shared_ptr<Exchange::IDataFetcher> _dataFetcher;
    Market::TimePoint _lastFrom{};
    Market::TimePoint _lastTill{};

    std::vector<Market::Candle> _candles;
};


#endif //COOLAPPNAME_INSTRUMENTSESSION_H
