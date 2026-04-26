//
// Created by DED on 26.04.2026.
//

#include "HttpClientFactory.h"

HttpClientFactory& HttpClientFactory::instance()  {
    static HttpClientFactory factory;
    return factory;
}

void HttpClientFactory::registerClient(std::string_view name, FactoryFunction factory){
    std::string key(name);
    _registry[key] = std::move(factory);
}

std::unique_ptr<IHttpClient> HttpClientFactory::create(std::string_view name, const HttpClientConfig& config) const {
    std::string key(name);
    auto it = _registry.find(key);

    if (it == _registry.end()) {
        throw std::invalid_argument(
            std::string("Unknown HTTP client: ") + key
        );
    }

    return it->second(config);
}

std::unique_ptr<IHttpClient> HttpClientFactory::create(std::string_view name, std::chrono::milliseconds timeout, const std::string& userAgent) const {
    HttpClientConfig config;
    config.timeout = timeout;
    if (!userAgent.empty()) {
        config.userAgent = userAgent;
    }
    return create(name, config);
}

std::vector<std::string> HttpClientFactory::getRegisteredClients() const {
    std::vector<std::string> names;
    names.reserve(_registry.size());

    for (const auto& [name, _] : _registry) {
        names.push_back(name);
    }

    return names;
}

bool HttpClientFactory::isRegistered(std::string_view name) const {
    return _registry.contains(std::string(name));
}
