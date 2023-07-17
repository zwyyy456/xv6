// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
    struct run *next;
};

struct {
    struct spinlock lock;
    struct run *freelist;
} kmem;

struct {
    struct spinlock lock;
    uint8 ref_arr[PHYSTOP / PGSIZE];
} kref_cnt;

void kinit() {
    initlock(&kmem.lock, "kmem");
    initlock(&kref_cnt.lock, "kfre_cnt");
    memset(kref_cnt.ref_arr, 0, PHYSTOP / PGSIZE);
    freerange(end, (void *)PHYSTOP);
}

uint8 get_ref(uint64 paddr) {
    return kref_cnt.ref_arr[paddr / PGSIZE];
}

void dec_ref(uint64 paddr) {
    if (kref_cnt.ref_arr[paddr / PGSIZE] == 0) {
        return;
    }
    acquire(&kref_cnt.lock);
    kref_cnt.ref_arr[paddr / PGSIZE] -= 1;
    release(&kref_cnt.lock);
}

void inc_ref(uint64 paddr) {
    acquire(&kref_cnt.lock);
    kref_cnt.ref_arr[paddr / PGSIZE] += 1;
    release(&kref_cnt.lock);
}

void set_ref(uint64 paddr, uint8 val) {
    acquire(&kref_cnt.lock);
    kref_cnt.ref_arr[paddr / PGSIZE] = val;
    release(&kref_cnt.lock);
}

void freerange(void *pa_start, void *pa_end) {
    char *p;
    p = (char *)PGROUNDUP((uint64)pa_start);
    for (; p + PGSIZE <= (char *)pa_end; p += PGSIZE)
        kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
// 修改 kfree，当计数大于 1 时，递减计数；否则直接置零
void kfree(void *pa) {
    if (get_ref((uint64)pa) > 1) {
        dec_ref((uint64)pa);
        return;
    }
    set_ref((uint64)pa, 0);
    struct run *r;

    if (((uint64)pa % PGSIZE) != 0 || (char *)pa < end || (uint64)pa >= PHYSTOP)
        panic("kfree");

    // Fill with junk to catch dangling refs.
    memset(pa, 1, PGSIZE);

    r = (struct run *)pa;

    acquire(&kmem.lock);
    r->next = kmem.freelist;
    kmem.freelist = r;
    release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *kalloc(void) {
    struct run *r;

    acquire(&kmem.lock);
    r = kmem.freelist;
    if (r)
        kmem.freelist = r->next;
    release(&kmem.lock);

    if (r)
        memset((char *)r, 5, PGSIZE); // fill with junk
    set_ref((uint64)r, 1);
    return (void *)r;
}
