//
// Created by DED on 25.04.2026.
//

#include "ConfigurationParser.h"

Parser::ConfigurationParser::ConfigurationParser(const std::filesystem::path& configPath) {
    if (!std::filesystem::exists(configPath)) {
        _lastError = Parser::ExchangeConfigurationError::FileNotFound;
        Logger::log<Logger::LogLevel::Error>("Конфиг-файл не найден: {}", configPath.string());
        return;
    }

    try {
        _config = toml::parse_file(configPath.string());
        Logger::log<Logger::LogLevel::Info>("Конфиг-файл успешно загружен из: {}", configPath.string());
    } catch (const toml::parse_error& parseError) {
        _lastError = Parser::ExchangeConfigurationError::ParseError;
        Logger::log<Logger::LogLevel::Error>("Ошибка парсинга TOML: {}", parseError.what());
    }
}

template <Parser::configValue T>
std::expected<T, Parser::ExchangeConfigurationError> Parser::ConfigurationParser::
getValue(const std::string& key) const {
    if (!_config) {
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
T Parser::ConfigurationParser::getValue(const std::string& key, const T& defaultValue) const {
    auto result = getValue<T>(key);
    if (!result) {
        Logger::log<Logger::LogLevel::Info>("Использовано дефолтное значение для {}: {}", key, defaultValue);
    }
    return result.value_or(defaultValue);
}
