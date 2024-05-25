#ifndef TERMINALMODEL_HH_
#define TERMINALMODEL_HH_

#include <vector>
#include <utility>
#include <cstddef>

class TerminalModel {
public:
    struct Char { char c; };
    struct Pos { size_t x, y; };

    void initialize(size_t columns, size_t lines);

    void add_string(std::string const& str);

    constexpr std::pair<size_t, size_t> size() const { return { lines_, columns_ }; }
    Char const& chr(size_t y, size_t x) const { return buffer_.at(y).at(x); }
    Pos const& cursor() const { return cursor_; }

    void clear();

    enum Mode { M_ANSI, M_RAW };
    enum NewLine { NL_CR, NL_LF, NL_CRLF };

    void set_mode(Mode mode) { mode_ = mode; }
    void set_new_line(NewLine new_line) { new_line_ = new_line; }

private:
    size_t columns_ = 0, lines_ = 0;
    std::vector<std::vector<Char>> buffer_ {};
    Pos cursor_ { 0, 0 };
    Mode mode_ = M_ANSI;
    NewLine new_line_ = NL_CR;

    void scroll_up();
};

#endif
