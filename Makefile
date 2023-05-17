CFLAGS = -Wall -Wextra
PROGRAMS = ls true false whoami

coreutils:
	mkdir -p bin/
	make $(PROGRAMS)
install: 
	install -c bin/* /usr/bin/
clean:
	rm -r bin/

ls: src/ls.c
	cc $(CFLAGS) src/ls.c -o bin/ls
true: src/true.c
	cc $(CFLAGS) src/true.c -o bin/true
false: src/false.c
	cc $(CFLAGS) src/false.c -o bin/false
whoami: src/whoami.c
	cc $(CFLAGS) src/whoami.c -o bin/whoami

