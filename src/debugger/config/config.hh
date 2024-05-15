#ifndef CONFIG_HH_
#define CONFIG_HH_

#include "ui/properties.hh"

struct Config {

    std::string get_str(std::string const& key) const;
    int         get_int(std::string const& key) const;
    bool        get_bool(std::string const& key) const;

    void        set(std::string const& key, std::string const& value);
    void        set(std::string const& key, const char* value) { set(key, std::string(value)); }
    void        set(std::string const& key, int value);
    void        set(std::string const& key, bool value);

    void load();
    void save();

private:
    std::unordered_map<std::string, std::string> config_;
    std::string filename = std::string(getenv("HOME")) + "/.config/.fdbg.rc";
};

#endif