# sprintf

内核中由以下几种sprintf函数

`sprintf`

向buf中写入字符串，返回strlen，因为没有传入bufsize，可能导致buf overflow。

`snprintf`

向buf中写入字符串，返回应写入的字符数（可能大于bufsize），由于传入了bufsize，不会overflow。

`scnprintf`

向buf中写入字符串，返回strlen，由于传入了bufsize，不会overflow。

## Sample Test

```
char buf[10];
strlen("123456789");			# return 9, without counting '\0'
sprintf(buf, "123456789");		# return 9, without counting '\0'
sprintf(buf, "1234567890");		# build time error: overflow
snprintf(buf, "1234567890");		# return 10, buf="123456789"
scnprintf(buf, "1234567890");		# return 9, buf="123456789"
```