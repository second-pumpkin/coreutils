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
#include <limits.h>
#include <pwd.h>
#include <grp.h>

void get_dir_info(char *pathname);
void fill_ent(char *pathname, char **outbuf);
void print_dir_columns(char ***outbuf, int ents);
void print_dir(char ***outbuf, int ents);
void register_opt(char opt);
void set_opt_variant(char variant);
void register_opts(char *optstr);
void handle_opt_normal(char opt);
void handle_opt_layout(char opt);
void get_mode(char *buf, int len, mode_t mode);
void get_user(char *buf, int len, uid_t uid);
void get_group(char *buf, int len, gid_t gid);
void get_timestamp(char *buf, int len, struct timespec timestamp);

/* attributes a directory entry can have and can be shown by ls */
/* I am addicted to bitwise bullshit */
/* the least significant byte represents the attribute (and corresponds to the actual command switch) */
/* the next byte represents its variant */
enum fstats {
	NAME          = 'n',				/* name */
	ST_MODE       = 'm',				/* mode */
	OMODE         = 'm' | ('1' << CHAR_BIT),	/* octal mode */
	USER          = 'u',				/* owning user */
	ST_UID        = 'u' | ('1' << CHAR_BIT),	/* owning uid */
	GROUP         = 'g',				/* owning group */
	ST_GID        = 'g' | ('1' << CHAR_BIT),	/* owning gid */
	ST_DEV        = 'd',				/* residing device id (major/minor) */
	DEV_MAJOR     = 'd' | ('1' << CHAR_BIT),	/* residing device id (major) */
	DEV_MINOR     = 'd' | ('2' << CHAR_BIT),	/* residing device id (minor) */
	ST_RDEV       = 'r',				/* device id (major/minor) */
	RDEV_MAJOR    = 'r' | ('1' << CHAR_BIT),	/* device id (major) */
	RDEV_MINOR    = 'r' | ('2' << CHAR_BIT),	/* device id (minor) */
	ST_NLINK      = 'k',				/* no. of hard links */
	ST_INO        = 'i',				/* inode number */
	ST_SIZE       = 's',				/* size in bytes */
	ST_BLKSIZE    = 'z',				/* block size */
	ST_BLOCKS     = 'b',				/* no. of blocks */
	ST_ATIM       = 'a',				/* last accessed timestamp */
	ST_MTIM       = 'f',				/* last modified timestamp */
	ST_CTIM       = 'c',				/* creation timestamp */
};

/* an array determining which order these attributes will be shown, set up when the arguments are parsed */
int *layout = NULL;
int layout_len = 0;

/* struct for general program options */
struct {
	bool separate_ents;
} program_args = {
	true,
};

/* get information about the contents of the directory */
void get_dir_info(char *pathname) {
	DIR *dir;
	struct dirent *ent;
	char ***outbuf = NULL;
	int ents = 0;

	/* open the directory */
	errno = 0;
	if (!(dir = opendir(pathname))) {
		/* if (errno == ENOTDIR)
			TODO */
		err(1, NULL);
	}
	
	/* add the directory entries to the output buffer array thing, resizing it as needed */
	while ( (ent = readdir(dir) ) ) {
		if ((ents % 16) == 0)
			if (!(outbuf = realloc(outbuf, sizeof outbuf * (ents + 16))))
				err(1, NULL);
		
		if (!(outbuf[ents] = malloc(sizeof outbuf[ents] * layout_len)))
			err(1, NULL);
		fill_ent(ent->d_name, outbuf[ents]);
		ents++;
	}
	
	if (program_args.separate_ents)
		print_dir_columns(outbuf, ents);
	else
		print_dir(outbuf, ents);
}

/* print the directory entries with nice spacing :) */
void print_dir_columns(char ***outbuf, int ents) {
	int column_maxlens[layout_len], column_len;

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
			printf("%-*s ", column_maxlens[col], outbuf[row][col]);
		printf("\n");
	}
}

void print_dir(char ***outbuf, int ents) {
	for (int row = 0; row < ents; row++)
		for (int col = 0; col < layout_len; col++)
			printf("%s ", outbuf[row][col]);
	printf("\n");
}

/* fill the array of strings passed in with strings containing file information determined by the layout */
void fill_ent(char *pathname, char **outbuf) {
	const int ELEM_SIZE = 32;
	struct stat stats;

	stat(pathname, &stats);

	for (int i = 0; i < layout_len; i++) {
		outbuf[i] = malloc(ELEM_SIZE);
		switch (layout[i]) {
			/* sorry */
			case NAME:		snprintf(outbuf[i], ELEM_SIZE, "%s", pathname);
						break;
			case ST_DEV:		snprintf(outbuf[i], ELEM_SIZE, "%d/%d", major(stats.st_dev), minor(stats.st_dev));
						break;
			case DEV_MAJOR:		snprintf(outbuf[i], ELEM_SIZE, "%d", major(stats.st_dev));
						break;
			case DEV_MINOR:		snprintf(outbuf[i], ELEM_SIZE, "%d", minor(stats.st_dev));
						break;
			case ST_INO:		snprintf(outbuf[i], ELEM_SIZE, "%li", stats.st_ino);
						break;
			case ST_MODE:		get_mode(outbuf[i], ELEM_SIZE, stats.st_mode);
						break;
			case OMODE:		snprintf(outbuf[i], ELEM_SIZE, "%o", stats.st_mode & 0777);
						break;
			case ST_NLINK:		snprintf(outbuf[i], ELEM_SIZE, "%li", stats.st_nlink);
						break;
			case USER:		get_user(outbuf[i], ELEM_SIZE, stats.st_uid);
						break;
			case ST_UID:		snprintf(outbuf[i], ELEM_SIZE, "%d", stats.st_uid);
						break;
			case GROUP:		get_group(outbuf[i], ELEM_SIZE, stats.st_gid);
						break;
			case ST_GID:		snprintf(outbuf[i], ELEM_SIZE, "%d", stats.st_gid);
						break;
			case ST_RDEV:		snprintf(outbuf[i], ELEM_SIZE, "%d/%d", major(stats.st_rdev), minor(stats.st_rdev));
						break;
			case RDEV_MAJOR:	snprintf(outbuf[i], ELEM_SIZE, "%d", major(stats.st_rdev));
						break;
			case RDEV_MINOR:	snprintf(outbuf[i], ELEM_SIZE, "%d", minor(stats.st_rdev));
						break;
			case ST_SIZE:		snprintf(outbuf[i], ELEM_SIZE, "%li", stats.st_size);
						break;
			case ST_BLKSIZE:	snprintf(outbuf[i], ELEM_SIZE, "%li", stats.st_blksize);
						break;
			case ST_BLOCKS:		snprintf(outbuf[i], ELEM_SIZE, "%li", stats.st_blocks);
						break;
			case ST_ATIM:		get_timestamp(outbuf[i], ELEM_SIZE, stats.st_atim);
						break;
			case ST_MTIM:		get_timestamp(outbuf[i], ELEM_SIZE, stats.st_mtim);
						break;
			case ST_CTIM:		get_timestamp(outbuf[i], ELEM_SIZE, stats.st_ctim);
						break;
			default:
				errx(1, "invalid layout option '%c'", layout[i]);
		}
	}
}

/* get an entry's mode in a good format */
void get_mode(char *buf, int len, mode_t mode) {
	char str[] = "-------------";
	
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

/* get the name of the user uid passed in */
void get_user(char *buf, int len, uid_t uid) {
	struct passwd *pw;
	
	if (!(pw = getpwuid(uid)))
		/* getpwuid kinda sucks so just put the uid in if it fails instead of actually dealing with the error */
		snprintf(buf, len, "%d", uid);
	else
		strncpy(buf, pw->pw_name, len);
}

/* get the name of the group gid passed in */
void get_group(char *buf, int len, gid_t gid) {
	struct group *gr;
	
	if (!(gr = getgrgid(gid)))
		snprintf(buf, len, "%d", gid);
	else
		strncpy(buf, gr->gr_name, len);
}

/* get a timestamp in a good format */
void get_timestamp(char *buf, int len, struct timespec timestamp) {
	char time[32];
	strftime(time, sizeof time, "%c", localtime(&timestamp.tv_sec));
	strncpy(buf, time, len);
}

/* add opt to the layout, reallocating as necessary */
void register_opt(char opt) {
	/* does opt specify a variant? */
	if (opt <= '9' && opt >= '0') {
		set_opt_variant(opt);
		return;
	}
	
	/* it does not */
	if (layout_len % 16 == 0)
		if (!(layout = realloc(layout, sizeof(int) * (layout_len + 16))))
			err(1, NULL);
	
	layout[layout_len] = (int) opt;
	layout_len++;
}

/* set the variant of the most recently registered layout option */
void set_opt_variant(char variant) {
	if (layout_len == 0)
		errx(1, "can't set mode of previous argument to %c, as there is no previous argument.", variant);

	layout[layout_len - 1] |= variant << CHAR_BIT;
}

/* register all the opt characters in optstr to the layout */
void register_opts(char *optstr) {
	do {
		register_opt(*optstr);
	} while (*++optstr);
}

/* handle a layout mode opt */
void handle_layout_opt(char opt) {
	switch (opt) {
		case 'l': register_opts("mkugsfn");
			  return;
		case 't': register_opts("dd1d2imm1kuu1gg1rr1r2szbafcn");
			  return;
		default:
			  register_opt(opt);
	}
}

/* handle a normal mode opt */
void handle_normal_opt(char opt) {
	switch (opt) {
		case 'v':
			printf("lf version (current version). written by lena.\n");
			exit(0);
		case 'h':
			printf("dude it's like so complicated. check the manual please\n");
			exit(0);
		case 'l':
			program_args.separate_ents = false;
			break;
		default:
			errx(1, "invalid normal argument '%c'", opt);
	}
}

/* main (I only added this comment to visually separate this function from the previous one) */
int main(int argc, char** argv) {
	char *pathname = ".";
	char opt;
	char mode = 'N'; /* normal mode */

	/* parse arguments and set up the layout array */
	opterr = 0;
	while (1) {
		opt = getopt(argc, argv, "NLabcdefghijklmnopqrstuvwxyz0123456789"); /* if anyone knows how to use getopt without the optstring please tell me how */
		if (opt == -1)
			break;
		if (opt == '?')
			errx(1, "invalid argument '%c'", optopt);

		/* is opt a mode character? */
		switch (opt) {
			case 'N':
			case 'L':
				mode = opt;
				continue;
		}
		
		/* opt is a mode specific switch */
		switch (mode) {
			case 'N': handle_normal_opt(opt);
				  break;
			case 'L': handle_layout_opt(opt);
				  break;
		}
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
	get_dir_info(pathname);

	return 0;
}

