//
// Created by DED on 26.04.2026.
//

#ifndef COOLAPPNAME_BOOSTBEASTHTTPCLIENT_H
#define COOLAPPNAME_BOOSTBEASTHTTPCLIENT_H

#include <memory>
#include <string>
#include <chrono>
#include <expected>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>

#include <NetworkTools/IHttpClient.h>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

class BoostBeastHttpClient : public IHttpClient {
public:
    explicit BoostBeastHttpClient(const std::chrono::milliseconds timeout = std::chrono::seconds{30},
                                  const std::string& userAgent = "");

    explicit BoostBeastHttpClient(const std::string& userAgent = "");

    ~BoostBeastHttpClient() override = default;

    [[nodiscard]] std::expected<Http::Response, Http::Error> get(const std::string& url) override;
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
        Http::ParsedUrl url;
        tcp::resolver::results_type resolveResults;

        RequestSession() : resolver(ioc), stream(ioc) {
        }
    };

    [[nodiscard]] std::expected<Http::Response, Http::Error>
    performHttpRequest(const Http::ParsedUrl& url) const;

    // Async-цепочка: resolve → connect → write → read
    // Каждый шаг запускает следующий через completion handler

    void resolveAsync(const std::shared_ptr<RequestSession>& session) const;

    void connectAsync(const std::shared_ptr<RequestSession>& session) const ;

    void writeAsync(const std::shared_ptr<RequestSession>& session) const ;

    void readAsync(const std::shared_ptr<RequestSession>& session) const ;

private:
    std::chrono::milliseconds _timeout;
    std::string _userAgent;
};


#endif //COOLAPPNAME_BOOSTBEASTHTTPCLIENT_H