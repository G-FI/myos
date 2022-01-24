1. TODO: gdt.c gdt_init() 加载地址， int a[10] 其中a和&a的值是相同的，但a表示第一个元素的地址，&a表示整个数组的地址，地址运算有区别
2. TODO: 除了irq，其他都成功 已解决：kernel_main 直接退出了，所以接收不到中断请求
3. x/nx address： 显示address处的n个字->4字节
4. TODO:pmm:32行和 vmm:31行， 为什么要把pte标记为usermod， 还有为什么 kernel_dir U| RW| P 也是usermod， 那什么情况下才是supervisor mode
5. TODO: 下面访问地址触发pagefault，isr_handler 如果不处理，返回后继续执行又会出发pagefault
    ``` char *ptr = 0x10e000;
        char a = *ptr;
6. TODO: pagetable_dir->tables可以不要，因为tables只是为了直接找到页表，但页表也可以通过判断dir的present再用高20位地址访问到页表
