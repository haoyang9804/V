# CLANG:=$(shell command -v clang 2> /dev/null)

# ifdef CLANG
# CC = clang
# else
# CC = gcc
# endif

CFLAGS=-std=c11 -g -fsanitize=address
LDFLAGS = -fsanitize=address

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

vcc: $(OBJS)
	${CC} -o $@ $^ ${LDFLAGS}

clean:
	rm -f vcc *.o *~ tmp* *.dot *.svg

format:
	find . \( -name "*.c" -o -name "*.h" \) -exec clang-format -i {} \;

.PHONY: clean format