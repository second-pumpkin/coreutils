this was originally going to be a clone of several gnu coreutils, but I got distracted and made ls fancy. so for now this is an ls project.

---

## lf

lf is ls but bloated. you can specify the format that directory contents will be shown. it'll read the arguments from right to left, and display them in that order. the valid arguments and their corresponding file information are these:

| switch | attribute |
| ------ | --------- |
| n | name |
| m | mode |
| u | owner* |
| g | owning group* |
| s | size in bytes |
| i | inode number |
| k | number of hard links |
| d | residing device id (major/minor) |
| r | device id (major/minor) |
| z | block size |
| b | number of blocks |
| a | last accessed timestamp |
| m | last modification timestamp |
| c | creation timestamp |

<sup>* doesn't actually work yet</sup>

there are also two arguments that are equivalent to multiple arguments:

| switch | arguments | purpose |
| ------ | --------- | ------- |
| l | -mkugsfn | emulates the real ls -l |
| t | -dimkugrszbafcn | shows every single attribute for testing |

---

## notes

- the code is bad

- I will be adding even more options

- yes there will be a configuration file

- 🏳️‍🌈🏳️‍🌈🏳️‍🌈🏳️‍🌈🏳️‍🌈🏳️‍🌈
