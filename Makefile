CLANG:=$(shell command -v clang 2> /dev/null)

ifdef CLANG
CC = clang
CXX = clang++
else
CC = gcc
CXX = g++
endif
CXXFLAGS=-std=c++17 -g -fsanitize=address -Wall -Wno-unused-function $(shell llvm-config --cxxflags)
CFLAGS=-std=c17 -g -fsanitize=address -Wall -Wno-unused-function
LDFLAGS = -fsanitize=address $(shell llvm-config --ldflags --libs --system-libs)

SRCS=$(wildcard *.c *.cpp)
OBJS_C=$(SRCS:.c=.o)
OBJS=$(OBJS_C:.cpp=.o)

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $^

%.o: %.cpp
	${CXX} ${CXXFLAGS} `llvm-config --cxxflags --ldflags --system-libs --libs core` -c -o $@ $^

vcc: $(OBJS)
	${CXX} -o $@ $^ ${LDFLAGS}

clean:
	rm -f vcc *.o *~ tmp* *.dot *.svg

format:
	find . \( -name "*.c" -o -name "*.h" -o -name "*.cpp" -o -name "*.hpp"\) -exec clang-format -i {} \;

.PHONY: clean format