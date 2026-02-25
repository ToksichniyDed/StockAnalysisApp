//
// Created by DED on 26.02.2026.
//

module;

#include <optional>
#include <chrono>
#include <algorithm>

export module Market;

namespace Market {
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

    export struct Candle {
        TimePoint startPoint; // Время начала периода свечи
        double openValue{}; // Цена открытия свечи
        double closeValue{}; // Цена закрытия свечи
        double highValue{}; // Максимальная цена за период
        double smallestValue{}; // Минимальная цена за период
        double volume{}; // Объём торгов в лотах
        std::optional<double> value{}; // Оборот в валюте
        std::optional<TimePoint> endPoint{}; // Конец периода
    };

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
}
