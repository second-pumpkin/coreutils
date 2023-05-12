progs := lf
all: $(progs)

lf: lf.c
	gcc lf.c -o lf
