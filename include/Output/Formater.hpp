#include <chrono>
#include <format>

std::string time_point_to_str(std::chrono::system_clock::time_point tp) {
    return std::format("{:%Y-%m-%d %H:%M}", tp);
}