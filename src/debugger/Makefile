BASE=../..

include ${BASE}/defs.mk


#
# targets
#

TARGET = f-debugger
all: ${TARGET}

#
# check dependencies
#

K := $(if $(shell pkg-config --modversion glfw3),,$(error "glfw3 pkg-config file not found - please install GLFW"))
K := $(if $(shell pkg-config --modversion lua),,$(error "lua pkg-config file not found - please install Lua 5.4"))

${BASE}/src/library/libfdbg-client.a:
	[ ! -f $@ ] && echo "The library needs to be built first." && exit 1

${BASE}/src/library/libfdbg-server.a:
	[ ! -f $@ ] && echo "The library needs to be built first." && exit 1

#
# flags
#

CPPFLAGS += -I${BASE} -I${BASE}/src/library -I${BASE}/src/library/server -I${BASE}/src/library/client \
			-I${CONTRIB}/imgui -I${CONTRIB}/imgui/backends -I${CONTRIB}/glfw/include -I${CONTRIB}/glad/include \
			-I${CONTRIB}/imgui-filebrowser $(shell pkg-config --cflags glfw3 lua)

LDFLAGS += `pkg-config --libs glfw3 lua`

#
# f-debugger
#

IMGUI_SRC = \
        ${CONTRIB}/imgui/imgui.o \
        ${CONTRIB}/imgui/imgui_demo.o \
        ${CONTRIB}/imgui/imgui_draw.o \
        ${CONTRIB}/imgui/imgui_tables.o \
        ${CONTRIB}/imgui/imgui_widgets.o \
        ${CONTRIB}/imgui/backends/imgui_impl_glfw.o \
        ${CONTRIB}/imgui/backends/imgui_impl_opengl3.o \
        ${CONTRIB}/glad/src/gl.o

F_DEBUGGER_SRC = \
        main.o \
        config/config.o \
        model/model.o \
        ui/properties.o \
        ui/ui.o \
        windows/window.o \
        windows/code.o \
        windows/configuration.o \
        windows/cycles.o \
        windows/demo.o \
        windows/mainmenu.o \
        windows/memory.o \
        windows/messagebox.o \
        windows/registers.o \
        windows/startup.o \
        windows/uploadrom.o \
        ${IMGUI_SRC}

f-debugger: ${F_DEBUGGER_SRC} ${BASE}/src/library/libfdbg-client.a ${BASE}/src/library/libfdbg-server.a
	$(CXX) -rdynamic -o $@ $^ ${PROTOBUF_LDFLAGS} ${LDFLAGS}

-include $(F_DEBUGGER_SRC:.o=.d)

#
# clean
#

clean:
	rm -rf ${TARGET} ${F_DEBUGGER_SRC} $(F_DEBUGGER_SRC:.o=.d)
