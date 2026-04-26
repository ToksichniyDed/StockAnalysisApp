//
// Created by DED on 26.04.2026.
//

#ifndef COOLAPPNAME_HTTPCLIENTFACTORY_H
#define COOLAPPNAME_HTTPCLIENTFACTORY_H


#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <stdexcept>

#include "IHttpClient.h"

struct HttpClientConfig {
    std::chrono::milliseconds timeout = std::chrono::seconds{30};
    std::string userAgent = "HttpClient";
};

class HttpClientFactory {
public:
    using FactoryFunction = std::function<std::unique_ptr<IHttpClient>(const HttpClientConfig&)>;

    static HttpClientFactory& instance();

    void registerClient(std::string_view name, FactoryFunction factory);

    //лучше перед использовать isRegistered()
    [[nodiscard]] std::unique_ptr<IHttpClient> create(
        std::string_view name,
        const HttpClientConfig& config = {}
    ) const;

    [[nodiscard]] std::unique_ptr<IHttpClient> create(
        std::string_view name,
        std::chrono::milliseconds timeout,
        const std::string& userAgent = ""
    ) const;

    [[nodiscard]] std::vector<std::string> getRegisteredClients() const;

    [[nodiscard]] bool isRegistered(std::string_view name) const;

private:
    HttpClientFactory() = default;

    std::unordered_map<std::string, FactoryFunction> _registry;

    HttpClientFactory(const HttpClientFactory&) = delete;
    HttpClientFactory& operator=(const HttpClientFactory&) = delete;
};

class HttpClientRegistrar {
public:
    HttpClientRegistrar(
        std::string_view name,
        HttpClientFactory::FactoryFunction factory
    ) {
        HttpClientFactory::instance().registerClient(name, std::move(factory));
    }
};

// Использование:
// REGISTER_HTTP_CLIENT("boost-beast", [](const auto& cfg) {
//     return std::make_unique<BoostBeastHttpClient>(cfg.timeout, cfg.userAgent);
// });
#define REGISTER_HTTP_CLIENT(name, factoryFunc) \
        namespace { \
            static ::HttpClientRegistrar _registrar_##name(#name, factoryFunc); \
        }

#endif //COOLAPPNAME_HTTPCLIENTFACTORY_H
