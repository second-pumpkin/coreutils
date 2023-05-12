#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <time.h>
#include <string.h>
#include <err.h>

int print_dir(char *pathname);
int print_ent(char *dirname, int dirname_len, char *pathname, char **outbuf);
void register_opt(char opt);
void register_opts(char *optstr);
void print_mode(char *buf, int len, mode_t mode);
void print_uid(char *buf, int len, uid_t uid);
void print_gid(char *buf, int len, gid_t gid);
void print_timestamp(char *buf, int len, struct timespec timestamp);

/* attributes a directory entry can have and can be shown by ls */
enum fstats {
	ST_DEV        = 'd',
	ST_INO        = 'i',
	ST_MODE       = 'm',
	ST_NLINK      = 'k',
	ST_UID        = 'u',
	ST_GID        = 'g',
	ST_RDEV       = 'r',
	ST_SIZE       = 's',
	ST_BLKSIZE    = 'z',
	ST_BLOCKS     = 'b',
	ST_ATIM       = 'a',
	ST_MTIM       = 'f',
	ST_CTIM       = 'c',
	NAME          = 'n',
};

/* an array determining which order these attributes will be shown, set up when the arguments are parsed */
char *layout = NULL;
int layout_len = 0;

/* print the contents of the directory */
int print_dir(char *pathname) {
	DIR *dir;
	struct dirent *ent;
	int pathname_len = strlen(pathname);
	char ***outbuf = NULL;
	int ents = 0;
	int column_maxlens[layout_len], column_len;

	/* open the directory */
	errno = 0;
	if (!(dir = opendir(pathname))) {
		if (errno == ENOTDIR)
			/* TODO */
			return 1;
		err(1, NULL);
	}
	
	/* add the directory entries to the output buffer array thing, resizing it as needed */
	while (ent = readdir(dir)) {
		if (ents % 16 == 0)
			if (!(outbuf = realloc(outbuf, sizeof outbuf * (ents + 16))))
				err(1, NULL);
		
		if (!(outbuf[ents] = malloc(sizeof outbuf[ents] * layout_len)))
			err(1, NULL);
		print_ent(pathname, pathname_len, ent->d_name, outbuf[ents]);
		ents++;
	}

	/* print the entries with nice spacing :) */
	/* first calculate the spacings */
	for (int col = 0; col < layout_len; col++) {
		column_maxlens[col] = 0;
		for (int row = 0; row < ents; row++) {
			column_len = strlen(outbuf[row][col]);
			if (column_len > column_maxlens[col])
				column_maxlens[col] = column_len;
		}
	}
	
	/* now print stuff */
	for (int row = 0; row < ents; row++) {
		for (int col = 0; col < layout_len; col++)
			printf("%-*s", column_maxlens[col] + 1, outbuf[row][col]);
		printf("\n");
	}

	return 0;

}

/* TODO: come up with a better name and comment for this function */
/* put an entry's text into the buffer passed in  */
int print_ent(char *dirname, int dirname_len, char *pathname, char **outbuf) {
	const int ELEM_SIZE = 64;
	struct stat stats;
	char *full_pathname;
	
	/* contruct the file's full pathname */

	stat(pathname, &stats);

	for (int i = 0; i < layout_len; i++) {
		outbuf[i] = malloc(ELEM_SIZE);
		switch (layout[i]) {
			/* sorry */
			case NAME:		snprintf(outbuf[i], ELEM_SIZE, "%s", pathname);
						break;
			case ST_DEV:		snprintf(outbuf[i], ELEM_SIZE, "%d/%d", major(stats.st_dev), minor(stats.st_dev));
						break;
			case ST_INO:		snprintf(outbuf[i], ELEM_SIZE, "%d", stats.st_ino);
						break;
			case ST_MODE:		print_mode(outbuf[i], ELEM_SIZE, stats.st_mode);
						break;
			case ST_NLINK:		snprintf(outbuf[i], ELEM_SIZE, "%d", stats.st_nlink);
						break;
			case ST_UID:		print_uid(outbuf[i], ELEM_SIZE, stats.st_uid);
						break;
			case ST_GID:		print_gid(outbuf[i], ELEM_SIZE, stats.st_gid);
						break;
			case ST_RDEV:		snprintf(outbuf[i], ELEM_SIZE, "%d/%d", major(stats.st_rdev), minor(stats.st_rdev));
						break;
			case ST_SIZE:		snprintf(outbuf[i], ELEM_SIZE, "%d", stats.st_size);
						break;
			case ST_BLKSIZE:	snprintf(outbuf[i], ELEM_SIZE, "%d", stats.st_blksize);
						break;
			case ST_BLOCKS:		snprintf(outbuf[i], ELEM_SIZE, "%d", stats.st_blocks);
						break;
			case ST_ATIM:		print_timestamp(outbuf[i], ELEM_SIZE, stats.st_atim);
						break;
			case ST_MTIM:		print_timestamp(outbuf[i], ELEM_SIZE, stats.st_mtim);
						break;
			case ST_CTIM:		print_timestamp(outbuf[i], ELEM_SIZE, stats.st_ctim);
						break;
		}
	}

	return 0;
}

/* print an entry's mode in a good format */
void print_mode(char *buf, int len, mode_t mode) {
	char str[] = "-------------";
	int i;
	
	/* first get the file type */
	switch (mode & S_IFMT) {
		case S_IFSOCK: str[0] = 's';
			       break;
		case S_IFLNK:  str[0] = 'l';
			       break;
		case S_IFBLK:  str[0] = 'b';
			       break;
		case S_IFDIR:  str[0] = 'd';
			       break;
		case S_IFCHR:  str[0] = 'c';
			       break;
		case S_IFIFO:  str[0] = 'f';
			       break;
	}

	/* then determine the three weird bits */
	if (mode & S_ISUID) str[1] = 'u';
	if (mode & S_ISGID) str[2] = 'g';
	if (mode & S_ISVTX) str[3] = 's';

	/* now do the actual permissions (characters 4-13 in the mode string) */
	for (int i = 4; i < 13; i++) { 
		if ((0400 >> (i-4)) & mode) {
			if (i % 3 == 1) str[i] = 'r';
			if (i % 3 == 2) str[i] = 'w';
			if (i % 3 == 0) str[i] = 'x';
		}
	}

	strncpy(buf, str, len);
}

/* get and print the name of the user uid passed in */
void print_uid(char *buf, int len, uid_t uid) {
	strncpy(buf, "uid", len);
}

/* get and print the name of the group gid passed in */
void print_gid(char *buf, int len, gid_t gid) {
	strncpy(buf, "gid", len);
}

/* print a timestamp in a good format */
void print_timestamp(char *buf, int len, struct timespec timestamp) {
	char time[32];
	strftime(time, sizeof time, "%c", localtime(&timestamp.tv_sec));
	strncpy(buf, time, len);
}

/* add opt to the layout, reallocating as necessary */
void register_opt(char opt) {
	if (layout_len % 16 == 0)
		if (!(layout = realloc(layout, layout_len + 16)))
			err(1, NULL);
	
	layout[layout_len] = opt;
	layout_len++;
}

/* register all the opt characters in optstr to the layout */
void register_opts(char *optstr) {
	do {
		register_opt(*optstr);
	} while (*++optstr);
}

/* main (I only added this comment to visually separate this function from the previous one) */
int main(int argc, char** argv) {
	char *pathname = ".";
	int path_len;
	char opt;

	/* parse arguments and set up the layout array */
	opterr = 0;
	while (1) {
		opt = getopt(argc, argv, "ltdimkugrszbafcn");
		if (opt == -1)
			break;
		/* is the opt one of the normal ones? */
		switch (opt) {
			case '?':
				fprintf(stderr, "invalid argument '%c'\n", optopt);
				exit(1);
			case 'l':
				register_opts("mkugsfn");
				continue;
			case 't':
				register_opts("dimkugrszbafcn");
				continue;
		}
		/* opt is a layout switch */
		register_opt(opt);
	}

	/* default only show entry names */
	if (!layout)
		register_opt('n');
	
	/* the last arguments not processed are directories to scan */
	if (optind < argc) {
		if (chdir(argv[optind]))
			err(1, NULL);
	}

	/* print the contents of the directory */
	return print_dir(pathname);
}

