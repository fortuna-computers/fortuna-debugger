#include "terminalmodel.hh"

#include <cstdio>

TerminalModel::TerminalModel()
{
    auto [lines, columns] = size();
    for (size_t y = 0; y < lines; ++y)
        for (size_t x = 0; x < columns; ++x)
            buffer_.at(y).at(x) = Char { ' ' };

    buffer_[0][0] = Char { 'H' };
    cursor_ = { 0, 0 };
}

void TerminalModel::add_char(char c)
{
    printf("Added char '%c'.\n", c);   // TODO
}

