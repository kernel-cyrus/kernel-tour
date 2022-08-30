# lockdep

死锁检测

CONFIG_PROVE_LOCKING

This feature enables the kernel to report locking related deadlocks before they actually occur. For more details, see Documentation/locking/lockdep-design.txt.

CONFIG_DEBUG_LOCK_ALLOC

Detect incorrect freeing of live locks.

CONFIG_DEBUG_LOCKDEP

The lock dependency engine will do additional runtime checks to debug itself, at the price of more runtime overhead.

CONFIG_LOCK_STAT

Lock usage statistics. For more details, see Documentation/locking/lockstat.txt

CONFIG_DEBUG_LOCKING_API_SELFTESTS

The kernel to run a short self-test during bootup in start_kernel(). The self-test checks whether common types of locking bugs are detected by debugging mechanisms or not. For more details, see lib/locking-selftest.c

CONFIG_LOCKUP_DETECTOR

CONFIG_HARDLOCKUP_DETECTOR_OTHER_CPU

CONFIG_HARDLOCKUP_DETECTOR

CONFIG_DEBUG_ATOMIC_SLEEP