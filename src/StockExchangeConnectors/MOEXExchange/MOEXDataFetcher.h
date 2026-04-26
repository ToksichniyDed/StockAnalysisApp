//
// Created by DED on 25.04.2026.
//

#ifndef COOLAPPNAME_MOEXDATAFETCHER_H
#define COOLAPPNAME_MOEXDATAFETCHER_H

#include <vector>
#include <expected>
#include <filesystem>

#include <boost/json.hpp>
#include <boost/system/error_code.hpp>

#include <StockExchangeConnectors/IDataFetcher.h>
#include <StockExchangeConnectors/MOEXExchange/MOEXResponseParser.h>
#include <StockExchangeConnectors/StockDataConfigurations/ConfigurationParser.h>
#include <NetworkTools/IHttpClient.h>

#include "MOEXconfig.h"

class MOEXDataFetcher : public Exchange::IDataFetcher {
public:
    explicit MOEXDataFetcher(const std::filesystem::path& filePath = {},
                             std::shared_ptr<IHttpClient> httpClient = nullptr,
                             std::shared_ptr<MOEXResponseParser> responseParser = nullptr,
                             const std::shared_ptr<Parser::ConfigurationParser>& configurationParser = nullptr);

    ~MOEXDataFetcher() override = default;

protected:
    [[nodiscard]] std::expected<std::vector<Market::Candle>, Exchange::FetchError> fetchCandles(
        const Market::Ticker& ticker, const Market::TimePoint& from,
        const Market::TimePoint& till,
        const Market::Timeframe& timeframe = Market::Timeframe::Day) const override;

protected:
    std::shared_ptr<IHttpClient> _httpClient;
    std::shared_ptr<MOEXResponseParser> _moexResponseParser;
    std::shared_ptr<Parser::ConfigurationParser> _configurationParser;
};

#endif //COOLAPPNAME_MOEXDATAFETCHER_H