CFLAGS=-std=c11 -g

vcc:  main.o tokenizer.o
	${CC} -o $@ $? ${LDFLAGS}

clean:
	rm -f vcc *.o *~ tmp*

format:
	find . \(-name "*.c" -o -name "*.h"\) -exec clang-format -i {} \;

.PHONY: clean format