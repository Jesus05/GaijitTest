#include "statisticsmanager.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>

StatisticsManager::StatisticsManager()
    : totalCommands_(0)
    , recentCommands_(0)
    , running_(true)
    , detailedStatsEnabled_(true)
    , printerThread_(&StatisticsManager::printerLoop, this)
{
}

StatisticsManager::~StatisticsManager() {
    running_ = false;
    if (printerThread_.joinable()) {
        printerThread_.join();
    }

    std::cout << "\n[FINAL STATISTICS]" << std::endl;
    printStatistics();
}

void StatisticsManager::recordCommand(const std::string& key, bool isRead) {
    totalCommands_++;
    recentCommands_++;

    if (!key.empty() && detailedStatsEnabled_) {
        std::lock_guard<std::mutex> lock(keyStatsMutex_);
        auto& stats = keyStatsMap_[key];
        if (isRead) {
            stats.reads++;
        } else {
            stats.writes++;
        }
    }
}

void StatisticsManager::recordRead(const std::string& key) {
    recordCommand(key, true);
}

void StatisticsManager::recordWrite(const std::string& key) {
    recordCommand(key, false);
}

StatisticsManager::KeyStatistics StatisticsManager::getKeyStats(const std::string& key) const {
    if (!detailedStatsEnabled_) {
        return {0, 0};
    }

    std::lock_guard<std::mutex> lock(keyStatsMutex_);
    auto it = keyStatsMap_.find(key);
    if (it != keyStatsMap_.end()) {
        return {it->second.reads.load(), it->second.writes.load()};
    }
    return {0, 0};
}

void StatisticsManager::resetRecentCommands() {
    recentCommands_.store(0);
}

std::string StatisticsManager::getCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

void StatisticsManager::printStatistics() const {
    auto total = totalCommands_.load();
    auto recent = recentCommands_.load();

    std::cout << "[" << getCurrentTime() << "] "
              << "Total: " << total
              << ", Last 5s: " << recent;
    std::cout << std::endl;
}

void StatisticsManager::printerLoop() {
    while (running_) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        printStatistics();
        resetRecentCommands();
    }
}

// void StatisticsManager::enableDetailedStats(bool enable) {
//     detailedStatsEnabled_ = enable;
// }

// uint64_t StatisticsManager::getTotalCommands() const {
//     return totalCommands_.load();
// }

// uint64_t StatisticsManager::getRecentCommands() const {
//     return recentCommands_.load();
// }

// void StatisticsManager::reset() {
//     totalCommands_.store(0);
//     recentCommands_.store(0);
//     std::lock_guard<std::mutex> lock(keyStatsMutex_);
//     keyStatsMap_.clear();
// }

void StatisticsManager::debugDump() const {
    std::cout << "\n=== DEBUG STATISTICS DUMP ===" << std::endl;
    std::cout << "Total commands: " << totalCommands_.load() << std::endl;
    std::cout << "Recent commands: " << recentCommands_.load() << std::endl;

    if (detailedStatsEnabled_) {
        std::lock_guard<std::mutex> lock(keyStatsMutex_);
        std::cout << "Key statistics (" << keyStatsMap_.size() << " keys):" << std::endl;
        for (const auto& [key, stats] : keyStatsMap_) {
            std::cout << "  " << key << ": reads=" << stats.reads.load()
            << ", writes=" << stats.writes.load() << std::endl;
        }
    }
    std::cout << "============================\n" << std::endl;
}

// std::unordered_map<std::string, StatisticsManager::KeyStatistics>
// StatisticsManager::getAllKeyStats() const {
//     std::unordered_map<std::string, KeyStatistics> result;
//     if (detailedStatsEnabled_) {
//         std::lock_guard<std::mutex> lock(keyStatsMutex_);
//         for (const auto& [key, stats] : keyStatsMap_) {
//             result[key] = {stats.reads.load(), stats.writes.load()};
//         }
//     }
//     return result;
// }

// size_t StatisticsManager::getUniqueKeysCount() const {
//     if (!detailedStatsEnabled_) return 0;
//     std::lock_guard<std::mutex> lock(keyStatsMutex_);
//     return keyStatsMap_.size();
// }
