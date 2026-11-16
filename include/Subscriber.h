#pragma once

#include "Node.h"
#include <string>

class Message;

// 订阅者节点：从指定话题被动接收消息
class Subscriber : public Node {
public:
    Subscriber(const std::string& name, const std::string& topic);

    const std::string& topic() const;

    void init() override;
    void shutdown() override;

    // 被动接收消息，输出内容与时间戳；虚函数便于扩展自定义接收行为
    virtual void receive(const Message& message);

private:
    std::string m_topic;
};
