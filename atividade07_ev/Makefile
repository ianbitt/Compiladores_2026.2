CC=clang
CFLAGS=-Wall -Wextra -std=c11 -pedantic -g
all: ev
ev: ev.c
	$(CC) $(CFLAGS) ev.c -o ev
testes: ev
	@for f in testes/*.ev; do echo "=== $$f ==="; ./ev "$$f" || true; done
clean:
	rm -f ev testes/*.s
.PHONY: all testes clean
