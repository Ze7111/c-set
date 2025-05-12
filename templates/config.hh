#include <map>
#include <string>
#include <vector>

#include "../incl/cds.hh"

class Config : public ConfigDataSource {
  public:
    explicit
     Config(std::string &file);
    ~Config();
     Config(const Config &other)
        : values(other.values)
        , filePath(other.filePath)
        , open(other.open)
        , valid(other.valid) {}
     Config &operator=(const Config &other) {
        if (this != &other) {
            values = other.values;
            filePath = other.filePath;
            open = other.open;
            valid = other.valid;
        }
        return *this;
    }
     Config(Config &&other) noexcept
        : values(std::move(other.values))
        , filePath(std::move(other.filePath))
        , open(other.open)
        , valid(other.valid) {
        other.open = false;
        other.valid = false;
    }
     Config &operator=(Config &&other) noexcept {
        if (this != &other) {
            values = std::move(other.values);
            filePath = std::move(other.filePath);
            open = other.open;
            valid = other.valid;
            other.open = false;
            other.valid = false;
        }
        return *this;
    }

    auto load() -> void;
    auto save() -> void;
    auto close() -> void;

    auto operator[](const std::string &key) -> std::string &;
    auto operator[](std::string key) -> std::string;
    auto get(const std::string &key) -> std::string override;
    auto set(const std::string &key, const std::string &value) -> void;
    auto remove(const std::string &key) -> void;
    auto clear() -> void;

    auto is_open() const -> bool;

    auto get_all_keys(void (*process)(const std::string &) = nullptr)
        -> std::vector<std::string>;
    auto get_all_values(void (*process)(const std::string &) = nullptr)
        -> std::vector<std::string>;
    auto get_all(void (*process)(const std::string &, const std::string &) = nullptr)
        -> std::map<std::string, std::string>;
    auto contains(const std::string &key) const -> bool;
    auto validate() const -> bool;

  private:
    std::map<std::string, std::string> values;
    std::string filePath;
    bool open;
    bool valid;
};