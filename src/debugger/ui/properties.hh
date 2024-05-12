#ifndef PROPERTIES_HH_
#define PROPERTIES_HH_

#include "imgui.h"

#include <unordered_map>
#include <string>

class IniPropertiesFile {
public:
    void initialize(struct ImGuiContext* context);

    std::string get(std::string const& key) const;
    bool        get_bool(std::string const& key) const;

    void        set(std::string const& key, std::string const& value);
    void        set(std::string const& key, bool value);

    void        save();

private:
    std::unordered_map<std::string, std::string> properties_ {};
};

#endif //PROPERTIES_HH_
