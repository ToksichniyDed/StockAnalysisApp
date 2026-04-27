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
        [[nodiscard]] std::expected<T, Parser::ExchangeConfigurationError> getValue(const std::string& key) const {
            if (!_config.has_value()) {
                auto err = _lastError.value_or(Parser::ExchangeConfigurationError::FileNotFound);
                Logger::log<Logger::LogLevel::Error>("Попытка чтения без конфига (ошибка: {})", static_cast<int>(err));
                return std::unexpected(err);
            }

            auto node = _config->at_path(key);
            if (!node) {
                Logger::log<Logger::LogLevel::Warn>("Ключ не найден: {}", key);
                return std::unexpected(Parser::ExchangeConfigurationError::KeyNotFound);
            }

            auto val = node.value<T>();
            if (!val) {
                Logger::log<Logger::LogLevel::Warn>("Неверный тип для ключа {} (ожидается: {})", key, typeid(T).name());
                return std::unexpected(Parser::ExchangeConfigurationError::InvalidType);
            }

            Logger::log<Logger::LogLevel::Trace>("Прочитано значение для {}: {}", key, *val);
            return *val;
        }

        template <Parser::configValue T>
        [[nodiscard]] T getValue(const std::string& key, const T& defaultValue) const {
            auto result = getValue<T>(key);
            if (!result) {
                Logger::log<Logger::LogLevel::Info>("Использовано дефолтное значение для {}: {}", key, defaultValue);
            }
            return result.value_or(defaultValue);
        }

    private:
        std::optional<toml::table> _config;
        std::optional<Parser::ExchangeConfigurationError> _lastError;
    };
}

#endif //COOLAPPNAME_CONFIGURATIONPARSER_H
