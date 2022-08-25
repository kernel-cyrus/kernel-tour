# string

/lib/string.c

## strcpy

`strcpy`

可能导致buf overflow。

`strncpy`

strncpy会向buf填入n个字符，需要memset这块buf为0，n传入sizeof(buf)-1，预留出\0的空间。

`strlcpy`

strlcpy会向buf填入n个字符，且第n个字符会填入\0，这就不需要在buf中预留最后一个\0的空间。

`strscpy`

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