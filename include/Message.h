#pragma once

#include <string>

// 节点间通信的数据载体：封装内容与自动生成的时间戳，只提供只读接口
class Message {
public:
    explicit Message(const std::string& content);

    const std::string& content() const;
    const std::string& timestamp() const;

private:
    std::string m_content;
    std::string m_timestamp;

    static std::string currentTimestamp();
};
