this was originally going to be a clone of several gnu coreutils, but I got distracted and made ls fancy. so for now this is an ls project.

---

## lf

lf is ls but bloated. you can specify the format that directory contents will be shown. it'll read the specified options from left to right, and display information about the directory contents in that order. by using a number, you can change the previous option to display as a different variant. the valid options, their variants, and their corresponding file information are these:

| switch | attribute | variant 1 | variant 2 |
| ------ | --------- |
| n | name | - | - |
| m | mode | octal mode | - |
| u | owner | owning uid | - |
| g | owning group | owning gid | - |
| s | size in bytes | - | - |
| i | inode number | - | - |
| k | number of hard links | - | - |
| d | residing device id (major/minor) | residing device id (major) | residing device id (minor) |
| r | device id (major/minor) | device id (major) | device id (minor) |
| z | block size | - | - |
| b | number of blocks | - | - |
| a | last accessed timestamp | - | - |
| m | last modification timestamp | - | - |
| c | creation timestamp | - | - |

there are also two options that are equivalent to multiple others:

| switch | equivalent options | purpose |
| ------ | --------- | ------- |
| l | -mkugsfn | emulates the real ls -l |
| t | -dd1d2imm1kuu1gg1rr1r2szbafcn | shows every single attribute and variant the program is capable of showing |

---

## notes

- the code is bad

- I will be adding even more options

- yes there will be a configuration file

- 🏳️‍🌈🏳️‍🌈🏳️‍🌈🏳️‍🌈🏳️‍🌈🏳️‍🌈

---

```
mkdir ':3'
touch ':3'/':)'
lf ':3' -nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn
```
