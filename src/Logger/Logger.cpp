//
// Created by DED on 26.04.2026.
//

#include "Logger.h"

std::shared_ptr<spdlog::logger> Logger::logger = nullptr;

void Logger::init(bool enableConsole, bool enableFile, const spdlog::level::level_enum defaultLevel,
                  const std::string& fileName, const std::size_t maxSizeMB, const std::size_t maxFiles) {
    std::vector<spdlog::sink_ptr> loggerSinks;

    if (enableConsole) {
        auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console->set_level(spdlog::level::trace);
        console->set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");
        loggerSinks.push_back(console);
    }

    if (enableFile) {
        namespace fs = std::filesystem;
        fs::create_directories("logs");

        auto file = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            fileName,
            1024 * 1024 * maxSizeMB,
            maxFiles
        );

        file->set_level(spdlog::level::trace);
        file->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] %v");
        loggerSinks.push_back(file);
    }

    if (loggerSinks.empty()) {
        loggerSinks.push_back(std::make_shared<spdlog::sinks::null_sink_st>());
    }

    logger = std::make_shared<spdlog::logger>("CoolNameApp", loggerSinks.begin(),
                                              loggerSinks.end());
    logger->set_level(defaultLevel);
    spdlog::register_logger(logger);

    logger->info("Логирование инициализировано (console: {}, file: {})", enableConsole, enableFile);
}

void Logger::setLoggingLevel(const LogLevel logLevel) {
    if (logger)
        logger->set_level(static_cast<spdlog::level::level_enum>(logLevel));
}

void Logger::disableLogger() {
    if (logger)
        logger->set_level(spdlog::level::off);
}

void Logger::enable(const spdlog::level::level_enum lvl) {
    if (logger)
        logger->set_level(lvl);
}
