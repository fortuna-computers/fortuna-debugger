#include "filewatcher/filewatcher.hh"

#include <chrono>
using namespace std::chrono_literals;

static constexpr auto INTERVAL = 500ms;

FileWatcher::~FileWatcher()
{
    running_ = false;
    thread_.join();
}

void FileWatcher::update_file_list(std::vector<std::string> const &file_list)
{
    const std::lock_guard<std::mutex> lock(file_list_mutex_);

    file_list_.clear();
    for (std::string const& file: file_list)
        file_list_[file] = std::filesystem::last_write_time(file);
}

void FileWatcher::reset()
{
    files_updated_ = false;
}

void FileWatcher::run()
{
    thread_ = std::thread([this]() {
        for (;;) {
            const std::lock_guard<std::mutex> lock(file_list_mutex_);
            for (auto& f: file_list_) {
                auto filetime = std::filesystem::last_write_time(f.first);
                if (f.second != filetime) {
                    files_updated_ = true;
                    f.second = filetime;
                }
            }
            std::this_thread::sleep_for(INTERVAL);
        }
    });
}

