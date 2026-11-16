#pragma once

#include <fstream>
#include <mutex>
#include <string>

// 日志级别：INFO 常规信息 / WARN 警告 / ERROR 错误
enum class LogLevel {
    INFO,
    WARN,
    ERROR
};

// 全局日志器（单例）：同时输出到控制台与可选的文本文件
class Logger {
public:
    static Logger& instance();

    void log(LogLevel level, const std::string& module, const std::string& message);
    void enableFileLog(const std::string& filePath);
    void disableFileLog();
    bool isFileLogEnabled() const;

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger() = default;
    ~Logger() = default;

    static std::string levelToString(LogLevel level);
    static std::string currentTimestamp();

    mutable std::mutex m_mutex;
    std::ofstream m_file;
    bool m_fileEnabled = false;
};

// 便捷宏，调用处写起来更清爽
#define LOG_INFO(module, msg)  Logger::instance().log(LogLevel::INFO,  (module), (msg))
#define LOG_WARN(module, msg)  Logger::instance().log(LogLevel::WARN,  (module), (msg))
#define LOG_ERROR(module, msg) Logger::instance().log(LogLevel::ERROR, (module), (msg))
