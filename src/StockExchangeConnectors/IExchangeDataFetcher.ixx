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

namespace ExchangeDataFetcher {
    export using TimePoint = std::chrono::system_clock::time_point;

    export enum class Timeframe : int {
        Minute1 = 1, // 1 минута
        Minute10 = 10, // 10 минут
        Hour = 60, // 1 час
        Day = 24, // 1 день (daily)
        Week = 7, // 1 неделя
        Month = 31, // 1 месяц
        Quarter = 4 // 1 квартал
    };

    export struct BaseCandle {
        TimePoint startPoint; // Время начала периода свечи
        double openValue{}; // Цена открытия свечи
        double closeValue{}; // Цена закрытия свечи
        double highValue{}; // Максимальная цена за период
        double smallestValue{}; // Минимальная цена за период
        double volume{}; // Объём торгов в лотах
        std::optional<double> value{}; // Оборот в валюте
        std::optional<TimePoint> endPoint{}; // Конец периода
    };

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

        FetchError(const FetchStatus stat, std::string msg, int code = 0) : status(stat), errorMessage(std::move(msg)),
                                                                            httpStatusCode(code) {
            Logger::log<Logger::LogLevel::Error>("Ошибка запроса данных [статус: {}]: {}", static_cast<int>(status),
                                                 errorMessage);
        }
    };
}

export class Ticker {
public:
    explicit Ticker(std::string name) : _name(std::move(name)) {
    };

    bool operator==(const Ticker& other) const noexcept = default;
    auto operator<=>(const Ticker& other) const noexcept = default;

    [[nodiscard]] const std::string& name() const noexcept { return _name; };

protected:
    void normalize(std::string& str) {
        str.erase(0, str.find_first_not_of(" \t"));
        str.erase(str.find_last_not_of(" \t") + 1);

        std::ranges::transform(str, str.begin(), [](const unsigned char c) { return std::toupper(c); });
    }

protected:
    std::string _name;
};

export class IExchangeDataFetcher {
public:
    virtual ~IExchangeDataFetcher() = default;

    [[nodiscard]] virtual std::expected<std::vector<ExchangeDataFetcher::BaseCandle>, ExchangeDataFetcher::FetchError>
    fetchCandles(
        const Ticker& ticker, const ExchangeDataFetcher::TimePoint& from,
        const ExchangeDataFetcher::TimePoint& till,
        const ExchangeDataFetcher::Timeframe& timeframe = ExchangeDataFetcher::Timeframe::Day) const = 0;

protected:
    explicit IExchangeDataFetcher(std::shared_ptr<ConfigurationParser> configurationParser) : _configurationParser(
        std::move(configurationParser)) {

    };

protected:
    std::shared_ptr<ConfigurationParser> _configurationParser;
};
