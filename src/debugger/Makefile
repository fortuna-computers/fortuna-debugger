BASE=../..

include ${BASE}/defs.mk


#
# flags
#

CPPFLAGS += -I${BASE} -I${BASE}/src/library -I${BASE}/src/library/server -I${BASE}/src/library/client \
			-I${CONTRIB}/imgui -I${CONTRIB}/imgui/backends -I${CONTRIB}/glfw/include -I${CONTRIB}/glad/include \
			-I${CONTRIB}/imgui-filebrowser

CPPFLAGS += -I/opt/homebrew/Cellar/glfw/3.3.9/include

LDFLAGS += `pkg-config --libs glfw3`

#
# targets
#

TARGET = f-debugger
all: ${TARGET}

#
# check dependencies
#

K := $(if $(shell pkg-config --modversion glfw3),,$(error "glfw3 pkg-config file not found - please install GLFW"))

${BASE}/src/library/libfdbg-client.a:
	[ ! -f $@ ] && echo "The library needs to be built first." && exit 1

${BASE}/src/library/libfdbg-server.a:
	[ ! -f $@ ] && echo "The library needs to be built first." && exit 1

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
        config.o \
        load.o \
        model.o \
        main.o \
        ui.o \
        emulator/emulator.o \
        windows/demo.o \
        windows/memory.o \
        windows/messagebox.o \
        windows/startup.o \
        ${IMGUI_SRC}

f-debugger: ${F_DEBUGGER_SRC} ${BASE}/src/library/libfdbg-client.a ${BASE}/src/library/libfdbg-server.a
	$(CXX) -o $@ $^ ${PROTOBUF_LDFLAGS} ${LDFLAGS}

-include $(F_DEBUGGER_SRC:.o=.d)

#
# clean
#

clean:
	rm -rf ${CONTRIB}/glfw/build ${TARGET} ${F_DEBUGGER_SRC} $(F_DEBUGGER_SRC:.o=.d)
