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
import HttpClientFactory;
import Logger;
import MOEXResponseParser;
import Market;

class MOEXDataFetcher : public Exchange::IDataFetcher {
public:
    explicit MOEXDataFetcher(const std::filesystem::path& filePath = {},
                             std::shared_ptr<IHttpClient> httpClient = nullptr,
                             std::shared_ptr<ConfigurationParser> configurationParser = nullptr) : IDataFetcher(
            std::make_shared<ConfigurationParser>(filePath)),
        _httpClient(HttpClientFactory::instance().create("default")),
        _moexResponseParser(std::make_shared<MOEXResponseParser>()) {
    }

    ~MOEXDataFetcher() override = default;

protected:
    [[nodiscard]] std::expected<std::vector<Market::Candle>, Exchange::FetchError>
    fetchCandles(
        const Market::Ticker& ticker, const Market::TimePoint& from,
        const Market::TimePoint& till,
        const Market::Timeframe& timeframe = Market::Timeframe::Day) const override {
        auto hostOpt = _configurationParser->getValue<std::string>("network.host");
        if (!hostOpt.has_value() || hostOpt->empty()) {
            return std::unexpected(Exchange::FetchError{
                Exchange::FetchStatus::InvalidParameter,
                "Host not configured or empty"
            });
        }

        auto serviceOpt = _configurationParser->getValue<std::string>("network.service");
        if (!serviceOpt.has_value() || serviceOpt->empty()) {
            return std::unexpected(Exchange::FetchError{
                Exchange::FetchStatus::InvalidParameter,
                "Service not configured or empty"
            });
        }

        auto candlesOpt = _configurationParser->getValue<std::string>("requests.candles");
        if (!candlesOpt.has_value() || candlesOpt->empty()) {
            return std::unexpected(Exchange::FetchError{
                Exchange::FetchStatus::InvalidParameter,
                "Candles path not configured or empty"
            });
        }

        std::string target = *candlesOpt;
        size_t pos = target.find("{ticker}");
        if (pos != std::string::npos) {
            target.replace(pos, 8, ticker.name());
        } else {
            return std::unexpected(Exchange::FetchError{
                Exchange::FetchStatus::InvalidParameter,
                "Candles template missing {ticker} placeholder"
            });
        }

        target += std::format("?from={:%Y-%m-%d}&till={:%Y-%m-%d}&interval={}", from, till,
                              static_cast<int>(timeframe));

        std::vector<Market::Candle> allCandles;
        int start = 0;

        do {
            constexpr int pageSize = 1000;
            std::string url = std::format("https://{}{}&start={}",
                                          *hostOpt, target, start);

            std::expected<Http::Response, Http::Error> httpResult = _httpClient->get(url);

            if (!httpResult.has_value()) {
                return std::unexpected(Exchange::FetchError{
                    Exchange::FetchStatus::HttpError,
                    std::format("HTTP error: {}", httpResult.error().message),
                    httpResult.error().statusCode.value()
                });
            }

            const auto& parsedResponse = _moexResponseParser->parse(httpResult.value().body);
            if (!parsedResponse.has_value()) {
                return std::unexpected(Exchange::FetchError{
                    Exchange::FetchStatus::ParseError,
                    std::format("Parsing error: {}", parsedResponse.error().errorMessage),
                    parsedResponse.error().httpStatusCode.value()
                });
            }

            if (parsedResponse.value().empty()) {
                if (start == 0) {
                    return std::unexpected(Exchange::FetchError{
                        Exchange::FetchStatus::NoDataFound,
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
    std::shared_ptr<MOEXResponseParser> _moexResponseParser;
};
