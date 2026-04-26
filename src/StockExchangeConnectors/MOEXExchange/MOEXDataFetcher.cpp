//
// Created by DED on 25.04.2026.
//

#include "MOEXDataFetcher.h"

#include <NetworkTools/HttpClientFactory.h>


MOEXDataFetcher::MOEXDataFetcher(const std::filesystem::path& filePath,
                                 std::shared_ptr<IHttpClient> httpClient,
                                 std::shared_ptr<MOEXResponseParser> responseParser,
                                 const std::shared_ptr<Parser::ConfigurationParser>&
                                 configurationParser) : IDataFetcher(
                                                            std::make_shared<Parser::ConfigurationParser>(
                                                                filePath.empty() ? MOEX_CONFIG_PATH : filePath)),
                                                        _httpClient(
                                                            httpClient.get()
                                                            ? httpClient
                                                            : HttpClientFactory::instance().create("default")),
                                                        _moexResponseParser(
                                                            responseParser.get()
                                                            ? responseParser
                                                            : std::make_shared<MOEXResponseParser>()),
                                                        _configurationParser(configurationParser.get()
                                                                             ? configurationParser
                                                                             : std::make_shared<
                                                                                 Parser::ConfigurationParser>()) {
}

std::expected<std::vector<Market::Candle>, Exchange::FetchError> MOEXDataFetcher::fetchCandles(
    const Market::Ticker& ticker, const Market::TimePoint& from, const Market::TimePoint& till,
    const Market::Timeframe& timeframe) const {
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
}
