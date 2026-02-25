//
// Created by DED on 21.12.2025.
//

module;

#include <algorithm>
#include <chrono>
#include <expected>
#include <optional>
#include <string>
#include <filesystem>
#include <utility>

export module IExchangeDataFetcher;

import ConfigurationParser;
import Logger;
import Market;

namespace Exchange {
    export enum class FetchStatus {
        Success, // Успешно
        NetworkError, // Connect/timeout/read от Beast
        HttpError, // Non-200, с optional code в struct
        ParseError, // JSON invalid
        NoDataFound, // Пустой ответ, invalid ticker
        InvalidParameter, // Локальная ошибка
        Unknown // Catch-all
    };

    export struct FetchError {
        FetchStatus status;
        std::string errorMessage;
        std::optional<int64_t> httpStatusCode;

        FetchError(const FetchStatus stat, std::string msg, int64_t code = 0) : status(stat), errorMessage(std::move(msg)),
                                                                            httpStatusCode(code) {
        }
    };

    export class IDataFetcher {
    public:
        virtual ~IDataFetcher() = default;

        [[nodiscard]] virtual std::expected<std::vector<Market::Candle>, Exchange::FetchError>
        fetchCandles(
            const Market::Ticker& ticker, const Market::TimePoint& from,
            const Market::TimePoint& till,
            const Market::Timeframe& timeframe = Market::Timeframe::Day) const = 0;

    protected:
        explicit IDataFetcher(std::shared_ptr<ConfigurationParser> configurationParser) : _configurationParser(
            std::move(configurationParser)) {

        };

    protected:
        std::shared_ptr<ConfigurationParser> _configurationParser;
    };
}
