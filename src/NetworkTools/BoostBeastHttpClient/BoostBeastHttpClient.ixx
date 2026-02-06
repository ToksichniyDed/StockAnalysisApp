//
// Created by DED on 10.01.2026.
//

module;

#include <memory>
#include <string>
#include <chrono>
#include <expected>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>

export module BoostBeastHttpClient;

import IHttpClient;
import Logger;
import UrlParser;
import HttpClientFactory;

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

export class BoostBeastHttpClient : public IHttpClient {
public:
    explicit BoostBeastHttpClient(
        const std::chrono::milliseconds timeout = std::chrono::seconds{30},
        const std::string& userAgent = "") : _timeout(timeout),
                                             _userAgent(userAgent.empty() ? "BoostBeastHttpClient" : userAgent) {
    }

    explicit BoostBeastHttpClient(const std::string& userAgent = "") : _timeout(std::chrono::seconds{30}),
                                                                       _userAgent(userAgent.empty()
                                                                           ? "BoostBeastHttpClient"
                                                                           : userAgent) {
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

        Logger::log<Logger::LogLevel::Debug>(
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
            return std::unexpected(std::move(error));
        }

        result->elapsed = elapsed;

        Logger::log<Logger::LogLevel::Info>(
            "GET запрос выполнен: {} байтов в {}мс",
            result.value().body.size(), elapsed.count()
        );

        return result;
    }

private:
    // Структура для хранения состояния запроса внутри async-цепочки
    struct RequestSession {
        net::io_context ioc;
        tcp::resolver resolver;
        beast::tcp_stream stream;
        http::request<http::empty_body> request;
        http::response<http::string_body> response;
        beast::flat_buffer buffer;
        beast::error_code finalEc;

        RequestSession() : resolver(ioc), stream(ioc) {
        }
    };

    [[nodiscard]] std::expected<Http::Response, Http::Error>
    performHttpRequest(const Http::ParsedUrl& url) const {
        auto session = std::make_shared<RequestSession>();

        beast::get_lowest_layer(session->stream).expires_after(_timeout);

        // Запускаем async-цепочку
        resolveAsync(session, url);

        // Единственный run() — крутит event loop до завершения всей цепочки
        session->ioc.run();

        // Обрабатываем результат после run()
        if (session->finalEc) {
            // Проверяем, это timeout или другая ошибка
            if (session->finalEc == beast::error::timeout) {
                return std::unexpected(Http::Error{
                    Http::ErrorType::Timeout,
                    std::format("Request timed out after {}ms", _timeout.count())
                });
            }

            return std::unexpected(Http::Error{
                Http::ErrorType::NetworkError,
                std::format("Request failed: {}", session->finalEc.message())
            });
        }

        {
            beast::error_code ec;
            session->stream.socket().shutdown(tcp::socket::shutdown_both, ec);
            if (ec) {
                Logger::log<Logger::LogLevel::Debug>("Graceful shutdown warning: {}", ec.message());
            }
        }

        Http::ResponseHeaders headers;
        for (const auto& field : session->response) {
            headers[std::string(field.name_string())] = std::string(field.value());
        }

        const int statusCode = static_cast<int>(session->response.result());

        if (statusCode < 200 || statusCode >= 300) {
            return std::unexpected(Http::Error{
                Http::ErrorType::HttpError,
                std::string(session->response.reason()),
                statusCode
            });
        }

        return Http::Response{statusCode, std::move(session->response.body()), std::move(headers)};
    }

    // Async-цепочка: resolve → connect → write → read
    // Каждый шаг запускает следующий через completion handler

    void resolveAsync(const std::shared_ptr<RequestSession>& session, const Http::ParsedUrl& url) const {
        session->resolver.async_resolve(url.host, url.port,
                                        [this, session, &url](const beast::error_code& ec,
                                                              const tcp::resolver::results_type& results) {
                                            if (ec) {
                                                session->finalEc = ec;
                                                return;
                                            }

                                            connectAsync(session, url, results);
                                        }
        );
    }

    void connectAsync(const std::shared_ptr<RequestSession>& session, const Http::ParsedUrl& url,
                      const tcp::resolver::results_type& results) const {
        session->stream.async_connect(results,
                                      [this, session, &url](const beast::error_code& ec, const tcp::endpoint&) {
                                          if (ec) {
                                              session->finalEc = ec;
                                              return;
                                          }

                                          writeAsync(session, url);
                                      }
        );
    }

    void writeAsync(const std::shared_ptr<RequestSession>& session, const Http::ParsedUrl& url) const {
        session->request.method(http::verb::get);
        session->request.target(url.fullPath());
        session->request.version(11);
        session->request.set(http::field::host, url.host);
        session->request.set(http::field::user_agent, _userAgent);

        http::async_write(session->stream, session->request,
                          [this, session](const beast::error_code& ec, size_t) {
                              if (ec) {
                                  session->finalEc = ec;
                                  return;
                              }

                              readAsync(session);
                          }
        );
    }

    void readAsync(const std::shared_ptr<RequestSession>& session) const {
        http::async_read(session->stream, session->buffer, session->response,
                         [session](const beast::error_code& ec, size_t) {
                             if (ec) {
                                 session->finalEc = ec;
                             }
                         }
        );
    }

private:
    std::chrono::milliseconds _timeout;
    std::string _userAgent;
};
