#ifndef __UTILS_H__
#define __UTILS_H__

#ifdef _WIN32
#define debug_enabled !utils::get_env_var("DEBUG").empty()
#else
#define debug_enabled true
#endif
#define dbg(x)                                                                 \
    if (debug_enabled) {                                                       \
        std::cout << x << std::endl;                                           \
        std::cout << "debug of " << yellow << #x << reset << " at " << __FILE__\
                  << ":" << __LINE__ << green << " works!" << reset            \
                  << std::endl;                                                \
    }

#define dbg_all(...)                                                       \
    if (debug_enabled) {                                                   \
        std::cout << red << "dbg " << green << __FILE__ << ":" << __LINE__ \
                  << reset << std::endl;                                   \
        auto argList = {__VA_ARGS__};                                      \
        auto argNames = #__VA_ARGS__;                                      \
        std::istringstream iss(argNames);                                  \
        std::vector<std::string> names(                                    \
            (std::istream_iterator<std::string>(iss)),                     \
            std::istream_iterator<std::string>());                         \
        auto it = argList.begin();                                         \
        for (auto &name : names) {                                         \
            if (name.back() == ',') {                                      \
                name.pop_back();                                           \
            }                                                              \
            std::cout << "\t" << yellow << name << ": " << green << "\""   \
                      << *it << "\"" << reset << std::endl;                \
            if (it != argList.end())                                       \
                ++it;                                                      \
        }                                                                  \
    }

#define TIME_IT(obj)                                                         \
    auto start = std::chrono::high_resolution_clock::now();                  \
    obj;                                                                     \
    auto stop = std::chrono::high_resolution_clock::now();                   \
    auto duration =                                                          \
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start); \
    std_v2::print("line: ", __LINE__, " took ",                              \
                  std::string(colors::fg16::red), duration,                  \
                  std::string(colors::reset))

#endif // __UTILS_H__