#ifndef IEVENTS_HH_
#define IEVENTS_HH_

class ITerminal {
public:
    ~ITerminal() = default;

    virtual void add_char(char c) = 0;
};

#endif