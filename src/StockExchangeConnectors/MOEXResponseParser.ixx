//
// Created by DED on 26.01.2026.
//

module;

#include <expected>
#include <string_view>
#include <string>
#include <vector>
#include <chrono>

#include <boost/json.hpp>
#include <boost/system/error_code.hpp>

export module MOEXResponseParser;

import IExchangeDataFetcher;
import Logger;
import Market;

export class MOEXResponseParser {
public:
    MOEXResponseParser() = default;
    ~MOEXResponseParser() = default;

    [[nodiscard]] std::expected<std::vector<Market::Candle>, Exchange::FetchError>
    parse(const std::string_view& jsonBody) const {
        boost::system::error_code errorCode;
        boost::json::value jsonValue = boost::json::parse(jsonBody, errorCode);

        if (errorCode) {
            return std::unexpected(Exchange::FetchError{
                Exchange::FetchStatus::ParseError,
                std::string("MOEX JSON request parse error: ") + errorCode.message()
            });
        }

        if (!jsonValue.is_object()) {
            return std::unexpected(Exchange::FetchError{
                Exchange::FetchStatus::ParseError,
                "Response is not a JSON object"
            });
        }

        const boost::json::object& response = jsonValue.as_object();

        if (!response.at("candles").is_object()) {
            return std::unexpected(Exchange::FetchError{
                Exchange::FetchStatus::ParseError,
                "'candles' is not an object"
            });
        }

        if (!response.at("data").is_array()) {
            return std::unexpected(Exchange::FetchError{
                Exchange::FetchStatus::ParseError,
                "'data' is not an array"
            });
        }

        const auto& data = response.at("data").as_array();

        if (data.empty()) {
            return std::unexpected(Exchange::FetchError{
                Exchange::FetchStatus::NoDataFound,
                std::string("No data found: ")
            });
        }

        std::vector<Market::Candle> candlesVector;
        candlesVector.reserve(data.size());

        for (const auto& row : data) {
            const auto& values = row.as_array();

            if (values.size() != _moexCandleColumnCount) {
                return std::unexpected(Exchange::FetchError{
                    Exchange::FetchStatus::InvalidParameter,
                    std::string("Candles count mismatch: ") + std::to_string(values.size())
                });
            }

            try {
                Market::Candle candle;

                if (!values[0].is_double() && !values[0].is_int64()) {
                    return std::unexpected(Exchange::FetchError{
                        Exchange::FetchStatus::ParseError,
                        "Invalid type for 'open'"
                    });
                }
                candle.openPrice = values[0].is_double()
                                   ? values[0].as_double()
                                   : static_cast<double>(values[0].as_int64());

                if (!values[1].is_double() && !values[1].is_int64()) {
                    return std::unexpected(Exchange::FetchError{
                        Exchange::FetchStatus::ParseError,
                        "Invalid type for 'close'"
                    });
                }
                candle.closePrice = values[1].is_double()
                                    ? values[1].as_double()
                                    : static_cast<double>(values[1].as_int64());

                if (!values[2].is_double() && !values[2].is_int64()) {
                    return std::unexpected(Exchange::FetchError{
                        Exchange::FetchStatus::ParseError,
                        "Invalid type for 'high'"
                    });
                }
                candle.highPrice = values[2].is_double()
                                   ? values[2].as_double()
                                   : static_cast<double>(values[2].as_int64());

                if (!values[3].is_double() && !values[3].is_int64()) {
                    return std::unexpected(Exchange::FetchError{
                        Exchange::FetchStatus::ParseError,
                        "Invalid type for 'low'"
                    });
                }
                candle.lowPrice = values[3].is_double()
                                       ? values[3].as_double()
                                       : static_cast<double>(values[3].as_int64());

                if (!values[4].is_double() && !values[4].is_int64()) {
                    return std::unexpected(Exchange::FetchError{
                        Exchange::FetchStatus::ParseError,
                        "Invalid type for 'value'"
                    });
                }
                candle.value = values[4].is_double()
                               ? values[4].as_double()
                               : static_cast<double>(values[4].as_int64());

                if (!values[5].is_double() && !values[5].is_int64()) {
                    return std::unexpected(Exchange::FetchError{
                        Exchange::FetchStatus::ParseError,
                        "Invalid type for 'volume'"
                    });
                }
                candle.volume = values[5].is_double()
                                ? values[5].as_double()
                                : static_cast<double>(values[5].as_int64());

                if (!values[6].is_string()) {
                    return std::unexpected(Exchange::FetchError{
                        Exchange::FetchStatus::ParseError,
                        "Invalid type for 'begin'"
                    });
                }
                candle.startPoint = parseDateTime(values[6].as_string()).value();

                if (!values[7].is_string()) {
                    return std::unexpected(Exchange::FetchError{
                        Exchange::FetchStatus::ParseError,
                        "Invalid type for 'end'"
                    });
                }
                candle.endPoint = parseDateTime(values[7].as_string()).value();

                candlesVector.push_back(candle);

            } catch (const std::exception& e) {
                return std::unexpected(Exchange::FetchError{
                    Exchange::FetchStatus::ParseError,
                    e.what()
                });
            }
        }

        Logger::log<Logger::LogLevel::Debug>(
            "Парсинг данных выполнился успешно."
        );

        return candlesVector;
    }

protected:
    [[nodiscard]] std::expected<Market::TimePoint, std::string>
    parseDateTime(std::string_view s) const {
        std::tm tm{};
        std::istringstream ss{std::string{s}};

        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

        if (ss.fail()) {
            return std::unexpected(std::format("Failed to parse datetime: '{}'", s));
        }

        const std::time_t time = std::mktime(&tm);

        if (time == -1) {
            return std::unexpected(std::format("Invalid date/time: '{}'", s));
        }

        return std::chrono::system_clock::from_time_t(time);
    }

protected:
    int _moexCandleColumnCount = 8;
};
