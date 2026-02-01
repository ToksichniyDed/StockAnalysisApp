//
// Created by DED on 25.01.2026.
//

#include <gtest/gtest.h>
#include <filesystem>
#include <expected>

import ConfigurationParser;

class ConfigurationParserTest : public ::testing::Test {
protected:
    std::filesystem::path _fixturesDir;

    void SetUp() override {
        _fixturesDir = std::filesystem::path(__FILE__).parent_path() / "tomls_test";

        ASSERT_TRUE(std::filesystem::exists(_fixturesDir))
            << "Fixtures directory not found: " << _fixturesDir;
    }

    std::filesystem::path getFixture(const std::string& filename) {
        return _fixturesDir / filename;
    }
};

TEST_F(ConfigurationParserTest, ParsesSimpleConfig) {
    ConfigurationParser parser(getFixture("valid_simple.toml"));

    EXPECT_EQ(parser.getValue<std::string>("host").value(), "iss.moex.com");
    EXPECT_EQ(parser.getValue<int>("port").value(), 443);
    EXPECT_EQ(parser.getValue<int>("timeout").value(), 30);
    EXPECT_TRUE(parser.getValue<bool>("enabled").value());
    EXPECT_DOUBLE_EQ(parser.getValue<double>("rate_limit").value(), 10.5);
}

TEST_F(ConfigurationParserTest, ParsesNestedStructures) {
    ConfigurationParser parser(getFixture("valid_nested.toml"));

    EXPECT_EQ(parser.getValue<std::string>("network.host").value(), "example.com");
    EXPECT_EQ(parser.getValue<int>("network.port").value(), 8080);
    EXPECT_TRUE(parser.getValue<bool>("network.ssl.enabled").value());
    EXPECT_EQ(parser.getValue<std::string>("network.ssl.cert_path").value(), "/path/to/cert");

    EXPECT_EQ(parser.getValue<std::string>("database.connection.host").value(), "localhost");
    EXPECT_EQ(parser.getValue<int>("database.connection.port").value(), 5432);
    EXPECT_EQ(parser.getValue<int>("database.pool.max_size").value(), 10);
    EXPECT_DOUBLE_EQ(parser.getValue<double>("database.pool.timeout").value(), 30.5);
}

TEST_F(ConfigurationParserTest, ParsesAllDataTypes) {
    ConfigurationParser parser(getFixture("valid_types.toml"));

    // Strings
    EXPECT_EQ(parser.getValue<std::string>("string_value").value(), "test string");
    EXPECT_EQ(parser.getValue<std::string>("empty_string").value(), "");
    EXPECT_EQ(parser.getValue<std::string>("url_string").value(),
              "https://example.com/api?param=value");

    // Integers
    EXPECT_EQ(parser.getValue<int>("int_value").value(), 42);
    EXPECT_EQ(parser.getValue<int>("negative_int").value(), -100);
    EXPECT_EQ(parser.getValue<int>("zero").value(), 0);
    EXPECT_EQ(parser.getValue<int>("large_int").value(), 1000000);

    // Floats
    EXPECT_NEAR(parser.getValue<double>("float_value").value(), 3.14159, 1e-5);
    EXPECT_NEAR(parser.getValue<double>("negative_float").value(), -123.456, 1e-3);
    EXPECT_NEAR(parser.getValue<double>("scientific").value(), 1.23e-4, 1e-10);
    EXPECT_DOUBLE_EQ(parser.getValue<double>("zero_float").value(), 0.0);

    // Booleans
    EXPECT_TRUE(parser.getValue<bool>("bool_true").value());
    EXPECT_FALSE(parser.getValue<bool>("bool_false").value());
}

TEST_F(ConfigurationParserTest, ParsesMoexConfigCorrectly) {
    ConfigurationParser parser(getFixture("valid_moex.toml"));

    EXPECT_EQ(parser.getValue<std::string>("network.host").value(), "iss.moex.com");
    EXPECT_EQ(parser.getValue<std::string>("network.service").value(), "443");
    EXPECT_EQ(parser.getValue<int>("network.timeout").value(), 30);

    auto candlesPath = parser.getValue<std::string>("requests.candles").value();
    EXPECT_TRUE(candlesPath.find("{ticker}") != std::string::npos);

    EXPECT_EQ(parser.getValue<int>("api.rate_limit").value(), 10);
    EXPECT_EQ(parser.getValue<int>("api.retry_count").value(), 3);
    EXPECT_DOUBLE_EQ(parser.getValue<double>("api.retry_delay").value(), 1.5);

    EXPECT_FALSE(parser.getValue<bool>("cache.enabled").value());
    EXPECT_EQ(parser.getValue<int>("cache.ttl").value(), 3600);
}

TEST_F(ConfigurationParserTest, RejectsNonExistentFile) {
    ConfigurationParser parser(getFixture("does_not_exist.toml"));

    auto result = parser.getValue<std::string>("any.key");
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Parser::ExchangeConfigurationError::FileNotFound);
}

TEST_F(ConfigurationParserTest, RejectsInvalidSyntax) {
    ConfigurationParser parser(getFixture("invalid_syntax.toml"));

    auto result = parser.getValue<std::string>("key");
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Parser::ExchangeConfigurationError::ParseError);
}

TEST_F(ConfigurationParserTest, ReturnsErrorForMissingKey) {
    ConfigurationParser parser(getFixture("valid_simple.toml"));

    auto result = parser.getValue<std::string>("nonexistent.key");
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Parser::ExchangeConfigurationError::KeyNotFound);
}

TEST_F(ConfigurationParserTest, ReturnsErrorOnTypeMismatch) {
    ConfigurationParser parser(getFixture("valid_types.toml"));

    // Пытаемся прочитать строку как число
    auto result = parser.getValue<int>("string_value");
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Parser::ExchangeConfigurationError::InvalidType);
}

TEST_F(ConfigurationParserTest, HandlesEmptyFile) {
    ConfigurationParser parser(getFixture("empty.toml"));

    auto result = parser.getValue<std::string>("any.key");
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Parser::ExchangeConfigurationError::KeyNotFound);
}

// --- Тесты дефолтных значений ---

TEST_F(ConfigurationParserTest, ReturnsDefaultWhenKeyMissing) {
    ConfigurationParser parser(getFixture("valid_simple.toml"));

    auto value = parser.getValue<int>("missing_key", 999);
    EXPECT_EQ(value, 999);
}

TEST_F(ConfigurationParserTest, ReturnsDefaultWhenFileNotFound) {
    ConfigurationParser parser(getFixture("nonexistent.toml"));

    EXPECT_EQ(parser.getValue<std::string>("key", "default"), "default");
    EXPECT_EQ(parser.getValue<int>("key", 42), 42);
    EXPECT_DOUBLE_EQ(parser.getValue<double>("key", 3.14), 3.14);
    EXPECT_TRUE(parser.getValue<bool>("key", true));
}

TEST_F(ConfigurationParserTest, ReturnsDefaultOnTypeMismatch) {
    ConfigurationParser parser(getFixture("valid_types.toml"));

    auto value = parser.getValue<int>("string_value", 777);
    EXPECT_EQ(value, 777);
}

TEST_F(ConfigurationParserTest, ReturnsActualValueOverDefault) {
    ConfigurationParser parser(getFixture("valid_simple.toml"));

    auto timeout = parser.getValue<int>("timeout", 99);
    EXPECT_EQ(timeout, 30); // Реальное значение, не дефолт
}

// --- Тесты граничных случаев ---

TEST_F(ConfigurationParserTest, AllowsMultipleReads) {
    ConfigurationParser parser(getFixture("valid_simple.toml"));

    auto read1 = parser.getValue<std::string>("host");
    auto read2 = parser.getValue<std::string>("host");
    auto read3 = parser.getValue<std::string>("host");

    EXPECT_EQ(read1.value(), "iss.moex.com");
    EXPECT_EQ(read2.value(), "iss.moex.com");
    EXPECT_EQ(read3.value(), "iss.moex.com");
}

TEST_F(ConfigurationParserTest, DefaultConstructorCreatesEmptyParser) {
    ConfigurationParser parser;

    auto result = parser.getValue<std::string>("any.key");
    EXPECT_FALSE(result.has_value());

    EXPECT_EQ(parser.getValue<std::string>("any.key", "default"), "default");
}
