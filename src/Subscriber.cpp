#include "Subscriber.h"
#include "ROS.h"
#include "Message.h"
#include "Exception.h"
#include "Logger.h"

Subscriber::Subscriber(const std::string& name, const std::string& topic)
    : Node(name), m_topic(topic) {
    if (topic.empty()) {
        throw TopicException("Topic name must not be empty for subscriber: " + name);
    }
}

const std::string& Subscriber::topic() const {
    return m_topic;
}

void Subscriber::init() {
    if (isInitialized()) {
        LOG_WARN("Subscriber", "[" + name() + "] already initialized, skip");
        return;
    }
    LOG_INFO("Subscriber", "[" + name() + "] initializing on topic [" + m_topic + "]");
    ROS::instance().registerSubscriber(this);
    markInitialized();
}

void Subscriber::shutdown() {
    if (!isInitialized()) {
        LOG_WARN("Subscriber", "[" + name() + "] not initialized, skip shutdown");
        return;
    }
    LOG_INFO("Subscriber", "[" + name() + "] shutting down");
    markShutdown();
}

void Subscriber::receive(const Message& message) {
    LOG_INFO("Subscriber", "[" + name() + "] received from [" + m_topic + "]: \"" +
                               message.content() + "\" at " + message.timestamp());
}
