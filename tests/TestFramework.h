#pragma once

#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

// 极简结构化测试框架：收集用例执行结果并汇总
class TestRunner {
public:
    static TestRunner& instance() {
        static TestRunner runner;
        return runner;
    }

    void run(const std::string& name, const std::function<void()>& testCase) {
        ++m_total;
        try {
            testCase();
            ++m_passed;
            std::cout << "[PASS] " << name << std::endl;
        } catch (const std::exception& error) {
            ++m_failed;
            std::cout << "[FAIL] " << name << " -> " << error.what() << std::endl;
        }
    }

    bool summary() const {
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "Total: " << m_total << "  Passed: " << m_passed
                  << "  Failed: " << m_failed << std::endl;
        return m_failed == 0;
    }

private:
    TestRunner() = default;
    int m_total = 0;
    int m_passed = 0;
    int m_failed = 0;
};

#define ASSERT_TRUE(condition)                                                        \
    do {                                                                              \
        if (!(condition)) {                                                           \
            throw std::runtime_error(std::string("ASSERT_TRUE failed: ") + #condition + \
                                     " (line " + std::to_string(__LINE__) + ")");       \
        }                                                                             \
    } while (false)

#define ASSERT_EQ(actual, expected)                                                   \
    do {                                                                              \
        if (!((actual) == (expected))) {                                              \
            throw std::runtime_error(std::string("ASSERT_EQ failed: ") + #actual +    \
                                     " == " + #expected +                            \
                                     " (line " + std::to_string(__LINE__) + ")");     \
        }                                                                             \
    } while (false)

#define ASSERT_THROW(statement, ExceptionType)                                        \
    do {                                                                              \
        bool caught = false;                                                          \
        try {                                                                         \
            statement;                                                                \
        } catch (const ExceptionType&) {                                              \
            caught = true;                                                            \
        }                                                                             \
        if (!caught) {                                                                \
            throw std::runtime_error(std::string("Expected exception not thrown: ") + \
                                     #ExceptionType +                                 \
                                     " (line " + std::to_string(__LINE__) + ")");      \
        }                                                                             \
    } while (false)
