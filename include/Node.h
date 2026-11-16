#pragma once

#include <string>

// 所有节点的抽象父类：封装唯一名称，定义初始化与关闭的统一行为规范
class Node {
public:
    explicit Node(const std::string& name);
    virtual ~Node() = default;

    const std::string& name() const;
    bool isInitialized() const;

    // 子类必须实现，并在实现中输出初始化/关闭日志
    virtual void init() = 0;
    virtual void shutdown() = 0;

    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

protected:
    void markInitialized();
    void markShutdown();

private:
    std::string m_name;
    bool m_initialized = false;
};
