#pragma once

#include <memory>

class Stats {
public:
    struct KeyStatistics {
        uint64_t reads;
        uint64_t writes;
    };

    typedef std::shared_ptr<Stats> pointer;
    virtual ~Stats() = default;

    virtual void recordRead(const std::string& key) = 0;
    virtual void recordWrite(const std::string& key) = 0;

    virtual KeyStatistics getKeyStats(const std::string& key) const = 0;

};

