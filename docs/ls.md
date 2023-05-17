## NAME
	ls - A rewrite of GNU ls - list files in a directory

## DESCRIPTION

ls lists files in a directory. you can specify the format in which information about the specified directory's contents are shown. because there are so many possible options, there are two modes of interpretation:

| mode | switch |
| ---- | ------ |
| normal | N |
| layout | L |

when parsing arguments, options will be interpreted left to right. using a mode switch will change the way the program interprets any other arguments. initially, arguments are interpreted in normal mode. below are listed the accepted normal mode arguments:

| switch | effect |
| ------ | ------ |
| v | show version information (I don't version software idk why I added this) |
| h | show help |
| l | print everything on one line |

and these are the valid layout mode options. the order in which these are specified determines the order in which information about the specified directory is shown. by putting a number, the previous option can be modified to display in a different way.

| switch | attribute | variant 1 | variant 2 |
| ------ | --------- | --------- | --------- |
| n | name |  |  |
| m | mode | octal mode |  |
| u | owner | owning uid |  |
| g | owning group | owning gid |  |
| s | size in bytes |  |  |
| i | inode number |  |  |
| k | number of hard links |  |  |
| d | residing device id (major/minor) | residing device id (major) | residing device id (minor) |
| r | device id (major/minor) | device id (major) | device id (minor) |
| z | block size |  |  |
| b | number of blocks |  |  |
| a | last accessed timestamp |  |  |
| m | last modification timestamp |  |  |
| c | creation timestamp |  |  |

there are also two options that are equivalent to multiple others:

| switch | equivalent options | purpose |
| ------ | --------- | ------- |
| l | mkugsfn | emulates the real ls -l |
| t | dd1d2imm1kuu1gg1rr1r2szbafcn | shows every single attribute and variant the program is capable of showing |

---

## EXAMPLES

this program is weird and complicated, so here are some examples of how to use it:

- `ls -v`: show version information
- `ls -Lnnni`: show the current directory's contents' names three times, then their inode number
- `ls -NlLr /dev/`: show the contents of /dev/'s device numbers, all on one line
- `ls -Lm1Nv /etc/': shows version information and terminates. if the Nv part wasn't included, it would print the contents of /etc/'s octal permission numbers.
- `ls -Ldd1d2`: show the id of the device in which the contents of the current direcory reside within. first both the major and the minor id, then the major id, then the minor id.

---

## NOTES

- the code is bad

- I will be adding even more options

- yes there will be a configuration file

- 🏳️‍🌈🏳️‍🌈🏳️‍🌈🏳️‍🌈🏳️‍🌈🏳️‍🌈
