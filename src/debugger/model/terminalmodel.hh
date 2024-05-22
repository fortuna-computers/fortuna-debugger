#ifndef TERMINALMODEL_HH_
#define TERMINALMODEL_HH_

#include "client/ievents.hh"

#include <array>

class TerminalModel : public ITerminal {
public:
    struct Char { char c; };

    TerminalModel();

    void add_char(char c) override;

    constexpr std::pair<size_t, size_t> size() const { return { 25, 80 }; }
    Char const& chr(size_t y, size_t x) const { return buffer_.at(y).at(x); }
    std::pair<int, int> const& cursor() const { return cursor_; }

private:
    std::array<std::array<Char, 80>, 25> buffer_ {};
    std::pair<int, int> cursor_ { 0, 0 };
};

#endif
