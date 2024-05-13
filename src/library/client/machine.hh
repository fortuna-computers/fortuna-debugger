#ifndef MACHINE_HH_
#define MACHINE_HH_

#include <cstdint>
#include <string>

class Machine {
public:
    Machine();
    ~Machine();

    void load_user_definition(std::string const& filename);

    uint16_t    id;
    std::string name;
    uint64_t    total_memory;

    int         uc_baudrate;
    std::string vendor_id;
    std::string product_id;

private:
    struct lua_State* L;

    void get_field(const char* field, bool mandatory) const;
    void assert_stack(int sz) const;
};

#endif