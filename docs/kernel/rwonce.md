# rwonce

`READ_ONCE`、`WRITE_ONCE` 是对volatile和编译屏障的封装。

这两个操作宏确保了每次执行这行访问时，避免编译器优化，操作的都是原始内存数据。

这两个接口的使用场景，主要适用于一个线程等待一个由另一个线程设置的变量的场景。

因为从单线程角度，这种情况下这个变量似乎根本不会发生变化，编译器会对这类变量的访问进行优化。

曾经的 `ACCESS_ONCE` 貌似存在一些BUG，现在已经被 `READ_ONCE` 取代了。

有了 `READ_ONCE`、`WRITE_ONCE` ，内核代码中已经完全不需要再声明volatile的变量了。

## Files

```
- /include/asm-generic/rwonce.h
- /arch/arm64/include/asm/rwonce.h
