//
// Created by DED on 25.04.2026.
//

#ifndef COOLAPPNAME_IDATAFETCHER_H
#define COOLAPPNAME_IDATAFETCHER_H


#include <algorithm>
#include <chrono>
#include <expected>
#include <optional>
#include <string>
#include <filesystem>
#include <utility>

#include <StockExchangeConnectors/StockDataConfigurations/ConfigurationParser.h>
#include <Logger/Logger.h>
#include <Market/Market.h>

namespace Exchange {
    enum class FetchStatus {
        Success, // Успешно
        NetworkError, // Connect/timeout/read от Beast
        HttpError, // Non-200, с optional code в struct
        ParseError, // JSON invalid
        NoDataFound, // Пустой ответ, invalid ticker
        InvalidParameter, // Локальная ошибка
        Unknown // Catch-all
    };

    struct FetchError {
        FetchStatus status;
        std::string errorMessage;
        std::optional<int64_t> httpStatusCode;

        FetchError(const FetchStatus stat, std::string msg, int64_t code = 0) : status(stat), errorMessage(std::move(msg)),
                                                                            httpStatusCode(code) {
        }
    };

    class IDataFetcher {
    public:
        virtual ~IDataFetcher() = default;

        [[nodiscard]] virtual std::expected<std::vector<Market::Candle>, Exchange::FetchError>
        fetchCandles(
            const Market::Ticker& ticker, const Market::TimePoint& from,
            const Market::TimePoint& till,
            const Market::Timeframe& timeframe = Market::Timeframe::Day) const = 0;

    protected:
        explicit IDataFetcher(std::shared_ptr<Parser::ConfigurationParser> configurationParser) : _configurationParser(
            std::move(configurationParser)) {

        };

    protected:
        std::shared_ptr<Parser::ConfigurationParser> _configurationParser;
    };
}


#endif //COOLAPPNAME_IDATAFETCHER_H