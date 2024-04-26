#ifndef __CDS_H__
#define __CDS_H__

#include <string>

class ConfigDataSource {
public:
    ConfigDataSource() = default;
    ConfigDataSource(const ConfigDataSource&) = default;
    ConfigDataSource& operator=(const ConfigDataSource&) = default;
    ConfigDataSource(ConfigDataSource&&) = default;
    ConfigDataSource& operator=(ConfigDataSource&&) = default;

    virtual ~ConfigDataSource() {}
    virtual std::string get(const std::string& key) = 0;
};

#endif // __CDS_H__