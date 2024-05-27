#include "config.hh"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

using namespace std::string_literals;

void Config::load()
{
    config_.clear();

    std::ifstream f(filename);
    std::string key, value;

    if (f.good())
        while (f >> key >> value)
            config_[key] = value;
}

void Config::save()
{
    std::ofstream f(filename);
    if (f.good()) {
        std::vector<std::string> keys;
        for (auto const& pair: config_)
            keys.push_back(pair.first);
        std::sort(keys.begin(), keys.end());
        for (auto const& key: keys)
            f << std::left << std::setw(30) << key << " " << config_[key] << "\n";
    }
}

std::string Config::get_str(std::string const &key) const
{
    try {
        return config_.at(key);
    } catch (std::out_of_range& e) {
        return "";
    }
}

int Config::get_int(std::string const &key) const
{
    try {
        return std::stoi(config_.at(key));
    } catch (std::out_of_range& e) {
        return 0;
    }
}

bool Config::get_bool(std::string const &key) const
{
    return get_str(key) == "true";
}

void Config::set(std::string const &key, std::string const &value)
{
    config_[key] = value;
    save();
}

void Config::set(std::string const &key, int value)
{
    set(key, std::to_string(value));
}

void Config::set(std::string const &key, bool value)
{
    set(key, value ? "true"s : "false"s);
}

void Config::remove(std::string const& key)
{
    config_.erase(key);
    save();
}
