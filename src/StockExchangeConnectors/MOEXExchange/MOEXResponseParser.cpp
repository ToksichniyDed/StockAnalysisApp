//
// Created by DED on 25.04.2026.
//

#include "MOEXResponseParser.h"

#include <Logger/Logger.h>

std::expected<std::vector<Market::Candle>, Exchange::FetchError> MOEXResponseParser::parse(
    const std::string_view& jsonBody) const {
    boost::system::error_code errorCode;
    boost::json::value jsonValue = boost::json::parse(jsonBody, errorCode);

    if (errorCode) {
        Logger::log<Logger::LogLevel::Error>("Ошибка парсинга JSON: {}", errorCode.message());
        return std::unexpected(Exchange::FetchError{
            Exchange::FetchStatus::ParseError,
            std::string("MOEX JSON request parse error: ") + errorCode.message()
        });
    }

    if (!jsonValue.is_object()) {
        Logger::log<Logger::LogLevel::Error>("Ответ не является JSON объектом, тип: {}", to_string(jsonValue.kind()));
        return std::unexpected(Exchange::FetchError{
            Exchange::FetchStatus::ParseError,
            "Response is not a JSON object"
        });
    }

    const boost::json::object& response = jsonValue.as_object();

    if (!response.contains("candles") || !response.at("candles").is_object()) {
        Logger::log<Logger::LogLevel::Error>("'candles' не является объектом");
        return std::unexpected(Exchange::FetchError{
            Exchange::FetchStatus::ParseError,
            "'candles' is not an object"
        });
    }

    const auto& candles = response.at("candles").as_object();

    if (!candles.contains("data") || !candles.at("data").is_array()) {
            Logger::log<Logger::LogLevel::Error>("'data' не является массивом");
            return std::unexpected(Exchange::FetchError{
                Exchange::FetchStatus::ParseError,
                "'data' is not an array"
            });
        }

    const auto& data = candles.at("data").as_array();
    Logger::log<Logger::LogLevel::Debug>("Получено {} строк данных", data.size());

    if (data.empty()) {
        Logger::log<Logger::LogLevel::Warn>("Данные отсутствуют в ответе");
        return std::unexpected(Exchange::FetchError{
            Exchange::FetchStatus::NoDataFound,
            std::string("No data found: ")
        });
    }

    std::vector<Market::Candle> candlesVector;
    candlesVector.reserve(data.size());

    for (size_t rowIndex = 0; rowIndex < data.size(); ++rowIndex) {
        const auto& values = data[rowIndex].as_array();

        if (values.size() != _moexCandleColumnCount) {
            Logger::log<Logger::LogLevel::Error>(
                "Строка {}: неверное количество колонок — ожидалось {}, получено {}",
                rowIndex, _moexCandleColumnCount, values.size()
            );
            return std::unexpected(Exchange::FetchError{
                Exchange::FetchStatus::InvalidParameter,
                std::string("Candles count mismatch: ") + std::to_string(values.size())
            });
        }

        try {
            Market::Candle candle;

            if (!values[0].is_double() && !values[0].is_int64()) {
                Logger::log<Logger::LogLevel::Error>("Строка {}: неверный тип 'open'", rowIndex);
                return std::unexpected(Exchange::FetchError{
                    Exchange::FetchStatus::ParseError,
                    "Invalid type for 'open'"
                });
            }
            candle.openPrice = values[0].is_double()
                               ? values[0].as_double()
                               : static_cast<double>(values[0].as_int64());

            if (!values[1].is_double() && !values[1].is_int64()) {
                Logger::log<Logger::LogLevel::Error>("Строка {}: неверный тип 'close'", rowIndex);
                return std::unexpected(Exchange::FetchError{
                    Exchange::FetchStatus::ParseError,
                    "Invalid type for 'close'"
                });
            }
            candle.closePrice = values[1].is_double()
                                ? values[1].as_double()
                                : static_cast<double>(values[1].as_int64());

            if (!values[2].is_double() && !values[2].is_int64()) {
                Logger::log<Logger::LogLevel::Error>("Строка {}: неверный тип 'high'", rowIndex);
                return std::unexpected(Exchange::FetchError{
                    Exchange::FetchStatus::ParseError,
                    "Invalid type for 'high'"
                });
            }
            candle.highPrice = values[2].is_double()
                               ? values[2].as_double()
                               : static_cast<double>(values[2].as_int64());

            if (!values[3].is_double() && !values[3].is_int64()) {
                Logger::log<Logger::LogLevel::Error>("Строка {}: неверный тип 'low'", rowIndex);
                return std::unexpected(Exchange::FetchError{
                    Exchange::FetchStatus::ParseError,
                    "Invalid type for 'low'"
                });
            }
            candle.lowPrice = values[3].is_double()
                              ? values[3].as_double()
                              : static_cast<double>(values[3].as_int64());

            if (!values[4].is_double() && !values[4].is_int64()) {
                Logger::log<Logger::LogLevel::Error>("Строка {}: неверный тип 'value'", rowIndex);
                return std::unexpected(Exchange::FetchError{
                    Exchange::FetchStatus::ParseError,
                    "Invalid type for 'value'"
                });
            }
            candle.value = values[4].is_double()
                           ? values[4].as_double()
                           : static_cast<double>(values[4].as_int64());

            if (!values[5].is_double() && !values[5].is_int64()) {
                Logger::log<Logger::LogLevel::Error>("Строка {}: неверный тип 'volume'", rowIndex);
                return std::unexpected(Exchange::FetchError{
                    Exchange::FetchStatus::ParseError,
                    "Invalid type for 'volume'"
                });
            }
            candle.volume = values[5].is_double()
                            ? values[5].as_double()
                            : static_cast<double>(values[5].as_int64());

            if (!values[6].is_string()) {
                Logger::log<Logger::LogLevel::Error>("Строка {}: неверный тип 'begin'", rowIndex);
                return std::unexpected(Exchange::FetchError{
                    Exchange::FetchStatus::ParseError,
                    "Invalid type for 'begin'"
                });
            }
            candle.startPoint = parseDateTime(values[6].as_string()).value();

            if (!values[7].is_string()) {
                Logger::log<Logger::LogLevel::Error>("Строка {}: неверный тип 'end'", rowIndex);
                return std::unexpected(Exchange::FetchError{
                    Exchange::FetchStatus::ParseError,
                    "Invalid type for 'end'"
                });
            }
            candle.endPoint = parseDateTime(values[7].as_string()).value();

            candlesVector.push_back(candle);

        } catch (const std::exception& e) {
            Logger::log<Logger::LogLevel::Error>("Строка {}: исключение при парсинге — {}", rowIndex, e.what());
            return std::unexpected(Exchange::FetchError{
                Exchange::FetchStatus::ParseError,
                e.what()
            });
        }
    }

    Logger::log<Logger::LogLevel::Debug>("Парсинг завершён успешно: {} свечей", candlesVector.size());

    return candlesVector;
}

std::expected<Market::TimePoint, std::string> MOEXResponseParser::parseDateTime(std::string_view s) const {
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

