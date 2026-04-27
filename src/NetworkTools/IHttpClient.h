//
// Created by DED on 26.04.2026.
//

#ifndef COOLAPPNAME_IHTTPCLIENT_H
#define COOLAPPNAME_IHTTPCLIENT_H


#include <expected>
#include <string>
#include <optional>
#include <chrono>
#include <unordered_map>

#include <Logger/Logger.h>

namespace Http {
    enum class ErrorType {
        NetworkError, // Нет соединения, DNS не резолвится, timeout
        SslError, // Проблемы с сертификатом, handshake
        HttpError, // HTTP статус != 2xx (400, 404, 500 ...)
        Timeout, // Превышен таймаут запроса
        TooManyRedirects, // Слишком много редиректов
        InvalidUrl, // Некорректный URL
        Unknown // Неизвестная ошибка
    };

    struct Error {
        ErrorType type; // Тип ошибки
        std::string message; // Описание ошибки
        std::optional<int> statusCode;
        std::optional<std::string> url;
        std::optional<std::chrono::milliseconds> elapsed; // Время выполнения до ошибки

        explicit Error(const ErrorType t, std::string msg)
            : type(t), message(std::move(msg)) {
        }

        Error(const ErrorType t, std::string msg, int statusCode)
            : type(t), message(std::move(msg)), statusCode(statusCode) {
        }

        Error(const ErrorType t, std::string msg, int statusCode, std::string errorUrl)
            : type(t), message(std::move(msg)), statusCode(statusCode), url(std::move(errorUrl)) {
        }

        [[nodiscard]] std::string format() const {
            std::string result = "[HttpError] " + message;

            if (statusCode.has_value()) {
                result += " (HTTP " + std::to_string(statusCode.value()) + ")";
            }

            if (url.has_value()) {
                result += " at " + url.value();
            }

            if (elapsed.has_value()) {
                result += " [" + std::to_string(elapsed.value().count()) + "ms]";
            }

            return result;
        }
    };

    using ResponseHeaders = std::unordered_map<std::string, std::string>;

    struct Response {
        int statusCode;
        std::string body;
        ResponseHeaders headers;
        std::chrono::milliseconds elapsed;

        Response(int code, std::string responseBody, ResponseHeaders responseHeaders = {},
                     std::chrono::milliseconds time = std::chrono::milliseconds{0}) : statusCode(code),
            body(std::move(responseBody)),
            headers(std::move(responseHeaders)), elapsed(time) {
        }
    };

    struct ParsedUrl {
        std::string scheme;
        std::string host;
        std::string port;
        std::string path;
        std::string query;

        [[nodiscard]] std::string fullPath() const {
            return query.empty() ? path : path + "?" + query;
        }
    };

    static const std::unordered_map<std::string, std::string> DEFAULT_PORTS = {
        {"http",  "80"},
        {"https", "443"},
        {"ftp",   "21"},
        {"ssh",   "22"}
    };

    struct ParseUrlError {
        std::string errorMessage;

        ParseUrlError(std::string msg) : errorMessage(std::move(msg)) {
            Logger::log<Logger::LogLevel::Error>("Ошибка парсинга URL : {}", errorMessage);
        }
    };
};


class IHttpClient {
    public:
    virtual ~IHttpClient() = default;
    virtual [[nodiscard]] std::expected<Http::Response, Http::Error> get (const std::string& url) = 0;
    virtual [[nodiscard]] std::expected<Http::Response, Http::Error> getDetailed (const std::string& url) {
        auto result = get(url);
        if (!result.has_value()) {
            return std::unexpected(result.error());
        }
        return Http::Response{200, std::move(result.value().body)};
    };
};

#endif //COOLAPPNAME_IHTTPCLIENT_H