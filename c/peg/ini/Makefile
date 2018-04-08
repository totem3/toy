all: clean iniparse

debug: clean iniparse.c
	gcc -O0 -g3 -oiniparse main.c

clean:
	rm -f iniparse.c iniparse

iniparse.c: iniparse.leg
	leg -oiniparse.c iniparse.leg

iniparse: iniparse.c
	gcc -oiniparse main.c

test:
	echo "foo=bar" | ./iniparse && echo NG || echo OK
