#pragma once

#pragma once

#include <atomic>
#include <mutex>
#include <unordered_map>
#include <string>
#include <thread>

#include "stats.h"

class StatisticsManager : public Stats {
public:
    struct KeyStats {
        std::atomic<uint64_t> reads{0};
        std::atomic<uint64_t> writes{0};
    };

    StatisticsManager();
    ~StatisticsManager();

    void recordRead(const std::string& key) override;
    void recordWrite(const std::string& key) override;

    KeyStatistics getKeyStats(const std::string& key) const override;
    void printStatistics() const;

    void debugDump() const;

    StatisticsManager(const StatisticsManager&) = delete;
    StatisticsManager& operator=(const StatisticsManager&) = delete;

private:
    void resetRecentCommands();
    void printerLoop();
    void recordCommand(const std::string& key = "", bool isRead = true);
    std::string getCurrentTime() const;

    std::atomic<uint64_t> totalCommands_;
    std::atomic<uint64_t> recentCommands_;

    mutable std::mutex keyStatsMutex_;
    std::unordered_map<std::string, KeyStats> keyStatsMap_;

    std::atomic<bool> running_;
    std::thread printerThread_;
};
