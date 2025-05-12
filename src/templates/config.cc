#include "config.hh"

#include <fstream>
#include <iostream>
#include <string>

#include "../libs/tomlplusplus/toml.hpp"
#include "../pkgs/base/include.hh"

Config::Config(std::string &file)
    : filePath(file)
    , open(false)
    , valid(false) {
    load();
}

Config::~Config() { close(); }

auto Config::load() -> void {
    if (open) {
        close();
    }

    auto config = toml::parse_file(filePath);

    for (const auto &pair : config) {
        values[std::string(pair.first)] =
            std::string(pair.second.as_string()->get());
    }

    open = true;
    valid = true;
}

auto Config::save() -> void {
    if (!open) {
        return;
    }

    std::ofstream file(filePath);
    if (!file.is_open()) {
        log("failed to open file: " + filePath, LogLevel::ERROR);
        return;
    }
}

auto Config::close() -> void {
    if (open) {
        save();
        values.clear();
        open = false;
        valid = false;
    }
}

auto Config::operator[](const std::string &key) -> std::string & {
    return values[key];
}

auto Config::operator[](std::string key) -> std::string { return values[key]; }

auto Config::get(const std::string &key) -> std::string { return values[key]; }

auto Config::set(const std::string &key, const std::string &value) -> void {
    values[key] = value;
}

auto Config::remove(const std::string &key) -> void { values.erase(key); }

auto Config::clear() -> void { values.clear(); }

auto Config::is_open() const -> bool { return open; }

auto Config::get_all_keys(void (*process)(const std::string &))
    -> std::vector<std::string> {
    std::vector<std::string> keys;
    if (process != nullptr) {
        for (auto &key : values) {
            process(key.first);
            keys.push_back(key.first);
        }
    } else {
        for (auto &key : values) {
            keys.push_back(key.first);
        }
    }
    return keys;
}

auto Config::get_all_values(void (*process)(const std::string &))
    -> std::vector<std::string> {
    std::vector<std::string> vals;
    if (process != nullptr) {
        for (auto &val : values) {
            process(val.second);
            vals.push_back(val.second);
        }
    } else {
        for (auto &val : values) {
            vals.push_back(val.second);
        }
    }
    return vals;
}

auto Config::get_all(void (*process)(const std::string &, const std::string &))
    -> std::map<std::string, std::string> {
    std::map<std::string, std::string> pairs;
    if (process != nullptr) {
        for (auto &pair : values) {
            process(pair.first, pair.second);
            pairs[pair.first] = pair.second;
        }
    } else {
        for (auto &pair : values) {
            pairs[pair.first] = pair.second;
        }
    }
    return pairs;
}

auto Config::contains(const std::string &key) const -> bool {
    return values.find(key) != values.end();
}

auto Config::validate() const -> bool { return valid; }

int main() {  // testing
    std::string file = ".c-set";
    Config config(file);

    std::cout << "Config file: " << file << '\n';
    config.load();
    config.get_all([](const std::string &key, const std::string &val) {
        log(key, "key:   ", LogLevel::INFO);
        log(val, "value: ", LogLevel::INFO);
    });
}