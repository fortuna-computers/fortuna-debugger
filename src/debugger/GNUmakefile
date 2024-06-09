#
# dependencies
#

K := $(if $(shell pkg-config --modversion glfw3),,$(error "glfw3 pkg-config file not found - please install GLFW"))
K := $(if $(shell pkg-config --modversion lua),,$(error "lua pkg-config file not found - please install Lua 5.4"))

#
# directories
#

LIBRARY  := ../library
PROTOBUF := ${LIBRARY}/protobuf
CONTRIB  := ../contrib
NANOPB   := ${CONTRIB}/nanopb

#
# flags
#

CPPFLAGS += -MMD -Wall -Wextra $(shell pkg-config --cflags glfw3 lua protobuf) \
			-I. -I${LIBRARY}/server -I${LIBRARY}/client -I${LIBRARY}/common -I${NANOPB} \
			-I${CONTRIB}/imgui -I${CONTRIB}/imgui/backends -I${CONTRIB}/glfw/include -I${CONTRIB}/glad/include \
			-I${CONTRIB}/imgui-filebrowser -I${CONTRIB}/libtmt
CXXFLAGS += -std=c++20
LDFLAGS += $(shell pkg-config --libs glfw3 lua protobuf)

#
# sources
#

CONTRIB_SRC = \
        ${CONTRIB}/imgui/imgui.o \
        ${CONTRIB}/imgui/imgui_demo.o \
        ${CONTRIB}/imgui/imgui_draw.o \
        ${CONTRIB}/imgui/imgui_tables.o \
        ${CONTRIB}/imgui/imgui_widgets.o \
        ${CONTRIB}/imgui/backends/imgui_impl_glfw.o \
        ${CONTRIB}/imgui/backends/imgui_impl_opengl3.o \
        ${CONTRIB}/glad/src/gl.o \
        ${CONTRIB}/libtmt/tmt.o

F_DEBUGGER_SRC = \
        main.o \
        config/config.o \
        filewatcher/filewatcher.o \
        model/model.o \
        model/terminalmodel.o \
        ui/keypress.o \
        ui/properties.o \
        ui/ui.o \
        windows/window.o \
        windows/code.o \
        windows/configuration.o \
        windows/cycles.o \
        windows/demo.o \
        windows/debugtext.o \
        windows/keypress.o \
        windows/mainmenu.o \
        windows/memory.o \
        windows/messagebox.o \
        windows/registers.o \
        windows/startup.o \
        windows/terminal.o \
        windows/uploadrom.o

#
# rules
#

all: f-debugger

${CONTRIB}/libtmt/tmt.o: ${CONTRIB}/libtmt/tmt.c   # just for ignoring warnigs
	gcc -c $^ ${CFLAGS} ${CPPFLAGS} -w -o $@

${LIBRARY}/client/libfdbg-client.a: FORCE
	$(MAKE) -C ${LIBRARY}/client libfdbg-client.a

${LIBRARY}/server/libfdbg-server.a: FORCE
	$(MAKE) -C ${LIBRARY}/server libfdbg-server.a

${F_DEBUGGER_SRC}: ${LIBRARY}/client/libfdbg-client.a ${LIBRARY}/server/libfdbg-server.a

f-debugger: ${F_DEBUGGER_SRC} ${CONTRIB_SRC} ${LIBRARY}/client/libfdbg-client.a ${LIBRARY}/server/libfdbg-server.a
	$(CXX) -rdynamic -o $@ $^ ${LDFLAGS}

#
# special rules
#

-include $(F_DEBUGGER_SRC:.o=.d)

clean:
	rm -f ${CONTRIB_SRC} *.o *.d **/*.o **/*.d f-debugger
.PHONY: clean

FORCE: