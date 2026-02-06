module;

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <stdexcept>

export module HttpClientFactory;

import IHttpClient;

export struct HttpClientConfig {
    std::chrono::milliseconds timeout = std::chrono::seconds{30};
    std::string userAgent = "HttpClient";
};

export class HttpClientFactory {
public:
    using FactoryFunction = std::function<std::unique_ptr<IHttpClient>(const HttpClientConfig&)>;

    static HttpClientFactory& instance() {
        static HttpClientFactory factory;
        return factory;
    }

    void registerClient(std::string_view name, FactoryFunction factory) {
        std::string key(name);
        _registry[key] = std::move(factory);
    }

    //лучше перед использовать isRegistered()
    [[nodiscard]] std::unique_ptr<IHttpClient> create(
        std::string_view name,
        const HttpClientConfig& config = {}
    ) const {
        std::string key(name);
        auto it = _registry.find(key);
        
        if (it == _registry.end()) {
            throw std::invalid_argument(
                std::string("Unknown HTTP client: ") + key
            );
        }
        
        return it->second(config);
    }

    [[nodiscard]] std::unique_ptr<IHttpClient> create(
        std::string_view name,
        std::chrono::milliseconds timeout,
        const std::string& userAgent = ""
    ) const {
        HttpClientConfig config;
        config.timeout = timeout;
        if (!userAgent.empty()) {
            config.userAgent = userAgent;
        }
        return create(name, config);
    }

    [[nodiscard]] std::vector<std::string> getRegisteredClients() const {
        std::vector<std::string> names;
        names.reserve(_registry.size());
        
        for (const auto& [name, _] : _registry) {
            names.push_back(name);
        }
        
        return names;
    }

    [[nodiscard]] bool isRegistered(std::string_view name) const {
        return _registry.contains(std::string(name));
    }

private:
    HttpClientFactory() = default;
    
    std::unordered_map<std::string, FactoryFunction> _registry;

    HttpClientFactory(const HttpClientFactory&) = delete;
    HttpClientFactory& operator=(const HttpClientFactory&) = delete;
};

export class HttpClientRegistrar {
public:
    HttpClientRegistrar(
        std::string_view name,
        HttpClientFactory::FactoryFunction factory
    ) {
        HttpClientFactory::instance().registerClient(name, std::move(factory));
    }
};

export {
    // Использование:
    // REGISTER_HTTP_CLIENT("boost-beast", [](const auto& cfg) {
    //     return std::make_unique<BoostBeastHttpClient>(cfg.timeout, cfg.userAgent);
    // });
    #define REGISTER_HTTP_CLIENT(name, factoryFunc) \
        namespace { \
            static ::HttpClientRegistrar _registrar_##name(#name, factoryFunc); \
        }
}
