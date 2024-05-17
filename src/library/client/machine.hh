#ifndef MACHINE_HH_
#define MACHINE_HH_

#include <cstdint>
#include <string>
#include <vector>
#include "debuginfo.hh"

class Machine {
public:
    Machine();
    ~Machine();

    void load_user_definition(std::string const& filename);
    DebugInfo compile(std::string const& filename) const;

    uint16_t    id;
    std::string name;
    uint64_t    total_memory;
    std::string comment_separators;

    int         uc_baudrate;
    std::string vendor_id;
    std::string product_id;

private:
    struct lua_State* L;
    bool   user_definition_loaded_ = false;

    void                     get_field(const char* field, bool mandatory=true) const;
    bool                     field_bool(const char* field, bool mandatory=true) const;
    int                      field_int(const char* field, bool mandatory=true) const;
    std::string              field_str(const char* field, bool mandatory=true) const;
    std::vector<uint8_t>     field_byte_array(const char* field, bool mandatory=true) const;
    std::vector<std::string> field_string_array(const char* field, bool mandatory=true) const;
    void                     assert_stack(int sz) const;
};

#endif