//
// Created by DED on 25.04.2026.
//

#ifndef COOLAPPNAME_CONFIGURATIONPARSER_H
#define COOLAPPNAME_CONFIGURATIONPARSER_H

#include <toml++/toml.h>
#include <expected>
#include <filesystem>
#include <concepts>

#include <Logger/Logger.h>

namespace Parser {
    template <typename T>
    concept configValue = requires(T value) {
        requires std::is_arithmetic_v<T> ||
                 std::same_as<T, std::string> ||
                 std::same_as<T, std::string_view> ||
                 std::same_as<T, bool>;
    };

    enum class ExchangeConfigurationError {
        KeyNotFound,
        InvalidType,
        FileNotFound,
        ParseError
    };

    class ConfigurationParser {
    public:
        ConfigurationParser() = default;

        explicit ConfigurationParser(const std::filesystem::path& configPath);
        template <Parser::configValue T>
        [[nodiscard]] std::expected<T, Parser::ExchangeConfigurationError> getValue(const std::string& key) const;
        template <Parser::configValue T>
        [[nodiscard]] T getValue(const std::string& key, const T& defaultValue) const;

    private:
        std::optional<toml::table> _config;
        std::optional<Parser::ExchangeConfigurationError> _lastError;
    };
}

#endif //COOLAPPNAME_CONFIGURATIONPARSER_H