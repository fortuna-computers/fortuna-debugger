#include "filewatcher/filewatcher.hh"

void FileWatcher::update_files(std::vector<std::string> const &files)
{
    files_.clear();
    for (std::string const& filename: files)
        files_[filename] = std::filesystem::last_write_time(filename);
}

void FileWatcher::run_verify()
{
    if (updated_)
        return;  // don't run if we already know files changed
    if (thread_.joinable())
        thread_.join();  // makes sure we're not running one verify in top of another

    thread_ = std::thread([this]() {
        for (auto& file: files_) {
            auto tm = std::filesystem::last_write_time(file.first);
            if (tm > file.second) {
                file.second = tm;
                updated_ = true;
            }
        }
    });
}

FileWatcher::~FileWatcher()
{
    if (thread_.joinable())
        thread_.join();
}
