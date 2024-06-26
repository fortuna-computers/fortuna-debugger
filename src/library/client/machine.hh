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

    void      load_user_definition(std::string const& filename);
    DebugInfo compile(std::string const& filename) const;

    uint16_t    id;
    std::string name;
    std::string comment_separators;
    uint16_t    terminal_lines = 25;
    uint16_t    terminal_columns = 80;

    struct Register { std::string name; uint8_t size; };
    std::vector<Register>    registers;
    std::vector<std::string> flags;

    std::vector<Register>    cycle_bytes;
    std::vector<std::string> cycle_bits;

    struct Memory { std::string name; size_t size; };
    std::vector<Memory> memories;

    int         uc_baudrate;
    std::string vendor_id;
    std::string product_id;
    size_t      uc_wait;

private:
    struct lua_State* L;
    bool              user_definition_loaded_ = false;
    std::string       lua_src_path;

    bool                     get_field(const char* field, bool mandatory=true) const;
    bool                     field_bool(const char* field, bool mandatory=true) const;
    ssize_t                  field_int(const char* field, bool mandatory=true) const;
    std::string              field_str(const char* field, bool mandatory=true) const;
    std::vector<uint8_t>     field_byte_array(const char* field, bool mandatory=true) const;
    std::vector<std::string> field_string_array(const char* field, bool mandatory=true) const;
    void                     assert_stack(int sz) const;
};

#endif
