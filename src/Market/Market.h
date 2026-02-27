//
// Created by DED on 26.02.2026.
//

#ifndef COOLAPPNAME_MARKET_H
#define COOLAPPNAME_MARKET_H

#include <optional>
#include <chrono>
#include <algorithm>

namespace Market {
    using TimePoint = std::chrono::system_clock::time_point;

    enum class Timeframe : int {
        Minute1 = 1, // 1 минута
        Minute10 = 10, // 10 минут
        Hour = 60, // 1 час
        Day = 24, // 1 день (daily)
        Week = 7, // 1 неделя
        Month = 31, // 1 месяц
        Quarter = 4 // 1 квартал
    };

    [[nodiscard]] inline std::string timeframeToString(const Timeframe tf)
    {
        switch (tf) {
            case Timeframe::Minute1:  return "1m";
            case Timeframe::Minute10: return "10m";
            case Timeframe::Hour:     return "1h";
            case Timeframe::Day:      return "1D";
            case Timeframe::Week:     return "1W";
            case Timeframe::Month:    return "1M";
            case Timeframe::Quarter:  return "3M";
        }
        return "?";
    }

    struct Candle {
        TimePoint startPoint; // Время начала периода свечи
        double openPrice{}; // Цена открытия свечи
        double closePrice{}; // Цена закрытия свечи
        double highPrice{}; // Максимальная цена за период
        double lowPrice{}; // Минимальная цена за период
        double volume{}; // Объём торгов в лотах
        std::optional<double> value{}; // Оборот в валюте
        std::optional<TimePoint> endPoint{}; // Конец периода
        std::optional<double> turnover; // Альтернативный оборот
        std::optional<double> buyVolume; // Объём покупок
        std::optional<double> sellVolume; // Объём продаж

        [[nodiscard]] std::optional<double> delta() const noexcept {
            if (!buyVolume || !sellVolume)
                return std::nullopt;
            return *buyVolume - *sellVolume;
        }

        // Проверяет, является ли свеча бычьей
        [[nodiscard]] bool isBullish() const noexcept {
            return closePrice >= openPrice;
        }
    };

    class Ticker {
    public:
        explicit Ticker(std::string name){
            normalize(name);
            _name = (std::move(name));
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
}

#endif //COOLAPPNAME_MARKET_H