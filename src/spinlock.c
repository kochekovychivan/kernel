#include"defs.h"

static inline u32
xchg(volatile u32* addr, u32 new_val)
{
  u32 res;

  __asm__ volatile("lock; xchgl %0, %1" : "+m" (*addr), "=a" (res) : "1" (new_val) : "cc");

  return res;
}

void 
spin_lock(struct spinlock *lk)
{
  while(xchg(&lk->locked, 1) != 0); 
}

void 
spin_unlock(struct spinlock *lk)
{
  __sync_synchronize();
  lk->locked = 0;
}

void 
spin_init(struct spinlock *lk)
{
  lk->locked = 0;
}
