#ifndef UPLOAD_ROM_HH_
#define UPLOAD_ROM_HH_

#include "window.hh"

class UploadROM : public Window {
public:
    using Window::Window;

    void draw() override;
    std::string name() const override { return "upload_rom"; }
};

#endif