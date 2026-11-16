#include "Message.h"
#include "Exception.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

Message::Message(const std::string& content) : m_content(content) {
    if (content.empty()) {
        throw MessageException("Message content must not be empty");
    }
    m_timestamp = currentTimestamp();
}

const std::string& Message::content() const {
    return m_content;
}

const std::string& Message::timestamp() const {
    return m_timestamp;
}

std::string Message::currentTimestamp() {
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
