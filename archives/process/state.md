# process state

Process是进程管理和调度的基本单位，在内核中用task_struct表示。

Linux内核中没有线程概念，或者说没有任何线程相关的数据结构，内核线程实际是一个特殊的Process。

## States

### Running State

进程的运行状态保存在 `tsk->state` 中，在 `sched.h` 文件中定义

#### (R) Running or Runnable (TASK_RUNNING)

进程创建时被设置为RUNNING，不论是否在进程队列中ready to run，还是真正正在RUNNING，都标识为TASK_RUNNING

#### (D) Uninterruptible Sleep (TASK_UNINTERRUPTIBLE)

进程运行过程中，可能需要等待IO操作完成，这时需要放弃CPU运行，在等待操作完成前，进程被设置为TASK_UNINTERRUPTIBLE。

这类情况大部分在等IO，不允许被Siganl唤醒，必须等到IO完成。

\* (D)表示Disk Sleep

#### (S) Interruptable Sleep (TASK_INTERRUPTIBLE)

与(D)类似，但是(S)可以被SIGNAL唤醒。

一般情况下为Timer Sleep或者wait for signal。

\* (S)表示Sleep

#### (T) Stopped (TASK_STOPPED)

被暂停的进程。

通过SIGSTOP信号来暂停进程运行，暂停的进程可以通过SIGCONT来重新继续运行。

#### (t) Traced (TASK_TRACED)

进程正在被debugger调试，比如ptrace。

#### (I) Idle (TASK_UINTERRUPTIBLE + TASK_NOLOAD)

Task Idle可以简单认为就是Task Uninterruptble。

\* (I)表示Idle

### Exit State

进程退出态保存在 `tsk->exit_state` 中，在 `sched.h` 文件中定义

#### (X) Dead (EXIT_DEAD)

进程正常退出，ps工具中应该永远看不到这个状态，因为进程已经退出了。

#### (Z) Zombie (EXIT_ZOMBIE)

子进程已经退出，但是父进程没有处理子进程退出的signal。

正常使用 `fork` 函数创建子进程，需要使用 `waitpid` 来等待子进程运行结束， `waitpid` 会去处理这个signal，清理子进程资源。

如果父进程没有使用 `waitpid`，在子进程运行结束后，没有人捕获和处理这个signal，子进程就会变成ZOMBIE状态。

\* 如果子进程没有结束，而父进程结束了，那么子进程会被init进程收养，这种进程叫孤儿进程，孤儿进程并没有什么危害。

## Usage Test

### View Process State

通过ps查看

```
ps -S
```

通过top查看

```
top
```

通过/proc查看

```
cat /proc/<pid>/status | grep State
```

状态描述：

```
D    uninterruptible sleep (usually IO)
I    Idle kernel thread
R    running or runnable (on run queue)
S    interruptible sleep (waiting for an event to complete)
T    stopped by job control signal
t    stopped by debugger during the tracing
X    dead (should never be seen)
Z    defunct ("zombie") process, terminated but not reaped by its parent
P    Parked
W    paging (not valid since the 2.6.xx kernel)
```

通过sched debug查看

```
cat /sys/kernel/debug/sched/debug
```

### Stop a Process

暂停进程（SIGSTOP）

```
kill -SIGSTOP <pid>
```
or
```
CTRL + Z
```

查看暂停进程

```
ps -S | grep " T "
```

恢复进程（SIGSCONT）

```
kill -SIGCONT <pid>
```

进程前后台切换

```
bg		// 查看后台进程
fg <id>		// 将进程切换回前台
```

Reference:

[How To Use Bash's Job Control to Manage Foreground and Background Processes](https://www.digitalocean.com/community/tutorials/how-to-use-bash-s-job-control-to-manage-foreground-and-background-processes)
