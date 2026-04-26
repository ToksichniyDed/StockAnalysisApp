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
