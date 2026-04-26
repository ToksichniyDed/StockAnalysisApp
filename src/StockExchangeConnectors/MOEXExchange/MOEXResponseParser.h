//
// Created by DED on 25.04.2026.
//

#ifndef COOLAPPNAME_MOEXRESPONSEPARSER_H
#define COOLAPPNAME_MOEXRESPONSEPARSER_H

#include <expected>
#include <string_view>
#include <string>
#include <vector>
#include <chrono>

#include <boost/json.hpp>
#include <boost/system/error_code.hpp>

#include <Market/Market.h>
#include <StockExchangeConnectors/IDataFetcher.h>

class MOEXResponseParser {
public:
    MOEXResponseParser() = default;
    ~MOEXResponseParser() = default;

    [[nodiscard]] std::expected<std::vector<Market::Candle>, Exchange::FetchError>
    parse(const std::string_view& jsonBody) const;
protected:
    [[nodiscard]] std::expected<Market::TimePoint, std::string>
    parseDateTime(std::string_view s) const;
protected:
    int _moexCandleColumnCount = 8;
};

#endif //COOLAPPNAME_MOEXRESPONSEPARSER_H