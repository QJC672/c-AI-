#pragma once

#include "Node.h"
#include <string>

// 发布者节点：向指定话题发送消息
class Publisher : public Node {
public:
    Publisher(const std::string& name, const std::string& topic);

    const std::string& topic() const;

    void init() override;
    void shutdown() override;

    // 模拟接收传感器字符串 → 构造 Message → 通过 ROS 核心发布
    void publish(const std::string& data);

private:
    std::string m_topic;
};
