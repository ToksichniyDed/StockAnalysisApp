//
// Created by DED on 25.01.2026.

#include <gtest/gtest.h>

#include <NetworkTools/UrlParser.h>

class UrlParserTest : public ::testing::Test {
};

TEST_F(UrlParserTest, ParsesSimpleHttpUrl) {
    auto result = UrlParser::parse("http://example.com");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->scheme, "http");
    EXPECT_EQ(result->host, "example.com");
    EXPECT_EQ(result->path, "/");
    EXPECT_EQ(result->query, "");
}

TEST_F(UrlParserTest, ParsesUrlWithCustomPort) {
    auto result = UrlParser::parse("http://localhost:8080");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->host, "localhost");
    EXPECT_EQ(result->port, "8080");
}

TEST_F(UrlParserTest, ParsesUrlWithPath) {
    auto result = UrlParser::parse("http://api.example.com/v1/data");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->path, "/v1/data");
}

TEST_F(UrlParserTest, ParsesUrlWithQueryString) {
    auto result = UrlParser::parse("http://example.com/search?q=test&limit=10");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->path, "/search");
    EXPECT_EQ(result->query, "q=test&limit=10");
}

TEST_F(UrlParserTest, ParsesComplexMoexUrl) {
    auto result = UrlParser::parse(
        "https://iss.moex.com:443/iss/engines/stock/markets/shares/boards/TQBR/"
        "securities/SBER/candles.json?from=2024-01-01&till=2024-12-31&interval=24"
    );

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->scheme, "https");
    EXPECT_EQ(result->host, "iss.moex.com");
    EXPECT_EQ(result->port, "443");
    EXPECT_EQ(result->path, "/iss/engines/stock/markets/shares/boards/TQBR/"
              "securities/SBER/candles.json");
    EXPECT_EQ(result->query, "from=2024-01-01&till=2024-12-31&interval=24");
}

TEST_F(UrlParserTest, FullPathCombinesPathAndQuery) {
    auto result = UrlParser::parse("http://example.com/api?key=value");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->fullPath(), "/api?key=value");
}

TEST_F(UrlParserTest, FullPathWithoutQuery) {
    auto result = UrlParser::parse("http://example.com/api");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->fullPath(), "/api");
}

TEST_F(UrlParserTest, ParsesIpAddress) {
    auto result = UrlParser::parse("http://192.168.1.1:8080/api");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->host, "192.168.1.1");
    EXPECT_EQ(result->port, "8080");
    EXPECT_EQ(result->path, "/api");
}

TEST_F(UrlParserTest, ParsesLocalhostWithPort) {
    auto result = UrlParser::parse("http://localhost:3000/test");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->host, "localhost");
    EXPECT_EQ(result->port, "3000");
}

TEST_F(UrlParserTest, ParsesUrlWithEncodedCharacters) {
    auto result = UrlParser::parse("http://example.com/search?q=%D1%82%D0%B5%D1%81%D1%82");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->query, "q=%D1%82%D0%B5%D1%81%D1%82");
}

TEST_F(UrlParserTest, ParsesUrlWithMultipleQueryParams) {
    auto result = UrlParser::parse(
        "http://api.example.com/endpoint?param1=value1&param2=value2&param3=value3"
    );

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result->query.find("param1=value1") != std::string::npos);
    EXPECT_TRUE(result->query.find("param2=value2") != std::string::npos);
    EXPECT_TRUE(result->query.find("param3=value3") != std::string::npos);
}

TEST_F(UrlParserTest, RejectsUrlWithoutScheme) {
    auto result = UrlParser::parse("example.com/path");

    ASSERT_FALSE(result.has_value());
    EXPECT_TRUE(result.error().errorMessage.find("Invalid URL format") != std::string::npos);
}

TEST_F(UrlParserTest, RejectsEmptyUrl) {
    auto result = UrlParser::parse("");

    ASSERT_FALSE(result.has_value());
}

TEST_F(UrlParserTest, RejectsUrlWithSpaces) {
    auto result = UrlParser::parse("http://example.com/path with spaces");

    ASSERT_FALSE(result.has_value());
}

TEST_F(UrlParserTest, RejectsMalformedUrl) {
    auto result = UrlParser::parse("http://");

    ASSERT_FALSE(result.has_value());
}

TEST_F(UrlParserTest, RejectsInvalidScheme) {
    auto result = UrlParser::parse("ftp://example.com");

    ASSERT_FALSE(result.has_value());
}

TEST_F(UrlParserTest, RejectsUrlWithoutHost) {
    auto result = UrlParser::parse("http:///path");

    ASSERT_FALSE(result.has_value());
}

TEST_F(UrlParserTest, ParsesUrlWithTrailingSlash) {
    auto result = UrlParser::parse("http://example.com/");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->path, "/");
}

TEST_F(UrlParserTest, ParsesUrlWithMultipleSlashesInPath) {
    auto result = UrlParser::parse("http://example.com///path//to///resource");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->path, "///path//to///resource");
}

TEST_F(UrlParserTest, ParsesUrlWithEmptyQuery) {
    auto result = UrlParser::parse("http://example.com/path?");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->path, "/path");
    EXPECT_EQ(result->query, "");
}

TEST_F(UrlParserTest, ParsesUrlWithDotInHost) {
    auto result = UrlParser::parse("http://sub.domain.example.com/path");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->host, "sub.domain.example.com");
}

TEST_F(UrlParserTest, ParsesUrlWithHyphenInHost) {
    auto result = UrlParser::parse("http://my-subdomain.example.com/path");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->host, "my-subdomain.example.com");
}

TEST_F(UrlParserTest, ParsesUrlWithPortAndNoPath) {
    auto result = UrlParser::parse("http://example.com:8080");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->host, "example.com");
    EXPECT_EQ(result->port, "8080");
    EXPECT_EQ(result->path, "/");
}

TEST_F(UrlParserTest, ParsesUrlWithComplexQueryString) {
    auto result = UrlParser::parse(
        "http://example.com/api?filters[status]=active&sort=-created_at&include=author,comments"
    );

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result->query.find("filters[status]=active") != std::string::npos);
}

TEST_F(UrlParserTest, ParsesUrlWithSpecialCharactersInQuery) {
    auto result = UrlParser::parse("http://example.com/search?q=hello+world&date=2024-01-01");

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result->query.find("hello+world") != std::string::npos);
}
