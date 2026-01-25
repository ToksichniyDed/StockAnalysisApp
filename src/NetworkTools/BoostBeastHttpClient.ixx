//
// Created by DED on 10.01.2026.
//

module;

#include <memory>
#include <string>
#include <chrono>
#include <expected>
#include <regex>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>

export module BoostBeastHttpClient;

import IHttpClient;
import Logger;

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;;

class UrlParser {
public:
    [[nodiscard]] static std::expected<Http::ParsedUrl, Http::ParseUrlError> parse(const std::string& url) {
        std::regex urlRegex(
            R"(^(https?):\/\/([^:\/\s]+)(?::(\d+))?(\/[^\?]*)?\??(.*)$)",
            std::regex::icase
        );

        std::smatch match;
        if (!std::regex_match(url, match, urlRegex)) {
            return std::unexpected("Invalid URL format: " + url);
        }

        Http::ParsedUrl parsed;
        parsed.scheme = match[1].str();
        parsed.host = match[2].str();

        if (match[3].matched) {
            parsed.port = match[3].str();
        } else {
            parsed.port = (parsed.scheme == "https") ? "443" : "80";
        }

        parsed.path = match[4].matched ? match[4].str() : "/";
        parsed.query = match[5].str();

        return parsed;
    }
};

export class BoostBeastHttpClient : public IHttpClient {
public:
    explicit BoostBeastHttpClient(
        std::chrono::milliseconds timeout = std::chrono::seconds{30},
        const std::string& userAgent = "") : _timeout(timeout) {
    }

    explicit BoostBeastHttpClient(const std::string& userAgent = "") : _timeout(std::chrono::seconds{30}) {
    }

    ~BoostBeastHttpClient() override = default;

    [[nodiscard]] std::expected<Http::Response, Http::Error> get(const std::string& url) override {
        auto startTime = std::chrono::steady_clock::now();

        auto parsedUrl = UrlParser::parse(url);
        if (!parsedUrl.has_value()) {
            return std::unexpected(Http::Error{
                Http::ErrorType::InvalidUrl,
                parsedUrl.error().errorMessage
            });
        }

        if (parsedUrl->scheme != "http") {
            return std::unexpected(Http::Error{
                Http::ErrorType::InvalidUrl,
                "HttpClient supports only http://, got: " + parsedUrl->scheme
            });
        }

        Logger::log<Logger::LogLevel::Info>(
            "GET запрос: {}://{}{}",
            parsedUrl->scheme, parsedUrl->host, parsedUrl->fullPath()
        );

        auto result = performHttpRequest(parsedUrl.value());

        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - startTime
        );

        if (!result.has_value()) {
            auto error = result.error();
            error.elapsed = elapsed;
            error.url = url;
            return std::unexpected(Http::Error{
                Http::ErrorType::Unknown,
                error.format()
            });
        }

        Logger::log<Logger::LogLevel::Info>(
            "GET запрос выполнен: {} байтов в {}мс",
            result.value().body.size(), elapsed.count()
        );

        return result;
    }

private:
    [[nodiscard]] std::expected<Http::Response, Http::Error>
    performHttpRequest(const Http::ParsedUrl& url) const {
        try {
            net::io_context ioContext;
            tcp::resolver resolver(ioContext);
            beast::tcp_stream stream(ioContext);

            stream.expires_after(_timeout);

            auto const results = resolver.resolve(url.host, url.port);

            stream.connect(results);

            http::request<http::empty_body> req{http::verb::get, url.fullPath(), 11};
            req.set(http::field::host, url.host);
            req.set(http::field::user_agent, "BoostBeastHttpClient");

            Http::ResponseHeaders headers;
            for (const auto& [key, value] : headers) {
                req.set(key, value);
            }

            http::write(stream, req);

            beast::flat_buffer buffer;
            http::response<http::string_body> res;
            http::read(stream, buffer, res);

            if (res.result() != http::status::ok) {
                return std::unexpected(Http::Error{
                    Http::ErrorType::HttpError,
                    std::string(res.reason()),
                    static_cast<int>(res.result())
                });
            }

            beast::error_code ec;
            stream.socket().shutdown(tcp::socket::shutdown_both, ec);

            if (ec) {
                Logger::log<Logger::LogLevel::Debug>("Ошибка graceful shutdown сокета: {}", ec.message());
            }

            Http::Response response {ec.value(), std::move(res.body()), std::move(headers)};

            return {response};
        } catch (const std::exception& e) {
            return std::unexpected(Http::Error{
                Http::ErrorType::Unknown,
                std::string("HTTP request failed: ") + e.what()
            });
        }
    }

private:
    std::chrono::milliseconds _timeout;
};
