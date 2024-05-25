#include "terminalmodel.hh"
#include "model.hh"

#include <cstdio>

void TerminalModel::initialize(size_t columns, size_t lines)
{
    columns_ = columns;
    lines_ = lines;

    buffer_.resize(lines);
    for (size_t y = 0; y < lines; ++y) {
        buffer_.at(y).resize(columns);
        for (size_t x = 0; x < columns; ++x)
            buffer_.at(y).at(x) = Char { ' ' };
    }
}

void TerminalModel::add_string(std::string const& str)
{
    if (model.terminal_model().mode_ == M_RAW) {
        for (char c : str) {
            if (c >= 32 && c != 127) {
                buffer_.at(cursor_.y).at(cursor_.x++) = Char { c };
            } else if (c == '\b' && cursor_.x > 0) {
                --cursor_.x;
            } else if ((c == 13 && new_line_ == NewLine::NL_CR)
                    || (c == 10 && new_line_ == NewLine::NL_LF)) {
                ++cursor_.y;
                cursor_.x = 0;
            } else if (c == 13 && new_line_ == NewLine::NL_CRLF) {
                cursor_.x = 0;
            } else if (c == 10 && new_line_ == NewLine::NL_CRLF) {
                ++cursor_.y;
            }

            if (cursor_.x >= columns_) {
                ++cursor_.y;
                cursor_.x = 0;
            }
            if (cursor_.y >= lines_)
                scroll_up();
        }
    }
}

void TerminalModel::clear()
{
    initialize(columns_, lines_);
}

void TerminalModel::scroll_up()
{
    // TODO
}
