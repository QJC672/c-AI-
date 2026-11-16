#include "TestFramework.h"
#include "ROS.h"
#include "Publisher.h"
#include "Subscriber.h"
#include "Message.h"
#include "Logger.h"
#include "Exception.h"

#include <cstddef>
#include <string>

namespace {

// 用计数订阅者验证「一次发布，多个订阅者都收到」
class CountingSubscriber : public Subscriber {
public:
    CountingSubscriber(const std::string& name, const std::string& topic)
        : Subscriber(name, topic) {}

    void receive(const Message& message) override {
        ++m_received;
        m_lastContent = message.content();
        m_lastTimestamp = message.timestamp();
    }

    int received() const { return m_received; }
    const std::string& lastContent() const { return m_lastContent; }
    const std::string& lastTimestamp() const { return m_lastTimestamp; }

private:
    int m_received = 0;
    std::string m_lastContent;
    std::string m_lastTimestamp;
};

void cleanup() {
    ROS::instance().shutdownAll();
}

// ---------- 正常场景 ----------

void testMessageContentAndTimestamp() {
    Message message("sensor reading");
    ASSERT_EQ(message.content(), std::string("sensor reading"));
    ASSERT_EQ(message.timestamp().size(), std::size_t(19));
    ASSERT_TRUE(message.timestamp()[4] == '-');
    ASSERT_TRUE(message.timestamp()[7] == '-');
    ASSERT_TRUE(message.timestamp()[13] == ':');
}

void testRegistrationAndCounts() {
    Publisher publisher("pub_count", "topic/count");
    Subscriber first("sub_count_1", "topic/count");
    Subscriber second("sub_count_2", "topic/count");

    publisher.init();
    first.init();
    second.init();

    ASSERT_EQ(ROS::instance().publisherCount("topic/count"), std::size_t(1));
    ASSERT_EQ(ROS::instance().subscriberCount("topic/count"), std::size_t(2));
    ASSERT_EQ(ROS::instance().nodeCount(), std::size_t(3));
    cleanup();
}

void testMultiSubscriberDelivery() {
    Publisher publisher("pub_delivery", "topic/delivery");
    CountingSubscriber first("sub_delivery_1", "topic/delivery");
    CountingSubscriber second("sub_delivery_2", "topic/delivery");

    publisher.init();
    first.init();
    second.init();
    publisher.publish("hello");
    cleanup();  // shutdownAll 会 join 异步派发线程

    ASSERT_EQ(first.received(), 1);
    ASSERT_EQ(second.received(), 1);
    ASSERT_EQ(first.lastContent(), std::string("hello"));
    ASSERT_EQ(second.lastContent(), std::string("hello"));
    ASSERT_TRUE(!first.lastTimestamp().empty());
}

void testContinuousPublish() {
    Publisher publisher("pub_continuous", "topic/continuous");
    CountingSubscriber subscriber("sub_continuous", "topic/continuous");
    publisher.init();
    subscriber.init();

    publisher.publish("msg-1");
    publisher.publish("msg-2");
    publisher.publish("msg-3");
    cleanup();

    ASSERT_EQ(subscriber.received(), 3);
    ASSERT_EQ(subscriber.lastContent(), std::string("msg-3"));
}

void testShutdownClearsState() {
    Publisher publisher("pub_shutdown", "topic/shutdown");
    Subscriber subscriber("sub_shutdown", "topic/shutdown");
    publisher.init();
    subscriber.init();
    ASSERT_EQ(ROS::instance().nodeCount(), std::size_t(2));

    cleanup();
    ASSERT_EQ(ROS::instance().nodeCount(), std::size_t(0));
}

void testRepeatedInitIsSafe() {
    Publisher publisher("pub_repeat", "topic/repeat");
    publisher.init();
    publisher.init();  // 仅 WARN，不抛异常也不重复注册
    ASSERT_EQ(ROS::instance().nodeCount(), std::size_t(1));
    cleanup();
}

// ---------- 边界 / 异常场景 ----------

void testEmptyNodeNameThrows() {
    ASSERT_THROW(Publisher invalid("", "topic"), NodeException);
}

void testEmptyTopicThrows() {
    ASSERT_THROW(Subscriber invalid("sub", ""), TopicException);
}

void testEmptyMessageThrows() {
    ASSERT_THROW(Message empty(""), MessageException);
}

void testEmptyPublishDataThrows() {
    Publisher publisher("pub_empty", "topic/empty");
    publisher.init();
    ASSERT_THROW(publisher.publish(""), MessageException);
    cleanup();
}

void testDuplicateRegistrationThrows() {
    Publisher first("same_name", "topic/duplicate");
    Publisher second("same_name", "topic/duplicate");
    first.init();
    ASSERT_THROW(second.init(), NodeException);
    cleanup();
}

void testUnknownTopicThrows() {
    Message message("orphan");
    ASSERT_THROW(ROS::instance().publish("topic/ghost", message), TopicException);
}

void testPublishBeforeInitThrows() {
    Publisher publisher("pub_uninitialized", "topic/uninitialized");
    ASSERT_THROW(publisher.publish("data"), NodeException);
}

}  // namespace

int main() {
    Logger::instance().disableFileLog();  // 测试期间不写日志文件

    TestRunner& runner = TestRunner::instance();
    runner.run("Message content and timestamp format", testMessageContentAndTimestamp);
    runner.run("Registration and mapping counts", testRegistrationAndCounts);
    runner.run("One publish delivered to multiple subscribers", testMultiSubscriberDelivery);
    runner.run("Continuous publish delivers every message", testContinuousPublish);
    runner.run("shutdownAll clears global state", testShutdownClearsState);
    runner.run("Repeated init is safe", testRepeatedInitIsSafe);
    runner.run("Empty node name throws NodeException", testEmptyNodeNameThrows);
    runner.run("Empty topic name throws TopicException", testEmptyTopicThrows);
    runner.run("Empty message throws MessageException", testEmptyMessageThrows);
    runner.run("Empty publish data throws MessageException", testEmptyPublishDataThrows);
    runner.run("Duplicate registration throws NodeException", testDuplicateRegistrationThrows);
    runner.run("Unknown topic throws TopicException", testUnknownTopicThrows);
    runner.run("Publish before init throws NodeException", testPublishBeforeInitThrows);

    return runner.summary() ? 0 : 1;
}
