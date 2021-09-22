CPPFLAGS+=$(shell python-config --cflags --embed)
LDFLAGS+=$(shell python-config --ldflags --embed)
LDLIBS+=$(shell python-config --libs --embed)

simple_python: simple_python.cpp
