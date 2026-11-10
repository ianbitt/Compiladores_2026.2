CC = clang
CFLAGS = -Wall -Wextra -std=c11

ec1: ec1.c
	$(CC) $(CFLAGS) -o ec1 ec1.c

test: ec1
	chmod +x tests/run_tests.sh
	./tests/run_tests.sh

clean:
	rm -f ec1 *.o
	rm -f tests/*.s tests/*.o tests/out_*

.PHONY: clean test
