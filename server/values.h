#ifndef VALUES_H
#define VALUES_H

#include <string>
#include <optional>

class Values
{
public:
    virtual std::optional<std::string> get(const std::string& key) const = 0;
    virtual void set(const std::string& key, const std::string& value) = 0;
    virtual ~Values() = default;
};

#endif // VALUES_H
