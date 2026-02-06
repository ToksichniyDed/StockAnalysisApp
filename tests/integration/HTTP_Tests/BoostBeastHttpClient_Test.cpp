#include <gtest/gtest.h>
#include <expected>
#include <string>
#include <chrono>

import IHttpClient;
import HttpClientFactory;

extern void registerBoostBeastHttpClient();

class BoostBeastHttpClientIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Mock-сервер должен быть запущен через docker-compose
        _baseUrl = "http://localhost:8080";
        registerBoostBeastHttpClient();
        _client = HttpClientFactory::instance().create("BoostBeastHttpClient", std::chrono::seconds{10},"BoostBeastHttpClientIntegrationTest");

        // Проверяем, что сервер доступен
        auto healthResult = _client->get(_baseUrl + "/health");
        ASSERT_TRUE(healthResult.has_value()) 
            << "Mock server is not running. Start it with: docker-compose up -d";
    }

    void TearDown() override {
        _client.reset();
    }

protected:
    std::string _baseUrl;
    std::unique_ptr<IHttpClient> _client;
};

// Тесты успешных ответов

TEST_F(BoostBeastHttpClientIntegrationTest, GetRequestReturns200OK) {
    auto result = _client->get(_baseUrl + "/success");

    ASSERT_TRUE(result.has_value()) << result.error().format();
    EXPECT_EQ(result->statusCode, 200);
    EXPECT_FALSE(result->body.empty());
    EXPECT_TRUE(result->body.find("ok") != std::string::npos);
}

TEST_F(BoostBeastHttpClientIntegrationTest, GetRequestReturnsPlainText) {
    auto result = _client->get(_baseUrl + "/text");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->statusCode, 200);
    EXPECT_EQ(result->body, "Plain text response");
}

TEST_F(BoostBeastHttpClientIntegrationTest, GetRequestReturnsEmptyBody) {
    auto result = _client->get(_baseUrl + "/empty");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->statusCode, 200);
    EXPECT_TRUE(result->body.empty());
}

TEST_F(BoostBeastHttpClientIntegrationTest, GetRequestHandlesLargeResponse) {
    auto result = _client->get(_baseUrl + "/large");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->statusCode, 200);
    EXPECT_GT(result->body.size(), 100000); // 100KB+
}

// Тесты HTTP ошибок

TEST_F(BoostBeastHttpClientIntegrationTest, GetRequestHandles404NotFound) {
    auto result = _client->get(_baseUrl + "/notfound");

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().type, Http::ErrorType::HttpError);
    EXPECT_EQ(result.error().statusCode.value(), 404);
}

TEST_F(BoostBeastHttpClientIntegrationTest, GetRequestHandles400BadRequest) {
    auto result = _client->get(_baseUrl + "/badrequest");

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().type, Http::ErrorType::HttpError);
    EXPECT_EQ(result.error().statusCode.value(), 400);
}

TEST_F(BoostBeastHttpClientIntegrationTest, GetRequestHandles401Unauthorized) {
    auto result = _client->get(_baseUrl + "/unauthorized");

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().statusCode.value(), 401);
}

TEST_F(BoostBeastHttpClientIntegrationTest, GetRequestHandles403Forbidden) {
    auto result = _client->get(_baseUrl + "/forbidden");

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().statusCode.value(), 403);
}

TEST_F(BoostBeastHttpClientIntegrationTest, GetRequestHandles500ServerError) {
    auto result = _client->get(_baseUrl + "/servererror");

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().type, Http::ErrorType::HttpError);
    EXPECT_EQ(result.error().statusCode.value(), 500);
}

TEST_F(BoostBeastHttpClientIntegrationTest, GetRequestHandles502BadGateway) {
    auto result = _client->get(_baseUrl + "/badgateway");

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().statusCode.value(), 502);
}

TEST_F(BoostBeastHttpClientIntegrationTest, GetRequestHandles503ServiceUnavailable) {
    auto result = _client->get(_baseUrl + "/unavailable");

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().statusCode.value(), 503);
}

// Тесты timeout

TEST_F(BoostBeastHttpClientIntegrationTest, GetRequestHandlesTimeout) {
    // Создаем клиент с коротким таймаутом
    auto shortTimeoutClient = HttpClientFactory::instance().create("BoostBeastHttpClient", std::chrono::milliseconds{500},"BoostBeastHttpClientIntegrationTest");
    
    auto start = std::chrono::steady_clock::now();
    auto result = shortTimeoutClient->get(_baseUrl + "/timeout");
    auto elapsed = std::chrono::steady_clock::now() - start;

    ASSERT_FALSE(result.has_value());
    
    // Проверяем, что запрос завершился примерно через 500ms
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    EXPECT_LT(elapsedMs, 1000); // Должен завершиться раньше 1 секунды
    EXPECT_GT(elapsedMs, 400);  // Но не слишком быстро
}

TEST_F(BoostBeastHttpClientIntegrationTest, GetRequestCompletesBeforeTimeout) {
    // Медленный ответ (5 сек), но таймаут 10 сек - должно успеть
    auto longTimeoutClient = HttpClientFactory::instance().create("BoostBeastHttpClient", std::chrono::seconds{10},"BoostBeastHttpClientIntegrationTest");
    
    auto result = longTimeoutClient->get(_baseUrl + "/slow");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->statusCode, 200);
    EXPECT_TRUE(result->body.find("Slow response") != std::string::npos);
}

// Тесты query параметров

TEST_F(BoostBeastHttpClientIntegrationTest, GetRequestWithQueryParameters) {
    auto result = _client->get(_baseUrl + "/echo-query?param1=value1&param2=value2");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->statusCode, 200);
    EXPECT_TRUE(result->body.find("param1") != std::string::npos);
    EXPECT_TRUE(result->body.find("value1") != std::string::npos);
    EXPECT_TRUE(result->body.find("param2") != std::string::npos);
}

TEST_F(BoostBeastHttpClientIntegrationTest, GetRequestWithEncodedQuery) {
    auto result = _client->get(_baseUrl + "/echo-query?search=%D1%82%D0%B5%D1%81%D1%82");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->statusCode, 200);
}

// Тесты MOEX API симуляции

TEST_F(BoostBeastHttpClientIntegrationTest, MoexCandlesEndpointReturnsValidJson) {
    auto result = _client->get(_baseUrl + "/moex/candles?ticker=SBER&from=2024-01-01&till=2024-01-31&interval=24");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->statusCode, 200);
    EXPECT_TRUE(result->body.find("candles") != std::string::npos);
    EXPECT_TRUE(result->body.find("data") != std::string::npos);
}

TEST_F(BoostBeastHttpClientIntegrationTest, MoexCandlesWithoutTickerReturns400) {
    auto result = _client->get(_baseUrl + "/moex/candles?from=2024-01-01&till=2024-01-31");

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().statusCode.value(), 400);
}

// Тесты невалидных URL

TEST_F(BoostBeastHttpClientIntegrationTest, InvalidUrlReturnsError) {
    auto result = _client->get("http://non-existent-host-12345.com/path");

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().type, Http::ErrorType::NetworkError);
}

TEST_F(BoostBeastHttpClientIntegrationTest, InvalidPortReturnsError) {
    auto result = _client->get("http://localhost:99999/path");

    ASSERT_FALSE(result.has_value());
}

// Тесты производительности

TEST_F(BoostBeastHttpClientIntegrationTest, MultipleSequentialRequests) {
    constexpr int requestCount = 10;
    
    auto start = std::chrono::steady_clock::now();
    
    for (int i = 0; i < requestCount; ++i) {
        auto result = _client->get(_baseUrl + "/success");
        ASSERT_TRUE(result.has_value()) << "Request " << i << " failed";
    }
    
    auto elapsed = std::chrono::steady_clock::now() - start;
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    
    std::cout << "10 sequential requests took " << elapsedMs << "ms" << std::endl;
    EXPECT_LT(elapsedMs, 5000); // Должно быть быстрее 5 секунд
}

// Граничные случаи

TEST_F(BoostBeastHttpClientIntegrationTest, GetRequestToRootPath) {
    // Сервер должен вернуть 404 для корневого пути (не настроен endpoint)
    auto result = _client->get(_baseUrl + "/");

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().statusCode.value(), 404);
}

TEST_F(BoostBeastHttpClientIntegrationTest, GetRequestWithVeryLongPath) {
    std::string longPath = "/path/" + std::string(1000, 'x');
    auto result = _client->get(_baseUrl + longPath);

    // Должен вернуть 404, но не упасть
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().statusCode.value(), 404);
}