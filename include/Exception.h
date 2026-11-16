#pragma once

#include <stdexcept>
#include <string>

// ROS 模拟系统所有异常的基类
class ROSException : public std::runtime_error {
public:
    explicit ROSException(const std::string& message) : std::runtime_error(message) {}
};

// 节点相关异常：空名称、重复注册等
class NodeException : public ROSException {
public:
    explicit NodeException(const std::string& message) : ROSException(message) {}
};

// 话题相关异常：空话题、向不存在话题转发等
class TopicException : public ROSException {
public:
    explicit TopicException(const std::string& message) : ROSException(message) {}
};

// 消息相关异常：空消息内容等
class MessageException : public ROSException {
public:
    explicit MessageException(const std::string& message) : ROSException(message) {}
};
