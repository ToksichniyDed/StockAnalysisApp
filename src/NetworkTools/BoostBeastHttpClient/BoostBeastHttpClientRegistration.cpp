//
// BoostBeastHttpClient_Registration.cpp
// Регистрирует BoostBeastHttpClient в фабрике
// ЭТОТ файл - единственный, который импортирует модуль с Boost
//

#include <memory>
#include <boost/asio/error.hpp>

import HttpClientFactory;
import IHttpClient;
import BoostBeastHttpClient;

namespace {
    HttpClientRegistrar boostBeastRegistrar(
        "BoostBeastHttpClient",
        [](const HttpClientConfig& config) -> std::unique_ptr<IHttpClient> {
            return std::make_unique<BoostBeastHttpClient>(
                config.timeout,
                config.userAgent
            );
        }
    );

    HttpClientRegistrar boostBeastDefaultAlias(
        "default",
        [](const HttpClientConfig& config) -> std::unique_ptr<IHttpClient> {
            return std::make_unique<BoostBeastHttpClient>(
                config.timeout,
                config.userAgent
            );
        }
    );
}

void registerBoostBeastHttpClient() {
    auto& factory = HttpClientFactory::instance();

    factory.registerClient(
        "BoostBeastHttpClient",
        [](const HttpClientConfig& config) {
            return std::make_unique<BoostBeastHttpClient>(
                config.timeout, config.userAgent
            );
        }
    );
}
