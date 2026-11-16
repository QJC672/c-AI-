#include "ROS.h"
#include "Publisher.h"
#include "Subscriber.h"
#include "Message.h"
#include "Logger.h"
#include "Exception.h"

#include <exception>
#include <string>

namespace {

// 正常场景：多个发布者/订阅者、多消息、完整生命周期
void demoNormalFlow() {
    LOG_INFO("Demo", "=== Normal flow: full lifecycle ===");

    Publisher temperaturePublisher("temperature_publisher", "sensor/temperature");
    Publisher humidityPublisher("humidity_publisher", "sensor/humidity");
    Subscriber display("display_node", "sensor/temperature");
    Subscriber recorder("recorder_node", "sensor/temperature");
    Subscriber humidityView("humidity_view", "sensor/humidity");

    temperaturePublisher.init();
    humidityPublisher.init();
    display.init();
    recorder.init();
    humidityView.init();

    temperaturePublisher.publish("temperature=26.5C");
    temperaturePublisher.publish("temperature=27.0C");
    humidityPublisher.publish("humidity=60%");

    ROS::instance().shutdownAll();
}

// 异常场景：非法输入被捕获，进程继续运行
void demoExceptionFlow() {
    LOG_INFO("Demo", "=== Exception flow: no crash ===");

    try {
        Publisher invalidNode("", "sensor/test");
    } catch (const NodeException& error) {
        LOG_ERROR("Demo", std::string("[Empty node name] ") + error.what());
    }

    try {
        Subscriber invalidTopic("invalid_subscriber", "");
    } catch (const TopicException& error) {
        LOG_ERROR("Demo", std::string("[Empty topic name] ") + error.what());
    }

    Publisher publisher("exception_publisher", "sensor/exception");
    Subscriber subscriber("exception_subscriber", "sensor/exception");
    publisher.init();
    subscriber.init();

    try {
        publisher.publish("");
    } catch (const MessageException& error) {
        LOG_ERROR("Demo", std::string("[Empty message] ") + error.what());
    }

    Publisher duplicated("exception_publisher", "sensor/exception");
    try {
        duplicated.init();
    } catch (const NodeException& error) {
        LOG_ERROR("Demo", std::string("[Duplicate registration] ") + error.what());
    }

    try {
        Message orphan("orphan_message");
        ROS::instance().publish("sensor/not_exist", orphan);
    } catch (const TopicException& error) {
        LOG_ERROR("Demo", std::string("[Unknown topic] ") + error.what());
    }

    ROS::instance().shutdownAll();
}

}  // namespace

int main() {
    Logger::instance().enableFileLog("ros_simulation.log");
    LOG_INFO("Main", "ROS simulation started");

    try {
        demoNormalFlow();
        demoExceptionFlow();
    } catch (const ROSException& error) {
        LOG_ERROR("Main", std::string("ROS exception: ") + error.what());
        ROS::instance().shutdownAll();
    } catch (const std::exception& error) {
        LOG_ERROR("Main", std::string("Standard exception: ") + error.what());
        ROS::instance().shutdownAll();
    }

    LOG_INFO("Main", "ROS simulation finished");
    Logger::instance().disableFileLog();
    return 0;
}
