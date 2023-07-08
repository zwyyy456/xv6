#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

#define kAccessBit 6
#define kAll1 0xffffffffffffffff
uint64
sys_exit(void) {
    int n;
    if (argint(0, &n) < 0)
        return -1;
    exit(n);
    return 0; // not reached
}

uint64
sys_getpid(void) {
    return myproc()->pid;
}

uint64
sys_fork(void) {
    return fork();
}

uint64
sys_wait(void) {
    uint64 p;
    if (argaddr(0, &p) < 0)
        return -1;
    return wait(p);
}

uint64
sys_sbrk(void) {
    int addr;
    int n;

    if (argint(0, &n) < 0)
        return -1;

    addr = myproc()->sz;
    if (growproc(n) < 0)
        return -1;
    return addr;
}

uint64
sys_sleep(void) {
    int n;
    uint ticks0;

    if (argint(0, &n) < 0)
        return -1;
    acquire(&tickslock);
    ticks0 = ticks;
    while (ticks - ticks0 < n) {
        if (myproc()->killed) {
            release(&tickslock);
            return -1;
        }
        sleep(&ticks, &tickslock);
    }
    release(&tickslock);
    return 0;
}

#ifdef LAB_PGTBL
int sys_pgaccess(void) {
    // lab pgtbl: your code here.
    // since abits is uint, 32 bits, so we can check 32 pages once
    uint64 va; // va 就是一个地址
    uint64 mask_addr = 0;
    uint32 mask = 0;
    int n;
    struct proc *p = myproc();
    pagetable_t pt = p->pagetable;
    // get the arguments
    if (argint(1, &n) < 0) {
        return -1;
    }
    if (n > 32) {
        printf("error: check 32 pages at most for once!\n");
        return -1;
    }
    if (argaddr(0, &va) < 0) { // 这里 buf 已经被访问了一次了
        return -1;
    }
    // 清除访问 buf 时造成的 pte accessed bit 被设置
    // pte_t *p_clear_buf = walk(pt, va, 0);
    // *p_clear_buf = *p_clear_buf & (0xffffffffffffffff ^ (1 << kAccessBit));
    if (argaddr(2, &mask_addr) < 0) {
        return -1;
    }

    // walk 返回的是最后一级页表的的对应的 pte 的地址，该 pte 中就存放着真正的物理地址
    pte_t *ppte = walk(pt, va, 0); // ppte 是指向真的 pte 的地址
    for (int i = 0; i < n; ++i) {
        uint32 valid = PTE_A(ppte[i]);
        mask = mask | (valid << i);
        // printf("va: %p, n: %d, mask: %p, pte %p\n", va + i * PGSIZE, n, mask, ppte[i]);
        // *ppte = *ppte & (0xffffffffffffffff ^ (!(valid << kAccessBit)));
        ppte[i] = ppte[i] ^ (valid << kAccessBit);
    }

    // *(uint64 *)ppte = (*ppte & (kAll1 & (valid << kAccessBit))); // 清除  bit
    copyout(pt, mask_addr, (char *)(&mask), 4);
    // printf("after set: va: %p, n: %d, mask: %p, pte %p\n", va, n, mask, *ppte);
    return 0;
}
#endif

uint64
sys_kill(void) {
    int pid;

    if (argint(0, &pid) < 0)
        return -1;
    return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void) {
    uint xticks;

    acquire(&tickslock);
    xticks = ticks;
    release(&tickslock);
    return xticks;
}
