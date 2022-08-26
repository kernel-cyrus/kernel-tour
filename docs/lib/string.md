# string

/lib/string.c

## strcpy

`strcpy`

可能导致buf overflow。

返回dest的指针

`strncpy`

strncpy会向buf填入n个字符，需要memset这块buf为0，n传入sizeof(buf)-1，预留出\0的空间。

返回dest的指针

`strlcpy`

strlcpy会向buf填入n个字符，且第n个字符会填入\0，这就不需要在buf中预留最后一个\0的空间，n传入sizeof(buf)即可。

返回想要合并出的字符串大小，返回值可能大于bufsize，但是不用个担心溢出问题。

`strscpy`

参数与strlcpy相同，未溢出时返回strlen(dest)，溢出时返回-E2BIG

## strlen

`strlen`

`strnlen`

## strcat

`strcat`

`strlcat`

## strcmp

`strcmp`

`strncmp`

## strchr

`strchr`

`strrchr`

`strnchr`

## strsep

`strsep`

## strstr

`strstr`

`strnstr`

## memset

`memset`

`memset16`

`memset32`

`memset64`

## memcpy

`memcpy`

`memmove`

## memchr

`memchr`

## Test

[test-string](https://github.com/kernel-cyrus/kernel-tour/tree/master/tests/test-string)

## Reference

[Ushering out strlcpy\(\)](https://lwn.net/Articles/905777/)