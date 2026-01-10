//
// Created by DED on 27.12.2025.
//

module;

#include <memory>
#include <filesystem>
#include <utility>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/null_sink.h>

#if defined(_MSC_VER)
#define FUNC_SIG __FUNCSIG__
#elif defined(__GNUC__) || defined(__clang__)
#define FUNC_SIG __PRETTY_FUNCTION__
#else
#define FUNC_SIG __func__
#endif

export module Logger;

namespace Logger {
    std::shared_ptr<spdlog::logger> logger;

    export enum class LogLevel : std::underlying_type_t<spdlog::level::level_enum> {
        Trace = static_cast<std::underlying_type_t<spdlog::level::level_enum>>(spdlog::level::trace),
        Debug = static_cast<std::underlying_type_t<spdlog::level::level_enum>>(spdlog::level::debug),
        Info = static_cast<std::underlying_type_t<spdlog::level::level_enum>>(spdlog::level::info),
        Warn = static_cast<std::underlying_type_t<spdlog::level::level_enum>>(spdlog::level::warn),
        Error = static_cast<std::underlying_type_t<spdlog::level::level_enum>>(spdlog::level::err),
        Critical = static_cast<std::underlying_type_t<spdlog::level::level_enum>>(spdlog::level::critical),
        Off = static_cast<std::underlying_type_t<spdlog::level::level_enum>>(spdlog::level::off)
    };

    export void init(bool enableConsole, bool enableFile, const spdlog::level::level_enum defaultLevel,
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

    export void setLoggingLevel(const LogLevel logLevel) {
        if (logger)
            logger->set_level(static_cast<spdlog::level::level_enum>(logLevel));
    }

    export void disableLogger() {
        if (logger)
            logger->set_level(spdlog::level::off);
    }

    export void enable(const spdlog::level::level_enum lvl = spdlog::level::info) {
        if (logger)
            logger->set_level(lvl);
    }

    export template <LogLevel Level, typename... Args>
    void log(fmt::format_string<Args...> msg, Args&&... args) {
        if (logger) {
            logger->log(static_cast<spdlog::level::level_enum>(Level), "[{}] {}", FUNC_SIG,
                        fmt::format(msg, std::forward<Args>(args)...));
        }
    }

    export namespace TestingLogger {
        void setLogger(std::shared_ptr<spdlog::logger> customLogger) {
            logger = std::move(customLogger);
        }

        std::shared_ptr<spdlog::logger> getLogger() {
            return logger;
        }

        void resetLogger() {
            logger = nullptr;
        }
    }
}
