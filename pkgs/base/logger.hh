#ifndef __LOG_H__
#define __LOG_H__

#include <array>
#include <iostream>
#include <string>

#include "colors_ansi.hh"

enum class LogLevel { INFO, WARN, ERROR, DEBUG, VERBOSE };

inline auto base_logger(const std::string &msg,
                        LogLevel level = LogLevel::INFO) -> std::string {
    // Format: (bold red) [INFO] (reset) ~ (green) [DATE/TIME] (reset) ~ (msg)
    std::string level_S;
    switch (level) {
    case LogLevel::INFO:
        level_S = std::string(colors::bold) + std::string(colors::bg16::green) +
                  "[INFO]" + std::string(colors::reset);
        break;
    case LogLevel::WARN:
        level_S = std::string(colors::bold) +
                  std::string(colors::bg16::yellow) + "[WARN]" +
                  std::string(colors::reset);
        break;
    case LogLevel::ERROR:
        level_S = std::string(colors::bold) + std::string(colors::bg16::red) +
                  "[ERROR]" + std::string(colors::reset);
        break;
    case LogLevel::DEBUG:
        level_S = std::string(colors::bold) + std::string(colors::bg16::gray) +
                  "[DEBUG]" + std::string(colors::reset);
        break;
    case LogLevel::VERBOSE:
        level_S = std::string(colors::bold) +
                  std::string(colors::bg16::magenta) + "[VERBOSE]" +
                  std::string(colors::reset);
        break;
    }

    std::time_t time_now = std::time(nullptr);
    std::array<char, 26> time_S = {};
    #if defined(_WIN32 ) || defined(_WIN64)
    ctime_s(time_S.data(), time_S.size(), &time_now);
    #else
    ctime_r(&time_now, time_S.data());
    #endif
    time_S[24] = time_S[25] = '\0';  // remove newline and null char

    return level_S + colors::reset + " ~ " + colors::bg16::gray +
           time_S.data() + colors::reset + " ~ " + msg;
}

inline auto log(const std::string &msg, LogLevel level = LogLevel::INFO) -> void {
    std::cout << base_logger(msg, level) << '\n';
}

template <typename T>
concept HasToString = requires(T &&obj) {
    { obj.to_string() } -> std::same_as<std::string>;
};

// anything that has a .to_string() method can be logged
template <typename T>
    requires HasToString<T>
inline auto log(T &obj, const std::string &msg, LogLevel level = LogLevel::INFO)
    -> void {
    // format: base_logger ~ <obj_type at address> >> obj
    std::cout << base_logger(msg, level) << " ~ " << colors::fg16::yellow << "<"
              << typeid(obj).name() << " at " << &obj << ">" << colors::reset
              << colors::fg16::cyan << " -> " << colors::reset
              << obj.to_string() << '\n';
}

template <typename T>
inline auto log(T &obj, const std::string &msg, LogLevel level = LogLevel::INFO) -> void {
    try {
        std::cout << base_logger(msg, level) << " ~ " << colors::fg16::yellow << "<"
              << "obj" << " at " << &obj << ">" << colors::reset
              << colors::fg16::cyan << " -> " << colors::reset
              << std::string(obj) << '\n';
    } catch (std::exception &e) {
        log("to_string() not defined for object! " + msg, LogLevel::ERROR);
    }
}

#endif  // __LOG_H__
