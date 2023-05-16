CFLAGS = -Wall -Wextra

coreutils : src/lf.c src/true.c
	cc $(CFLAGS) src/lf.c -o bin/lf
	cc $(CFLAGS) src/true.c -o bin/true
install: 
	install -c bin/* /usr/bin/
clean:
	rm bin/*

