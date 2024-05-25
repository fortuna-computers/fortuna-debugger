#include "terminalmodel.hh"

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

    buffer_.at(0).at(0) = Char { 'H' };
    cursor_ = { 0, 0 };
}

void TerminalModel::add_char(char c)
{
    printf("Added char '%c'.\n", c);   // TODO
}

