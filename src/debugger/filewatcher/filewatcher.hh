#ifndef FILEWATCHER_HH_
#define FILEWATCHER_HH_

#include <atomic>
#include <filesystem>
#include <string>
#include <thread>
#include <vector>
#include <unordered_map>

class FileWatcher {
public:
    ~FileWatcher();

    void update_files(std::vector<std::string> const& files);
    void run_verify();
    void reset() { updated_ = false; }

    bool updated() const { return updated_; }

private:
    std::atomic<bool> updated_ = false;
    std::unordered_map<std::string, std::filesystem::file_time_type> files_;
    std::thread thread_;
};

#endif