#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include "../../incl/cds.hh"

class Template : public ConfigDataSource {
  public:
    explicit Template(std::string &file);
    ~Template();
    Template(const Template &other)
        : values(other.values)
        , filePath(other.filePath)
        , open(other.open)
        , valid(other.valid) {}
    Template &operator=(const Template &other) {
        if (this != &other) {
            values = other.values;
            filePath = other.filePath;
            open = other.open;
            valid = other.valid;
        }
        return *this;
    }
    Template(Template &&other) noexcept
        : values(std::move(other.values))
        , filePath(std::move(other.filePath))
        , open(other.open)
        , valid(other.valid) {
        other.open = false;
        other.valid = false;
    }
    Template &operator=(Template &&other) noexcept {
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

    auto save()      -> void;
    virtual auto load()      -> void;
    virtual auto close()     -> void;
    auto file_name() -> std::string { return filePath; }

    auto operator==(const Template &other) const -> bool {
        return values  == other.values && filePath == other.filePath;
    }
    auto operator!=(const Template &other) const -> bool {
        return !(*this == other);
    }
    auto operator[](const std::string &key) -> std::string &;
    auto operator[](std::string key)  -> std::string;
    auto get(const  std::string &key) -> std::string override;
    auto set(const  std::string &key, const std::string &value) -> void;

    auto all_keys(void (*process)(const std::string &) = nullptr)
        -> std::vector<std::string>;
    auto all_values(void (*process)(const std::string &) = nullptr)
        -> std::vector<std::string>;
    auto all(void (*process)(const std::string &, const std::string &) = nullptr)
        -> std::map<std::string, std::string>;

    auto remove(const std::string &key) -> void;
    auto clear() -> void;

    auto is_open()  const -> bool { return open; };
    auto validate() const -> bool { return valid; }
    auto has(const std::string &key) const -> bool {
        return values.find(key) != values.end();
    } auto compare(const std::string &key,
                 const std::string &value) const -> bool {
        return values.at(key) == value;
    }

  private:
    std::map<std::string, std::string> values;
    std::string filePath;
    bool open;
    bool valid;
};
