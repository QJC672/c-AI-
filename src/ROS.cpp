#include "ROS.h"
#include "Node.h"
#include "Publisher.h"
#include "Subscriber.h"
#include "Message.h"
#include "Exception.h"
#include "Logger.h"

#include <utility>

ROS& ROS::instance() {
    static ROS core;  // 局部静态，C++11 起保证线程安全且全局唯一
    return core;
}

void ROS::registerPublisher(Publisher* publisher) {
    if (publisher == nullptr) {
        throw NodeException("Cannot register a null publisher");
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_registeredNames.find(publisher->name()) != m_registeredNames.end()) {
        throw NodeException("Duplicate node registration: " + publisher->name());
    }

    m_registeredNames.insert(publisher->name());
    m_publishers[publisher->topic()].push_back(publisher);
    m_allNodes.push_back(publisher);

    LOG_INFO("ROS", "Publisher [" + publisher->name() + "] registered to topic [" +
                        publisher->topic() + "]");
}

void ROS::registerSubscriber(Subscriber* subscriber) {
    if (subscriber == nullptr) {
        throw NodeException("Cannot register a null subscriber");
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_registeredNames.find(subscriber->name()) != m_registeredNames.end()) {
        throw NodeException("Duplicate node registration: " + subscriber->name());
    }

    m_registeredNames.insert(subscriber->name());
    m_subscribers[subscriber->topic()].push_back(subscriber);
    m_allNodes.push_back(subscriber);

    LOG_INFO("ROS", "Subscriber [" + subscriber->name() + "] registered to topic [" +
                        subscriber->topic() + "]");
}

void ROS::publish(const std::string& topic, const Message& message) {
    if (topic.empty()) {
        throw TopicException("Topic name must not be empty");
    }

    std::vector<Subscriber*> targets;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_subscribers.find(topic);
        if (it == m_subscribers.end() || it->second.empty()) {
            throw TopicException("No subscriber registered for topic: " + topic);
        }
        targets = it->second;
    }

    // 异步派发：每个订阅者一个独立任务，publish 立即返回
    std::lock_guard<std::mutex> lock(m_mutex);
    for (Subscriber* subscriber : targets) {
        Message payload = message;
        m_dispatchThreads.emplace_back([subscriber, payload]() {
            subscriber->receive(payload);
        });
    }
}

void ROS::shutdownAll() {
    std::vector<Node*> nodes;
    std::vector<std::thread> threads;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        nodes = m_allNodes;
        threads.reserve(m_dispatchThreads.size());
        for (std::thread& worker : m_dispatchThreads) {
            threads.push_back(std::move(worker));
        }
        m_dispatchThreads.clear();
    }

    for (std::thread& worker : threads) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    for (Node* node : nodes) {
        if (node != nullptr && node->isInitialized()) {
            node->shutdown();
        }
    }

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_publishers.clear();
        m_subscribers.clear();
        m_allNodes.clear();
        m_registeredNames.clear();
    }

    LOG_INFO("ROS", "All nodes have been shut down");
}

std::size_t ROS::publisherCount(const std::string& topic) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_publishers.find(topic);
    return it == m_publishers.end() ? 0 : it->second.size();
}

std::size_t ROS::subscriberCount(const std::string& topic) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_subscribers.find(topic);
    return it == m_subscribers.end() ? 0 : it->second.size();
}

std::size_t ROS::nodeCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_allNodes.size();
}
