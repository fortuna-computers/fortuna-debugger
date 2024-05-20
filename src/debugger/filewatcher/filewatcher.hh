#ifndef FILEWATCHER_HH_
#define FILEWATCHER_HH_

#include <atomic>
#include <filesystem>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

class FileWatcher {
public:
    ~FileWatcher();

    void update_file_list(std::vector<std::string> const& file_list);
    void reset();
    void run();

private:
    std::unordered_map<std::string, std::filesystem::file_time_type> file_list_;
    std::mutex file_list_mutex_;
    std::atomic<bool> files_updated_ = false;

    bool running_ = true;
    std::thread thread_;
};

#endif