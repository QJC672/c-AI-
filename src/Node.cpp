#include "Node.h"
#include "Exception.h"

Node::Node(const std::string& name) : m_name(name) {
    if (name.empty()) {
        throw NodeException("Node name must not be empty");
    }
}

const std::string& Node::name() const {
    return m_name;
}

bool Node::isInitialized() const {
    return m_initialized;
}

void Node::markInitialized() {
    m_initialized = true;
}

void Node::markShutdown() {
    m_initialized = false;
}
