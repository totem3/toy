avltree.o:
	clang -c -g -oavltree.o avltree.c

.PHONY: test
test: test_build after_test

.PHONY: test_build
test_build: avltree.o
	clang -g -L/usr/local/lib -I/usr/local/include/CUnit -lcunit -o test avltree.o test.c
	./test

.PHONY: after_test
after_test:
	rm -f test
