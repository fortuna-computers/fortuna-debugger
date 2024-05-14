#ifndef CONFIG_HH_
#define CONFIG_HH_

#include "ui/properties.hh"

struct Config {
    bool verify_upload_rom = true;

    void load();
    void save();
};

#endif