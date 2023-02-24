1. TODO: gdt.c gdt_init() 加载地址， int a[10] 其中a和&a的值是相同的，但a表示第一个元素的地址，&a表示整个数组的地址，地址运算有区别
2. TODO: 除了irq，其他都成功 已解决：kernel_main 直接退出了，所以接收不到中断请求
3. x/nx address： 显示address处的n个字->4字节
4. TODO:pmm:32行和 vmm:31行， 为什么要把pte标记为usermod， 还有为什么 kernel_dir U| RW| P 也是usermod， 那什么情况下才是supervisor mode
5. TODO: 下面访问地址触发pagefault，isr_handler 如果不处理，返回后继续执行又会出发pagefault
    ``` char *ptr = 0x10e000;
        char a = *ptr;
6. TODO: pagetable_dir->tables可以不要，因为tables只是为了直接找到页表，但页表也可以通过判断dir的present再用高20位地址访问到页表
7. TODO: test algorithm for ordered_array_t
8. TODO: 删除hole，或者增大减小hole的时候如何调整位置呢? solution: 根据*header_t在有序数组中找到该header对应的索引，然后remove或者increase或decrease
9. TODO: extract 中如何减小空间，如何处理header 在边界里面， 但footer在边界外面
10. TODO：删除kheap中的测试函数接口

11. 交叉编译gcc时的选项 --without-headers tells GCC not to rely on any C library (standard or runtime) being present for the target.

# 分页
```rust
page fault cpu会push一个err code，其中的位表示出错类型
出错地址保存在CR2寄存器中
Bit 0
    If set, 这个page是存在的，否则是page不存在
Bit 1
    If set, the operation that caused the fault was a write, else it was a read.
Bit 2
    If set, the processor was running in user-mode when it was interrupted. Else, it was running in kernel-mode.
Bit 3
    If set, the fault was caused by reserved bits being overwritten.
Bit 4
    If set, the fault occurred during an instruction fetch.

```

# Multitasking 
fork之后子进程在开始被调度时，栈有问题，返回地址不对，跳转到错误地址导致 present错误，说明从父进程复制栈的时候有问题，说明clone_table有问题

