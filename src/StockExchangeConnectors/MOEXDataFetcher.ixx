//
// Created by DED on 21.12.2025.
//

module;

#include <vector>
#include <expected>
#include <filesystem>

#include <boost/json.hpp>
#include <boost/system/error_code.hpp>

export module MOEXDataFetcher;

import ConfigurationParser;
import IExchangeDataFetcher;
import IHttpClient;
import BoostBeastHttpClient;
import Logger;

class MoexResponseParser {
public:
    MoexResponseParser() = default;
    ~MoexResponseParser() = default;

    [[nodiscard]] std::expected<std::vector<ExchangeDataFetcher::BaseCandle>, ExchangeDataFetcher::FetchError>
    parse(const std::string_view& jsonBody) const {
        boost::system::error_code errorCode;
        boost::json::value jsonValue = boost::json::parse(jsonBody, errorCode);

        if (errorCode) {
            return std::unexpected(ExchangeDataFetcher::FetchError{
                ExchangeDataFetcher::FetchStatus::ParseError,
                std::string("MOEX JSON request parse error: ") + errorCode.message()
            });
        }

        const boost::json::object& response = jsonValue.as_object();
        const auto& candles = response.at("candles").as_array();
        const auto& data = response.at("data").as_array();

        if (data.empty()) {
            return std::unexpected(ExchangeDataFetcher::FetchError{
                ExchangeDataFetcher::FetchStatus::NoDataFound,
                std::string("No data found: ")
            });
        }

        std::vector<ExchangeDataFetcher::BaseCandle> candlesVector;
        candlesVector.reserve(candles.size());

        for (const auto& row : data) {
            const auto& values = row.as_array();

            if (values.size() != _moexCandleColumnCount) {
                return std::unexpected(ExchangeDataFetcher::FetchError{
                    ExchangeDataFetcher::FetchStatus::InvalidParameter,
                    std::string("Candles count mismatch: ") + std::to_string(values.size())
                });
            }

            ExchangeDataFetcher::BaseCandle candle;
            candle.openValue = values.at(0).as_double();
            candle.closeValue = values.at(1).as_double();
            candle.highValue = values.at(2).as_double();
            candle.smallestValue = values.at(3).as_double();
            candle.value = values.at(4).as_double();
            candle.volume = values.at(5).as_double();
            candle.startPoint = parseDateTime(values.at(6).as_string());
            candle.endPoint = parseDateTime(values.at(6).as_string());

            candlesVector.push_back(candle);
        }

        Logger::log<Logger::LogLevel::Info>(
            "Парсинг данных выполнился успешно."
        );

        return candlesVector;
    }

protected:
    [[nodiscard]] ExchangeDataFetcher::TimePoint parseDateTime(std::string_view s) const {
        std::tm tm{};
        std::istringstream ss{std::string{s}};

        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

        if (ss.fail()) {
            throw std::runtime_error("Failed to parse datetime: " + std::string{s});
        }

        const std::time_t time = std::mktime(&tm);

        if (time == -1) {
            throw std::runtime_error("Invalid date/time");
        }

        return std::chrono::system_clock::from_time_t(time);
    }

protected:
    int _moexCandleColumnCount = 8;
};


class MOEXDataFetcher : public IExchangeDataFetcher {
public:
    explicit MOEXDataFetcher(const std::filesystem::path& filePath = {},
                             std::shared_ptr<IHttpClient> httpClient = nullptr,
                             std::shared_ptr<ConfigurationParser> configurationParser = nullptr) : IExchangeDataFetcher(
            std::make_shared<ConfigurationParser>(filePath)),
        _httpClient(std::make_shared<BoostBeastHttpClient>("MOEXDataFetcher")),
        _moexResponseParser(std::make_shared<MoexResponseParser>()) {
    }

    ~MOEXDataFetcher() override = default;

protected:
    [[nodiscard]] std::expected<std::vector<ExchangeDataFetcher::BaseCandle>, ExchangeDataFetcher::FetchError>
    fetchCandles(
        const Ticker& ticker, const ExchangeDataFetcher::TimePoint& from,
        const ExchangeDataFetcher::TimePoint& till,
        const ExchangeDataFetcher::Timeframe& timeframe = ExchangeDataFetcher::Timeframe::Day) const override {
        auto hostOpt = _configurationParser->getValue<std::string>("network.host");
        if (!hostOpt.has_value() || hostOpt->empty()) {
            return std::unexpected(ExchangeDataFetcher::FetchError{
                ExchangeDataFetcher::FetchStatus::InvalidParameter,
                "Host not configured or empty"
            });
        }

        auto serviceOpt = _configurationParser->getValue<std::string>("network.service");
        if (!serviceOpt.has_value() || serviceOpt->empty()) {
            return std::unexpected(ExchangeDataFetcher::FetchError{
                ExchangeDataFetcher::FetchStatus::InvalidParameter,
                "Service not configured or empty"
            });
        }

        auto candlesOpt = _configurationParser->getValue<std::string>("requests.candles");
        if (!candlesOpt.has_value() || candlesOpt->empty()) {
            return std::unexpected(ExchangeDataFetcher::FetchError{
                ExchangeDataFetcher::FetchStatus::InvalidParameter,
                "Candles path not configured or empty"
            });
        }

        std::string target = *candlesOpt;
        size_t pos = target.find("{ticker}");
        if (pos != std::string::npos) {
            target.replace(pos, 8, ticker.name());
        } else {
            return std::unexpected(ExchangeDataFetcher::FetchError{
                ExchangeDataFetcher::FetchStatus::InvalidParameter,
                "Candles template missing {ticker} placeholder"
            });
        }

        target += std::format("?from={:%Y-%m-%d}&till={:%Y-%m-%d}&interval={}", from, till,
                              static_cast<int>(timeframe));

        std::vector<ExchangeDataFetcher::BaseCandle> allCandles;
        int start = 0;

        do {
            constexpr int pageSize = 1000;
            std::string url = std::format("https://{}{}&start={}",
                                          *hostOpt, target, start);

            std::expected<Http::Response, Http::Error> httpResult = _httpClient->get(url);

            if (!httpResult.has_value()) {
                return std::unexpected(ExchangeDataFetcher::FetchError{
                    ExchangeDataFetcher::FetchStatus::HttpError,
                    std::format("HTTP error: {}", httpResult.error().message),
                    httpResult.error().statusCode.value()
                });
            }

            const auto& parsedResponse = _moexResponseParser->parse(httpResult.value().body);
            if (!parsedResponse.has_value()) {
                return std::unexpected(ExchangeDataFetcher::FetchError{
                    ExchangeDataFetcher::FetchStatus::ParseError,
                    std::format("Parsing error: {}", parsedResponse.error().errorMessage),
                    parsedResponse.error().httpStatusCode.value()
                });
            }

            if (parsedResponse.value().empty()) {
                if (start == 0) {
                    return std::unexpected(ExchangeDataFetcher::FetchError{
                        ExchangeDataFetcher::FetchStatus::NoDataFound,
                        std::string("There is no candle data for the specified period")
                    });
                }
                break;
            }

            allCandles.insert(allCandles.end(), std::make_move_iterator(parsedResponse.value().begin()),
                              std::make_move_iterator(parsedResponse.value().end()));

            start += pageSize;
        } while (true);

        Logger::log<Logger::LogLevel::Info>(
            "Запрос данных с MOEX по тикету {} выполнился успешно.", ticker.name()
        );

        return allCandles;
    };

protected:
    std::shared_ptr<IHttpClient> _httpClient;
    std::shared_ptr<MoexResponseParser> _moexResponseParser;
};
