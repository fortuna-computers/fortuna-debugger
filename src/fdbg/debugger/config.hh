#ifndef CONFIG_HH_
#define CONFIG_HH_

#include "imgui.h"

#include <unordered_map>
#include <string>

class Config {
public:
    void initialize(struct ImGuiContext* context);

    std::string get(std::string const& key) const;
    void        set(std::string const& key, std::string const& value);

    void        save();

private:
    std::unordered_map<std::string, std::string> properties_ {};
};

#endif //CONFIG_HH_
