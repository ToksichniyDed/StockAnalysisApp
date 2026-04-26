//
// Created by DED on 09.01.2026.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <spdlog/sinks/base_sink.h>
#include <spdlog/details/fmt_helper.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>
#include <thread>
#include <chrono>

#include <Logger/Logger.h>

// Кастомный sink для захвата логов в память
class MemorySink : public spdlog::sinks::base_sink<std::mutex> {
public:
    std::vector<std::string> messages;

protected:
    void sink_it_(const spdlog::details::log_msg& msg) override {
        spdlog::memory_buf_t formatted;
        formatter_->format(msg, formatted);
        messages.emplace_back(fmt::to_string(formatted));
    }

    void flush_() override {
    }
};

class SourceLocationCaptureSink : public spdlog::sinks::base_sink<std::mutex> {
public:
    struct CapturedEntry {
        std::string filename;
        int line{};
        std::string funcname;
        std::string message;
    };

    std::vector<CapturedEntry> entries;

protected:
    void sink_it_(const spdlog::details::log_msg& msg) override {
        CapturedEntry entry;
        entry.filename = msg.source.filename ? msg.source.filename : "";
        entry.line = msg.source.line;
        entry.funcname = msg.source.funcname ? msg.source.funcname : "";

        spdlog::memory_buf_t buf;
        formatter_->format(msg, buf);
        entry.message = fmt::to_string(buf);

        entries.push_back(std::move(entry));
    }

    void flush_() override {
    }
};

// Fixture для тестов Logger
class LoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Сбрасываем глобальный logger через тестовый интерфейс
        spdlog::drop_all();
        Logger::TestingLogger::resetLogger();

        // Создаем новый MemorySink для каждого теста
        memorySink = std::make_shared<MemorySink>();
    }

    void TearDown() override {
        // Чистим зарегистрированные логгеры
        spdlog::drop_all();
        Logger::TestingLogger::resetLogger();
    }

    // Вспомогательный метод для инициализации logger с MemorySink
    void initLoggerWithMemorySink(spdlog::level::level_enum level = spdlog::level::level_enum::trace) const {
        std::vector<spdlog::sink_ptr> sinks{memorySink};
        auto testLogger = std::make_shared<spdlog::logger>("TestLogger", sinks.begin(), sinks.end());
        testLogger->set_level(level);
        testLogger->set_pattern("[%l] %v"); // Упрощённый паттерн

        Logger::TestingLogger::setLogger(testLogger);
        spdlog::register_logger(testLogger);
    }

    std::shared_ptr<MemorySink> memorySink;
};

TEST_F(LoggerTest, InitWithMemorySink) {
    initLoggerWithMemorySink();

    // Логируем тестовое сообщение
    Logger::log<Logger::LogLevel::Info>("Test message {}", 42);

    // Проверяем, что сообщение попало в MemorySink
    ASSERT_EQ(memorySink->messages.size(), 1);
    EXPECT_THAT(memorySink->messages[0], testing::HasSubstr("[info]"));
    EXPECT_THAT(memorySink->messages[0], testing::HasSubstr("Test message 42"));
}

TEST_F(LoggerTest, LevelFiltering) {
    // Создаем logger с уровнем Info (Debug не должен проходить)
    initLoggerWithMemorySink(spdlog::level::info);

    // Логируем на разных уровнях
    Logger::log<Logger::LogLevel::Debug>("Debug - не должно попасть");
    Logger::log<Logger::LogLevel::Info>("Info - должно попасть");
    Logger::log<Logger::LogLevel::Warn>("Warn - должно попасть");

    // Проверяем фильтрацию
    ASSERT_EQ(memorySink->messages.size(), 2);
    EXPECT_THAT(memorySink->messages[0], testing::HasSubstr("Info - должно попасть"));
    EXPECT_THAT(memorySink->messages[1], testing::HasSubstr("Warn - должно попасть"));
}

TEST_F(LoggerTest, SetLoggingLevel) {
    initLoggerWithMemorySink();

    // Логируем Debug
    Logger::log<Logger::LogLevel::Debug>("Debug 1");
    ASSERT_EQ(memorySink->messages.size(), 1);

    // Меняем уровень на Warn
    Logger::setLoggingLevel(Logger::LogLevel::Warn);

    Logger::log<Logger::LogLevel::Debug>("Debug 2 - не должно попасть");
    Logger::log<Logger::LogLevel::Warn>("Warn 1 - должно попасть");

    // Проверяем
    ASSERT_EQ(memorySink->messages.size(), 2);
    EXPECT_THAT(memorySink->messages[1], testing::HasSubstr("Warn 1 - должно попасть"));
}

TEST_F(LoggerTest, DisableLogger) {
    initLoggerWithMemorySink();

    // Отключаем логирование
    Logger::disableLogger();

    Logger::log<Logger::LogLevel::Error>("Не должно логироваться");

    // Проверяем, что ничего не залогировалось
    ASSERT_TRUE(memorySink->messages.empty());
}

TEST_F(LoggerTest, EnableLogger) {
    // Создаем logger с выключенным уровнем
    initLoggerWithMemorySink(spdlog::level::off);

    Logger::log<Logger::LogLevel::Info>("Не должно попасть");
    ASSERT_TRUE(memorySink->messages.empty());

    // Включаем логирование
    Logger::enable(spdlog::level::info);

    Logger::log<Logger::LogLevel::Info>("Должно попасть");

    ASSERT_EQ(memorySink->messages.size(), 1);
    EXPECT_THAT(memorySink->messages[0], testing::HasSubstr("Должно попасть"));
}

TEST_F(LoggerTest, NoLoggerDoesNotThrow) {
    // Не инициализируем logger (остается nullptr)
    EXPECT_NO_THROW(Logger::log<Logger::LogLevel::Error>("Silent log"));
    EXPECT_NO_THROW(Logger::setLoggingLevel(Logger::LogLevel::Debug));
    EXPECT_NO_THROW(Logger::disableLogger());
    EXPECT_NO_THROW(Logger::enable());
}

TEST_F(LoggerTest, MultipleLogLevels) {
    initLoggerWithMemorySink();

    // Логируем на всех уровнях
    Logger::log<Logger::LogLevel::Trace>("Trace message");
    Logger::log<Logger::LogLevel::Debug>("Debug message");
    Logger::log<Logger::LogLevel::Info>("Info message");
    Logger::log<Logger::LogLevel::Warn>("Warn message");
    Logger::log<Logger::LogLevel::Error>("Error message");
    Logger::log<Logger::LogLevel::Critical>("Critical message");

    ASSERT_EQ(memorySink->messages.size(), 6);
    EXPECT_THAT(memorySink->messages[0], testing::HasSubstr("[trace]"));
    EXPECT_THAT(memorySink->messages[1], testing::HasSubstr("[debug]"));
    EXPECT_THAT(memorySink->messages[2], testing::HasSubstr("[info]"));
    EXPECT_THAT(memorySink->messages[3], testing::HasSubstr("[warning]"));
    EXPECT_THAT(memorySink->messages[4], testing::HasSubstr("[error]"));
    EXPECT_THAT(memorySink->messages[5], testing::HasSubstr("[critical]"));
}

TEST_F(LoggerTest, GetLoggerReturnsCurrentInstance) {
    ASSERT_EQ(Logger::TestingLogger::getLogger(), nullptr);

    initLoggerWithMemorySink();

    auto currentLogger = Logger::TestingLogger::getLogger();
    ASSERT_NE(currentLogger, nullptr);
    EXPECT_EQ(currentLogger->name(), "TestLogger");
}

TEST_F(LoggerTest, ResetLoggerClearsInstance) {
    initLoggerWithMemorySink();
    ASSERT_NE(Logger::TestingLogger::getLogger(), nullptr);

    Logger::TestingLogger::resetLogger();

    ASSERT_EQ(Logger::TestingLogger::getLogger(), nullptr);
}

TEST(LoggerFileTest, InitWithFileRotation) {
    namespace fs = std::filesystem;

    // Создаем временную директорию
    fs::path tempDir = fs::temp_directory_path() / "logger_test_logs";
    fs::create_directories(tempDir);
    std::string fileName = (tempDir / "test_rotation.log").string();

    // Инициализируем Logger с файловым логированием
    Logger::init(false, true, spdlog::level::trace, fileName, 1, 3);

    // Логируем большой объем данных для ротации
    std::string bigMsg(1000, 'x');
    for (int i = 0; i < 2100; ++i) {
        Logger::log<Logger::LogLevel::Info>("Big message {} {}", i, bigMsg);
    }

    // Принудительный flush
    if (auto currentLogger = Logger::TestingLogger::getLogger()) {
        currentLogger->flush();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Проверяем, что файлы созданы
    std::vector<fs::path> logFiles;
    for (const auto& entry : fs::directory_iterator(tempDir)) {
        if (entry.path().extension() == ".log" ||
            entry.path().string().find(".log.") != std::string::npos) {
            logFiles.push_back(entry.path());
        }
    }

    EXPECT_GE(logFiles.size(), 2);

    if (fs::exists(fileName)) {
        EXPECT_LE(fs::file_size(fileName), 1024 * 1024 * 1 + 10240);
    }

    // Cleanup
    Logger::disableLogger();
    spdlog::drop_all();
    Logger::TestingLogger::resetLogger();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    try {
        fs::remove_all(tempDir);
    } catch (const std::exception& e) {
        std::cerr << "Cleanup warning: " << e.what() << std::endl;
    }
}

TEST(LoggerFileTest, InitWithConsoleAndFile) {
    namespace fs = std::filesystem;

    fs::path tempDir = fs::temp_directory_path() / "logger_test_console";
    fs::create_directories(tempDir);
    std::string fileName = (tempDir / "test_console.log").string();

    // Инициализируем с консолью и файлом
    Logger::init(true, true, spdlog::level::info, fileName, 1, 2);

    Logger::log<Logger::LogLevel::Info>("Test console and file");
    Logger::log<Logger::LogLevel::Warn>("Warning message");

    if (auto currentLogger = Logger::TestingLogger::getLogger()) {
        currentLogger->flush();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    ASSERT_TRUE(fs::exists(fileName));

    std::ifstream logFile(fileName);
    std::string content((std::istreambuf_iterator<char>(logFile)),
                        std::istreambuf_iterator<char>());

    EXPECT_THAT(content, testing::HasSubstr("Test console and file"));
    EXPECT_THAT(content, testing::HasSubstr("Warning message"));

    // Cleanup
    Logger::disableLogger();
    spdlog::drop_all();
    Logger::TestingLogger::resetLogger();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    try {
        fs::remove_all(tempDir);
    } catch (...) {
        // Игнорируем ошибки
    }
}

TEST(LoggerFileTest, InitOnlyConsole) {
    namespace fs = std::filesystem;

    // Инициализируем только консоль (без файла)
    Logger::init(true, false, spdlog::level::debug, "", 1, 1);

    // Просто проверяем, что не падает
    EXPECT_NO_THROW(Logger::log<Logger::LogLevel::Debug>("Console only message"));
    EXPECT_NO_THROW(Logger::log<Logger::LogLevel::Info>("Another console message"));

    auto currentLogger = Logger::TestingLogger::getLogger();
    ASSERT_NE(currentLogger, nullptr);
    EXPECT_EQ(currentLogger->name(), "CoolNameApp");

    // Cleanup
    spdlog::drop_all();
    Logger::TestingLogger::resetLogger();
}

TEST(LoggerFileTest, InitWithNeitherConsoleNorFile) {
    // Инициализируем без консоли и без файла (должен создаться null_sink)
    Logger::init(false, false, spdlog::level::info, "", 1, 1);

    // Проверяем, что logger создан
    auto currentLogger = Logger::TestingLogger::getLogger();
    ASSERT_NE(currentLogger, nullptr);

    // Логируем (должно работать, но ничего не выводиться)
    EXPECT_NO_THROW(Logger::log<Logger::LogLevel::Info>("Null sink message"));

    // Cleanup
    spdlog::drop_all();
    Logger::TestingLogger::resetLogger();
}

TEST_F(LoggerTest, SourceLocationPointsToCallSite) {
    // Arrange
    auto locationSink = std::make_shared<SourceLocationCaptureSink>();
    locationSink->set_level(spdlog::level::trace);

    auto testLogger = std::make_shared<spdlog::logger>(
        "LocationTestLogger",
        spdlog::sinks_init_list{ locationSink }
    );
    testLogger->set_level(spdlog::level::trace);
    Logger::TestingLogger::setLogger(testLogger);

    Logger::log<Logger::LogLevel::Info>("source location test");

    // Assert
    ASSERT_EQ(locationSink->entries.size(), 1);

    const auto& entry = locationSink->entries[0];

    EXPECT_THAT(entry.filename, testing::HasSubstr("Logger_Test.cpp"));

    EXPECT_THAT(entry.funcname,
                testing::Not(testing::HasSubstr("Logger::log")));

    EXPECT_GT(entry.line, 0);
}
