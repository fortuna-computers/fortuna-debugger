cmake_minimum_required(VERSION 3.27)

project(
	fortuna-debugger
	VERSION 1.0
	LANGUAGES C CXX)

add_library(libfdbg SHARED
		machine.c
		machine.h
		libfdbg/server.c
		libfdbg/server.h
)