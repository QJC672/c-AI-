#include "Publisher.h"
#include "ROS.h"
#include "Message.h"
#include "Exception.h"
#include "Logger.h"

Publisher::Publisher(const std::string& name, const std::string& topic)
    : Node(name), m_topic(topic) {
    if (topic.empty()) {
        throw TopicException("Topic name must not be empty for publisher: " + name);
    }
}

const std::string& Publisher::topic() const {
    return m_topic;
}

void Publisher::init() {
    if (isInitialized()) {
        LOG_WARN("Publisher", "[" + name() + "] already initialized, skip");
        return;
    }
    LOG_INFO("Publisher", "[" + name() + "] initializing on topic [" + m_topic + "]");
    ROS::instance().registerPublisher(this);
    markInitialized();
}

void Publisher::shutdown() {
    if (!isInitialized()) {
        LOG_WARN("Publisher", "[" + name() + "] not initialized, skip shutdown");
        return;
    }
    LOG_INFO("Publisher", "[" + name() + "] shutting down");
    markShutdown();
}

void Publisher::publish(const std::string& data) {
    if (data.empty()) {
        throw MessageException("Cannot publish empty message from [" + name() + "]");
    }
    if (!isInitialized()) {
        throw NodeException("Publisher [" + name() + "] must be initialized before publishing");
    }

    Message message(data);
    LOG_INFO("Publisher", "[" + name() + "] published to [" + m_topic + "]: \"" + data + "\"");
    ROS::instance().publish(m_topic, message);
}
