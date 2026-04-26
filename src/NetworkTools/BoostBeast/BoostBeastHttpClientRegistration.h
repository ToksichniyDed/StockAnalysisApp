#pragma once

#include <memory>

#include <NetworkTools/HttpClientFactory.h>
#include <NetworkTools/IHttpClient.h>
#include <NetworkTools/BoostBeast/BoostBeastHttpClient.h>

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
