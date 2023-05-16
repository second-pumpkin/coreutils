CFLAGS = -Wall -Wextra

coreutils : src/lf.c src/true.c
	mkdir bin/
	cc $(CFLAGS) src/false.c -o bin/false
	cc $(CFLAGS) src/lf.c -o bin/lf
	cc $(CFLAGS) src/true.c -o bin/true
install: 
	install -c bin/* /usr/bin/
clean:
	rm -r bin/

