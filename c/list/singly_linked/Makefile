list.o: list.c
	clang -c -g -o list.o list.c

.PHONY: test_build
test_build: list.o
	clang -g -L/usr/local/lib -I/usr/local/include/CUnit -lcunit -o test test.c list.o

.PHONY: after_test
after_test:
	rm -f test

.PHONY: test_exec
test_exec:
	./test

.PHONY: test
test: test_build test_exec after_test
