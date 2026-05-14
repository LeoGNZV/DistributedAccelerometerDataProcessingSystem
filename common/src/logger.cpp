#include "logger.hpp"

std::mutex Logger::mutex_;

static std::string nowTime() {
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;

    ss << std::put_time(std::localtime(&t), "%F %T");

    return ss.str();
}

void Logger::info(const std::string& msg) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::cout << "[INFO] " << nowTime() << " " << msg << std::endl;
}

void Logger::error(const std::string& msg) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::cerr << "[ERROR] " << nowTime() << " " << msg << std::endl;
}
