#include "terminalmodel.hh"
#include "model.hh"

#include <cstdio>

extern "C" {
#include <tmt.h>
}

TerminalModel::~TerminalModel()
{
    if (vt_)
        tmt_clean(vt_);
}

void TerminalModel::initialize(size_t columns, size_t lines)
{
    mode_ = (TerminalModel::Mode) model.config().get_int("terminal_mode");
    new_line_ = (TerminalModel::NewLine) model.config().get_int("terminal_newline");

    columns_ = columns;
    lines_ = lines;

    buffer_.resize(lines);
    for (size_t y = 0; y < lines; ++y) {
        buffer_.at(y).resize(columns);
        for (size_t x = 0; x < columns; ++x)
            buffer_.at(y).at(x) = Char { ' ' };
    }

    vt_ = tmt_open(lines, columns, [](tmt_msg_t m, TMT *vt, const void *a, void *ptr) {

        auto tmodel = (TerminalModel *) ptr;

        switch (m) {
            case TMT_MSG_UPDATE: {
                auto screen = (const TMTSCREEN*) a;
                for (size_t y = 0; y < screen->nline; ++y)
                    if (screen->lines[y]->dirty)
                        for (size_t x = 0; x < screen->ncol; ++x)
                            tmodel->buffer_.at(y).at(x).c = (char) screen->lines[y]->chars[x].c;
                tmt_clean(vt);
                break;
            }
            case TMT_MSG_MOVED: {
                auto cursor = (const TMTPOINT *) a;
                tmodel->cursor_ = { cursor->c, cursor->r };
                tmt_clean(vt);
                break;
            }
            default: break;
        }

    }, this, nullptr);
    if (!vt_)
        throw std::runtime_error("Could not allocate terminal");
}

void TerminalModel::add_string(std::string const& str)
{
    if (model.terminal_model().mode_ == M_ANSI) {
        tmt_write(vt_, str.c_str(), 0);
    } else {
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
            if (cursor_.y >= lines_) {
                scroll_up();
                --cursor_.y;
            }
        }
    }
}

void TerminalModel::clear()
{
    initialize(columns_, lines_);
}

void TerminalModel::reset()
{
    if (model.terminal_model().mode_ == M_ANSI)
        tmt_reset(vt_);
}

void TerminalModel::scroll_up()
{
    for (size_t y = 1; y < lines_; ++y)
        buffer_.at(y-1) = buffer_.at(y);
    for (size_t x = 0; x < columns_; ++x)
        buffer_.at(lines_ - 1).at(x) = Char { ' ' };
}

void TerminalModel::set_mode(Mode mode)
{
    mode_ = mode;
    model.config().set("terminal_mode", (int) mode);
    model.config().save();
}

void TerminalModel::set_new_line(NewLine new_line)
{
    new_line_ = new_line;
    model.config().set("terminal_newline", (int) new_line);
    model.config().save();
}
