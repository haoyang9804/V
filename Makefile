CFLAGS=-std=c11 -g

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

vcc: $(OBJS)
	${CC} -o $@ $^ ${LDFLAGS}

clean:
	rm -f vcc *.o *~ tmp*

format:
	find . \( -name "*.c" -o -name "*.h" \) -exec clang-format -i {} \;

.PHONY: clean format