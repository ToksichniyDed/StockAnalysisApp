//
// Created by DED on 22.12.2025.
//
module;

#include <toml++/toml.h>
#include <expected>
#include <filesystem>
#include <concepts>

export module ConfigurationParser;

import Logger;

namespace {
    template <typename T>
    concept configValue = std::is_arithmetic_v<T> || std::same_as<T, std::string> || std::same_as<T, bool>;

    enum class ConfigError {
        KeyNotFound,
        InvalidType,
        FileNotFound,
        ParseError
    };
}

export class ConfigurationParser {
public:
    ConfigurationParser() = default;

    explicit ConfigurationParser(const std::filesystem::path& configPath) {
        if (!std::filesystem::exists(configPath)) {
            _lastError = ConfigError::FileNotFound;
            Logger::log<Logger::LogLevel::Error>("Конфиг-файл не найден: {}", configPath.string());
            return;
        }

        try {
            _config = toml::parse_file(configPath.string());
            Logger::log<Logger::LogLevel::Info>("Конфиг-файл успешно загружен из: {}", configPath.string());
        } catch (const toml::parse_error& parseError) {
            _lastError = ConfigError::ParseError;
            Logger::log<Logger::LogLevel::Error>("Ошибка парсинга TOML: {}", parseError.what());
        }
    }

    template <configValue T>
    [[nodiscard]] std::expected<T, ConfigError> getValue(const std::string& key) const {
        if (!_config) {
            auto err = _lastError.value_or(ConfigError::FileNotFound);
            Logger::log<Logger::LogLevel::Error>("Попытка чтения без конфига (ошибка: {})", static_cast<int>(err));
            return std::unexpected(err);
        }

        auto node = _config->at_path(key);
        if (!node) {
            Logger::log<Logger::LogLevel::Warn>("Ключ не найден: {}", key);
            return std::unexpected(ConfigError::KeyNotFound);
        }

        auto val = node.value<T>();
        if (!val) {
            Logger::log<Logger::LogLevel::Warn>("Неверный тип для ключа {} (ожидается: {})", key, typeid(T).name());
            return std::unexpected(ConfigError::InvalidType);
        }

        Logger::log<Logger::LogLevel::Trace>("Прочитано значение для {}: {}", key, *val);
        return *val;
    }

    template <configValue T>
    [[nodiscard]] T getValue(const std::string& key, const T& defaultValue) const {
        auto result = getValue<T>(key);
        if (!result) {
            Logger::log<Logger::LogLevel::Info>("Использовано дефолтное значение для {}: {}", key, defaultValue);
        }
        return result.value_or(defaultValue);
    }

private:
    std::optional<toml::table> _config;
    std::optional<ConfigError> _lastError;
};
