#ifndef CONFIG_HH_
#define CONFIG_HH_

#include "ui/properties.hh"

struct Config {
    explicit Config(IniPropertiesFile& ini_properties_file) : ini_properties_file_(ini_properties_file) {}

    bool verify_upload_rom = true;

    void load();
    void save();

private:
    IniPropertiesFile& ini_properties_file_;
};

#endif