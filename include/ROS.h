#pragma once

#include <cstddef>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Node;
class Publisher;
class Subscriber;
class Message;

// ROS 核心管理模块（单例）：维护话题映射关系、消息异步转发与全局关闭
class ROS {
public:
    static ROS& instance();

    void registerPublisher(Publisher* publisher);
    void registerSubscriber(Subscriber* subscriber);

    // 将消息转发给指定话题下的所有订阅者（异步，不等待订阅者）
    void publish(const std::string& topic, const Message& message);

    // 回收派发线程并遍历关闭所有已注册节点
    void shutdownAll();

    std::size_t publisherCount(const std::string& topic) const;
    std::size_t subscriberCount(const std::string& topic) const;
    std::size_t nodeCount() const;

    ROS(const ROS&) = delete;
    ROS& operator=(const ROS&) = delete;

private:
    ROS() = default;
    ~ROS() = default;

    std::unordered_map<std::string, std::vector<Publisher*>> m_publishers;
    std::unordered_map<std::string, std::vector<Subscriber*>> m_subscribers;
    std::vector<Node*> m_allNodes;
    std::unordered_set<std::string> m_registeredNames;
    std::vector<std::thread> m_dispatchThreads;
    mutable std::mutex m_mutex;
};
