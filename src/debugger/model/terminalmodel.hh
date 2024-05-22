#ifndef TERMINALMODEL_HH_
#define TERMINALMODEL_HH_

#include "client/ievents.hh"

class TerminalModel : public ITerminal {
public:
    void add_char(char c) override;
};

#endif
