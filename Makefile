CFLAGS = -Wall -Wextra
PROGRAMS = ls true false whoami yes

coreutils:
	mkdir -p bin/
	make $(PROGRAMS)
install: 
	install -c $(PROGRAMS) /usr/bin/
clean:
	rm $(PROGRAMS)

ls: src/ls.c
	cc $(CFLAGS) src/ls.c -o ls
true: src/true.c
	cc $(CFLAGS) src/true.c -o true
false: src/false.c
	cc $(CFLAGS) src/false.c -o false
whoami: src/whoami.c
	cc $(CFLAGS) src/whoami.c -o whoami
yes: src/yes.c
	cc $(CFLAGS) src/yes.c -o yes

