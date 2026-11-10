CC = clang
CFLAGS = -Wall -Wextra -std=c11

compci: compci.c
	$(CC) $(CFLAGS) -o compci compci.c

test: compci
	./compci tests/valido.ci
	$(CC) -o tests/valido tests/valido.s
	./tests/valido
	@echo "--- testando entrada invalida (deve falhar) ---"
	./compci tests/invalido.ci; test $$? -ne 0 && echo "OK: falhou como esperado"

clean:
	rm -f compci *.o
	rm -f tests/*.s tests/*.o tests/valido

.PHONY: clean test