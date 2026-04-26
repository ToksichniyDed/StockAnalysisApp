//
// Created by DED on 26.01.2026.
//

#include <gtest/gtest.h>
#include <expected>
#include <string>
#include <fstream>
#include <filesystem>
#include <chrono>

#include <StockExchangeConnectors/MOEXExchange/MOEXResponseParser.h>
#include <StockExchangeConnectors/MOEXExchange/MOEXDataFetcher.h>

class MOEXResponseParser_Test : public ::testing::Test {
protected:
    void SetUp() override {
        _fixturesDir = std::filesystem::path(__FILE__).parent_path()/"moex_responses_test";

        ASSERT_TRUE(std::filesystem::exists(_fixturesDir))
            << "Fixtures directory not found: " << _fixturesDir;
    }

    std::string loadFixture(const std::string& filename) {
        auto path = _fixturesDir / filename;
        std::ifstream file(path);

        if (!file.is_open()) {
            throw std::runtime_error("Cannot open json: " + path.string());
        }

        return std::string(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        );
    }

protected:
    MOEXResponseParser _parser;
    std::filesystem::path _fixturesDir;
};

TEST_F(MOEXResponseParser_Test, ParsesValidSingleCandle) {
    std::string json = loadFixture("valid_single_candle.json");
    auto result = _parser.parse(json);

    ASSERT_TRUE(result.has_value())<<result.error().errorMessage;
    ASSERT_EQ(result->size(), 1);

    const auto& candle = result->at(0);
    EXPECT_DOUBLE_EQ(candle.openPrice, 250.5);
    EXPECT_DOUBLE_EQ(candle.closePrice, 251.0);
    EXPECT_DOUBLE_EQ(candle.highPrice, 252.0);
    EXPECT_DOUBLE_EQ(candle.lowPrice, 249.5);
    EXPECT_TRUE(candle.value.has_value());
    EXPECT_DOUBLE_EQ(candle.value.value(), 1500000.0);
    EXPECT_DOUBLE_EQ(candle.volume, 6000.0);
}

TEST_F(MOEXResponseParser_Test, ParsesMultipleCandles) {
    std::string json = loadFixture("valid_multiple_candles.json");
    auto result = _parser.parse(json);

    ASSERT_TRUE(result.has_value())<<result.error().errorMessage;;
    EXPECT_EQ(result->size(), 3);

    EXPECT_DOUBLE_EQ(result->at(0).openPrice, 100.0);
    EXPECT_DOUBLE_EQ(result->at(1).openPrice, 101.0);
    EXPECT_DOUBLE_EQ(result->at(2).openPrice, 100.5);

    EXPECT_DOUBLE_EQ(result->at(0).closePrice, 101.0);
    EXPECT_DOUBLE_EQ(result->at(1).closePrice, 100.5);
    EXPECT_DOUBLE_EQ(result->at(2).closePrice, 102.0);
}

TEST_F(MOEXResponseParser_Test, ParsesDateTimeCorrectly) {
    std::string json = loadFixture("valid_single_candle.json");
    auto result = _parser.parse(json);

    ASSERT_TRUE(result.has_value())<<result.error().errorMessage;;

    const auto& candle = result->at(0);
    std::time_t startTime = std::chrono::system_clock::to_time_t(candle.startPoint);
    std::tm* tm = std::localtime(&startTime);

    EXPECT_EQ(tm->tm_year + 1900, 2024);
    EXPECT_EQ(tm->tm_mon + 1, 1);  // Январь
    EXPECT_EQ(tm->tm_mday, 15);
    EXPECT_EQ(tm->tm_hour, 10);
    EXPECT_EQ(tm->tm_min, 0);
    EXPECT_EQ(tm->tm_sec, 0);
}

TEST_F(MOEXResponseParser_Test, ParsesEndPointCorrectly) {
    std::string json = loadFixture("valid_single_candle.json");
    auto result = _parser.parse(json);

    ASSERT_TRUE(result.has_value())<<result.error().errorMessage;;

    const auto& candle = result->at(0);
    ASSERT_TRUE(candle.endPoint.has_value());

    std::time_t endTime = std::chrono::system_clock::to_time_t(candle.endPoint.value());
    std::tm* tm = std::localtime(&endTime);

    EXPECT_EQ(tm->tm_hour, 23);
    EXPECT_EQ(tm->tm_min, 59);
    EXPECT_EQ(tm->tm_sec, 59);
}

TEST_F(MOEXResponseParser_Test, HandlesNegativePrices) {
    std::string json = loadFixture("negative_prices.json");
    auto result = _parser.parse(json);

    ASSERT_TRUE(result.has_value())<<result.error().errorMessage;;
    EXPECT_DOUBLE_EQ(result->at(0).openPrice, -100.0);
    EXPECT_DOUBLE_EQ(result->at(0).closePrice, -99.0);
}

TEST_F(MOEXResponseParser_Test, RejectsNonObjectJson) {
    std::string json = loadFixture("non_object.json");
    auto result = _parser.parse(json);

    ASSERT_FALSE(result.has_value())<<result.error().errorMessage;;
    EXPECT_EQ(result.error().status, Exchange::FetchStatus::ParseError);
}

TEST_F(MOEXResponseParser_Test, RejectsMissingCandlesField) {
    std::string json = loadFixture("missing_candles_field.json");
    auto result = _parser.parse(json);

    ASSERT_FALSE(result.has_value())<<result.error().errorMessage;;
    EXPECT_EQ(result.error().status, Exchange::FetchStatus::ParseError);
}

TEST_F(MOEXResponseParser_Test, RejectsEmptyData) {
    std::string json = loadFixture("empty_data.json");
    auto result = _parser.parse(json);

    ASSERT_FALSE(result.has_value())<<result.error().errorMessage;;
    EXPECT_EQ(result.error().status, Exchange::FetchStatus::NoDataFound);
}

TEST_F(MOEXResponseParser_Test, RejectsInvalidColumnCount) {
    std::string json = loadFixture("invalid_column_count.json");
    auto result = _parser.parse(json);

    ASSERT_FALSE(result.has_value())<<result.error().errorMessage;;
    EXPECT_EQ(result.error().status, Exchange::FetchStatus::InvalidParameter);
    EXPECT_TRUE(result.error().errorMessage.find("Column count mismatch") != std::string::npos ||
                result.error().errorMessage.find("mismatch") != std::string::npos);
}

TEST_F(MOEXResponseParser_Test, RejectsInvalidDataTypes) {
    std::string json = loadFixture("invalid_data_types.json");
    auto result = _parser.parse(json);

    ASSERT_FALSE(result.has_value())<<result.error().errorMessage;;
    EXPECT_EQ(result.error().status, Exchange::FetchStatus::ParseError);
}

TEST_F(MOEXResponseParser_Test, RejectsInvalidDateTime) {
    std::string json = loadFixture("invalid_datetime.json");
    auto result = _parser.parse(json);

    ASSERT_FALSE(result.has_value())<<result.error().errorMessage;;
    EXPECT_EQ(result.error().status, Exchange::FetchStatus::ParseError);
}

TEST_F(MOEXResponseParser_Test, ParsesInlineJson) {
    std::string json = R"({
        "candles": {"columns": [], "data": []},
        "data": [
            [100.0, 101.0, 102.0, 99.0, 500000.0, 5000.0, "2024-01-01 10:00:00", "2024-01-01 18:50:00"]
        ]
    })";

    auto result = _parser.parse(json);
    ASSERT_TRUE(result.has_value())<<result.error().errorMessage;;
    EXPECT_EQ(result->size(), 1);
}

TEST_F(MOEXResponseParser_Test, PreservesDataOrder) {
    std::string json = loadFixture("valid_multiple_candles.json");
    auto result = _parser.parse(json);

    ASSERT_TRUE(result.has_value())<<result.error().errorMessage;;
    ASSERT_EQ(result->size(), 3);

    // Проверяем, что порядок сохранен
    EXPECT_LT(result->at(0).openPrice, result->at(1).openPrice);
    EXPECT_GT(result->at(1).openPrice, result->at(2).openPrice);
}

TEST_F(MOEXResponseParser_Test, ReservesCapacityCorrectly) {
    // Создаем JSON с большим количеством свечей
    std::string json = R"({
        "candles": {"columns": [], "data": []},
        "data": [)";

    for (int i = 0; i < 100; ++i) {
        json += std::format(
            "[{}.0, {}.0, {}.0, {}.0, 500000.0, 5000.0, \"2024-01-01 10:00:00\", \"2024-01-01 18:50:00\"]",
            i, i + 1, i + 2, i - 1
        );
        if (i < 99) json += ",";
    }
    json += "]}";

    auto result = _parser.parse(json);
    ASSERT_TRUE(result.has_value())<<result.error().errorMessage;;
    EXPECT_EQ(result->size(), 100);
}