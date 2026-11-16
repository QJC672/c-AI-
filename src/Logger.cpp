#include "Logger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

Logger& Logger::instance() {
    static Logger logger;  // C++11 起局部静态初始化线程安全
    return logger;
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERROR: return "ERROR";
        default:              return "UNKNOWN";
    }
}

std::string Logger::currentTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t seconds = std::chrono::system_clock::to_time_t(now);
    std::tm timeInfo{};
#ifdef _WIN32
    localtime_s(&timeInfo, &seconds);
#else
    localtime_r(&seconds, &timeInfo);
#endif
    std::ostringstream oss;
    oss << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void Logger::log(LogLevel level, const std::string& module, const std::string& message) {
    const std::string line = "[" + currentTimestamp() + "] [" + levelToString(level) +
                             "] [" + module + "] " + message;

    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << line << std::endl;

    if (m_fileEnabled && m_file.is_open()) {
        m_file << line << std::endl;
        m_file.flush();
    }
}

void Logger::enableFileLog(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_file.is_open()) {
        m_file.close();
    }
    m_file.open(filePath, std::ios::out | std::ios::app);
    m_fileEnabled = m_file.is_open();

    if (!m_fileEnabled) {
        std::cerr << "[Logger] WARN: cannot open log file: " << filePath << std::endl;
    }
}

void Logger::disableFileLog() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_file.is_open()) {
        m_file.close();
    }
    m_fileEnabled = false;
}

bool Logger::isFileLogEnabled() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_fileEnabled;
}
