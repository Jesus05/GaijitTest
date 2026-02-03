#include "config.h"

#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

Config::Config()
    : dirty_(false),
    running_(true),
    saveThread_(&Config::saveLoop, this) {
}

Config::~Config() {
    running_ = false;
    if (saveThread_.joinable()) {
        saveThread_.join();
    }
}

void Config::saveLoop() {
    while (running_) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (dirty_){
            save();
            dirty_ = false;
        }
    }
}

bool Config::load(const std::filesystem::__cxx11::path &filename) {
    path_ = filename;
    std::lock_guard lock(mutex_);
    config_.clear();

    if (!fs::exists(path_)) {
        std::ofstream newFile(path_);
        if (!newFile) {
            std::cerr << "Unable to create config file:" << path_ << std::endl;
            return false;
        }
        return true;
    }

    std::ifstream file(path_);
    if (!file.is_open()) {
        std::cerr << "Unable to open config file:" << path_ << std::endl;
        return false;
    }

    std::string line;

    while (std::getline(file, line)) {

        if (line.empty() || line[0] == '#') {
            continue;
        }

        const size_t start = line.find_first_not_of(" \t");
        if (start == std::string::npos) continue;

        const size_t end = line.find_last_not_of(" \t");
        const std::string trimmed = line.substr(start, end - start + 1);

        const size_t eqPos = trimmed.find('=');
        if (eqPos == std::string::npos || eqPos == 0) {
            std::cerr << "Uncorrect values in cofign IGNORED:" << trimmed << std::endl;
            continue;
        }

        std::string key = trimmed.substr(0, eqPos);
        std::string value = trimmed.substr(eqPos + 1);

        const size_t keyEnd = key.find_last_not_of(" \t");
        if (keyEnd != std::string::npos) {
            key = key.substr(0, keyEnd + 1);
        }

        const size_t valueStart = value.find_first_not_of(" \t");
        if (valueStart != std::string::npos) {
            value = value.substr(valueStart);
        }

        if (key.empty()) {
            continue;
        }

        config_[key] = value;
    }

    return true;
}

bool Config::save() {
    std::shared_lock lock(mutex_);
    fs::path tempPath = path_;
    tempPath += ".tmp";

    std::ofstream file(tempPath, std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Unable to create temp file:" << tempPath << std::endl;
        return false;
    }

    for (const auto& [key, value] : config_) {
        file << key << "=" << value << "\n";
    }

    file.close();

    if (!file.good()) {
        fs::remove(tempPath);
        std::cerr << "Something going wrong while write temp file:" << tempPath << " may be out of free space" << std::endl;
        return false;
    }

    try {
        fs::rename(tempPath, path_);
    } catch (const fs::filesystem_error&) {
        fs::remove(tempPath);
        std::cerr << "Something going wrong while rename temp file:" << tempPath << " may be right problem" << std::endl;
        return false;
    }

    std::cout << "Succesful saved" << std::endl;

    return true;
}

std::optional<std::string> Config::get(const std::string_view &key) const {
    std::shared_lock lock(mutex_);
    auto it = config_.find(std::string(key));
    if (it != config_.end()) {
        return it->second;
    }
    return std::nullopt;
}

void Config::set(const std::string_view &key, const std::string_view &value) {
    if (key.empty()) {
        std::cerr << "Key cannot be empty IGNORED" << std::endl;
        return;
    }

    if (key.find('=') != std::string::npos ||
        key.find('\n') != std::string::npos ||
        key.find('\r') != std::string::npos) {
        std::cerr << "Key contains invalid characters IGNORED" << std::endl;
    }

    if (value.find('\n') != std::string::npos ||
        value.find('\r') != std::string::npos) {
        std::cerr << "Value contains newline characters IGNORED" << std::endl;
    }

    std::lock_guard lock(mutex_);
    config_[std::string(key)] = value;
    dirty_ = true;
}


