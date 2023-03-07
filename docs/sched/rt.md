# rt

RT sched class

pick next task，pull / push
先看其他cpu上有没有高优先级的RT task，如果有（pushble可迁核的task），且当前CPU在起allow cpus中，则把这个task pull到当前cpu的RT runqueue中（用set_task_cpu做迁核操作）。然后，遍历当前runqueue，最终找到一个最高优先级的task返回，过程中，会递归runqueue中的task group。选完后，还会做push操作，如果有其他core的rt queue为空，则会将多余task推到其他核上。
选择task的核心函数是pick_next_rt_entity，runqueue是由一个priority bitmap和每个bit对应的task list组成，pick next就是找到最高优先级的bit所对应的list，从list中pick出第一个task。

schedule policy FIFO / RR
task_tick_rt回调中处理了RR策略，如果是RR策略，会在每个tick时不断将当前任务加到队尾。如果是FIFO，则使用默认处理流程，完全按照优先级执行，先来的任务会被处理完毕，再选取下一个。
