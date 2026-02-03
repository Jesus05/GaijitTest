#ifndef VALUES_H
#define VALUES_H

#include <string>
#include <optional>
#include <memory>

class Values {
public:
    typedef std::shared_ptr<Values> pointer;
    virtual std::optional<std::string> get(const std::string_view& key) const = 0;
    virtual void set(const std::string_view& key, const std::string_view& value) = 0;
    virtual ~Values() = default;
};

#endif // VALUES_H
