//
// Created by DED on 26.04.2026.
//

#ifndef COOLAPPNAME_LOGGER_H
#define COOLAPPNAME_LOGGER_H

#include <memory>
#include <filesystem>
#include <utility>
#include <source_location>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/null_sink.h>

namespace Logger {
    extern std::shared_ptr<spdlog::logger> logger;

    enum class LogLevel : std::underlying_type_t<spdlog::level::level_enum> {
        Trace = static_cast<std::underlying_type_t<spdlog::level::level_enum>>(spdlog::level::trace),
        Debug = static_cast<std::underlying_type_t<spdlog::level::level_enum>>(spdlog::level::debug),
        Info = static_cast<std::underlying_type_t<spdlog::level::level_enum>>(spdlog::level::info),
        Warn = static_cast<std::underlying_type_t<spdlog::level::level_enum>>(spdlog::level::warn),
        Error = static_cast<std::underlying_type_t<spdlog::level::level_enum>>(spdlog::level::err),
        Critical = static_cast<std::underlying_type_t<spdlog::level::level_enum>>(spdlog::level::critical),
        Off = static_cast<std::underlying_type_t<spdlog::level::level_enum>>(spdlog::level::off)
    };

    template <LogLevel Level>
    struct LogFmtWithLocation {
        std::string_view _fmt;
        std::source_location _loc;

        template <typename S>
        LogFmtWithLocation(S&& fmt,
                           std::source_location loc = std::source_location::current()) : _fmt(std::forward<S>(fmt)),
            _loc(loc) {
        }
    };

    void init(bool enableConsole, bool enableFile, const spdlog::level::level_enum defaultLevel,
              const std::string& fileName, const std::size_t maxSizeMB, const std::size_t maxFiles);
    void setLoggingLevel(const LogLevel logLevel);
    void disableLogger();
    void enable(const spdlog::level::level_enum lvl = spdlog::level::info);

    template <LogLevel Level, typename... Args>
    void log(LogFmtWithLocation<Level> fmtWithLoc, Args&&... args) {
        if (logger) {
            logger->log(
                spdlog::source_loc{
                    fmtWithLoc._loc.file_name(),
                    static_cast<int>(fmtWithLoc._loc.line()),
                    fmtWithLoc._loc.function_name()
                },
                static_cast<spdlog::level::level_enum>(Level),
                fmt::format(fmt::runtime(fmtWithLoc._fmt),
                            std::forward<Args>(args)...)
            );
        }
    }

    namespace TestingLogger {
        inline void setLogger(std::shared_ptr<spdlog::logger> customLogger) {
            logger = std::move(customLogger);
        }

        inline std::shared_ptr<spdlog::logger> getLogger() {
            return logger;
        }

        inline void resetLogger() {
            logger = nullptr;
        }
    }
}


#endif //COOLAPPNAME_LOGGER_H
