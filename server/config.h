#ifndef CONFIG_H
#define CONFIG_H

#include <unordered_map>
#include <shared_mutex>
#include <filesystem>
#include <optional>
#include <atomic>
#include <thread>

#include "values.h"

class Config : public Values {
    std::unordered_map<std::string, std::string> config_;
    mutable std::shared_mutex mutex_;
    std::filesystem::path path_;
    std::atomic<bool> dirty_;
    std::atomic<bool> running_;
    std::thread saveThread_;
public:
    Config();
    ~Config() override;
    void saveLoop();
    bool load(const std::filesystem::path &filename);
    bool save();
    std::optional<std::string> get(const std::string_view& key) const override;
    void set(const std::string_view& key, const std::string_view& value) override;
};

#endif // CONFIG_H
