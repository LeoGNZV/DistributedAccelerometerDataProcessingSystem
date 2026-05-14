#pragma once

#include <iostream>
#include <mutex>
#include <string>
#include <chrono>
#include <iomanip>

class Logger {
public:
    static void info(const std::string& msg);
    static void error(const std::string& msg);

private:
    static std::mutex mutex_;
};
