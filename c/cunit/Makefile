.PHONY: test
test: test_build after_test

.PHONY: test_build
test_build:
	clang -L/usr/local/lib -I/usr/local/include/CUnit -lcunit -o test test.c
	./test

.PHONY: after_test
after_test:
	rm -f test
